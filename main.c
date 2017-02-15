#include "graph.h"
#include "JobScheduler.h"


int main(int argc, char  *argv[]) {

    int             i, j,num_threads, job_counter, **incExplored, **outExplored, *counter, flag, output_fd, graphType;
    char            buff[100], action;
    uint32_t        to_id, from_id, cur_version;
    FILE            *input_file, *workload_file;
    Graph           *graph;    
    HT_info         *ht_info;
    Job             *job;
    JobScheduler    *scheduler;
    Queue           **incQueue, **outQueue;

	if (argc != 9) {

        fprintf(stderr, "Wrong command line args .\n" );
        fprintf(stderr, "Format: ./exe -i \"input_file\" -o \"output_file\" -w \"workload\" -t \"num_of_threads\" \n");
        return FAILURE;
    }

    for (i = 1; i < argc; i+=2) {

        if ((!strcmp (argv[i], "-i")) || (!strcmp (argv[i], "--input"))) {

            input_file = fopen(argv[i+1], "r");
        }
        else if ((!strcmp (argv[i], "-o")) || (!strcmp (argv[i], "--output"))) {

            output_fd = open (argv[i+1], O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
            dup2 (output_fd, 1);
        }
        else if ((!strcmp (argv[i], "-t")) || (!strcmp (argv[i], "--threads"))) {

            num_threads = atoi (argv[i+1]);
        }
        else if ((!strcmp (argv[i], "-w")) || (!strcmp (argv[i], "--workload"))) { 
            
            workload_file = fopen (argv[i+1], "r");

             if (!fgets (buff, sizeof(buff), workload_file)){                                               //Read first line from workload file

                fprintf (stderr, "Error reading from file \n" );
                return FAILURE;
            
            }

            if (!strcmp (buff, "STATIC\n")) {

                graphType = STATIC;
                if (!(graph = create_graph (STATIC))) {

                    fprintf(stderr, "Programm will terminate, unable to create graph \n" );
                    return FAILURE;
                }
            } 
            else if (!strcmp (buff, "DYNAMIC\n")) {

                graphType = DYNAMIC;
                if (!(graph = create_graph (DYNAMIC))) {

                    fprintf (stderr, "Programm will terminate, unable to create graph \n" );
                    return FAILURE;
                }
            }
            else {
                
                fprintf(stderr, "Undifined graph type in workload file\n");
                return FAILURE;

            }
        } 
        else {

        	fprintf (stderr, "Wrong command line args !!\n" ); 
            return FAILURE;
        }
    }

    srand (time (NULL));
    ht_info = createHT_info ();

    while (!feof (input_file)) {

            if (!fgets (buff, sizeof(buff), input_file)){  

                fprintf (stderr, "Error reading from input file\n" );
                return FAILURE;
            } 
            if (buff[0] == 'S') {

                continue;
            }

            sscanf (buff, "%u\t%u\n", &from_id, &to_id);

            if ((insertEdge (graph, ht_info, from_id, to_id, cur_version, HASHTABLES)) == FAILURE) {

                fprintf (stderr, "Unable to insert Edge. Programm will now terminate\n" );
                return FAILURE;
            }
    }
    fprintf(stderr, "Finished storing graph\n");
    
    if (!(incQueue = malloc (num_threads * sizeof(Queue *)))) {

        fprintf(stderr, "Error allocating queue\n" );
        return FAILURE;
    }

    if (!(outQueue = malloc (num_threads * sizeof(Queue *)))) {

        fprintf(stderr, "Error allocating queue\n" );
        return FAILURE;
    }

    if (!(incExplored = malloc (num_threads * sizeof(int *)))) {

        fprintf(stderr, "Error initializing explored\n" );
        return FAILURE;
    }

    if (!(outExplored = malloc (num_threads * sizeof(int *)))) {

        fprintf(stderr, "Error initializing explored\n" );
        return FAILURE;
    }

    if (!(counter = malloc (num_threads * sizeof(int)))) {

        fprintf(stderr, "Error initializing counter\n" );
        return FAILURE;
    }

    for (i = 0; i < num_threads; i++) {

        incQueue[i] = create_queue ();
        outQueue[i] = create_queue ();

        if (!(incExplored[i] = malloc (graph->index_out->maxNodes * sizeof(int *)))) {

            fprintf (stderr, "Error initializing explored\n" );
            return FAILURE;
        }

        if (!(outExplored[i] = malloc (graph->index_out->maxNodes * sizeof(int *)))) {

            fprintf (stderr, "Error initializing explored\n" );
            return FAILURE;
        }

        for (j = 0; j < graph->index_out->maxNodes; j++) {

            incExplored[i][j] = -1;
            outExplored[i][j] = -1;
        }

        counter[i] = -1;
    }

    
           

    if (graphType == STATIC) {

        estimateStronglyConnectedComponents (graph);

        createHyperGraphSCC (graph);
        fprintf (stderr, "%s\n", "Finished creating HyperGraph for SCC");

        buildGrailIndex (graph->scc);
        fprintf (stderr, "%s\n", "Finished building Grail");

        scheduler = initialize_scheduler (num_threads);
        job_counter = 0;

        pthread_mutex_lock (&(scheduler->q->execjob));

        while (!feof (workload_file)) {
            
            if (fgets (buff, sizeof(buff), workload_file)){               
                
                if (buff[0] == 'F') {

                    pthread_mutex_unlock (&(scheduler->q->execjob));
                    execute_all_jobs (scheduler, job_counter);
                    pthread_mutex_lock (&(scheduler->q->execjob));
                    job_counter = 0;

                    continue;
                }
                job_counter++;
                sscanf (buff, "%c\t%u\t%u\n", &action, &from_id, &to_id);

                if (action == 'Q') {

                    if (!(job = malloc (sizeof(Job)))) {

                        fprintf (stderr, "Error. Cannot allocate space for new job.\n" );
                        return FAILURE;
                    }

                    job->job_id = job_counter - 1;
                    job->function = &job_distributor;

                    if (!(job->args = malloc (sizeof(Arguments)))) {

                        fprintf (stderr, "Error. Cannot allocate space for job's arguments\n" );
                        return FAILURE;
                    }

                    job->args->incQueue = incQueue;
                    job->args->outQueue = outQueue;
                    job->args->incExplored = incExplored;
                    job->args->outExplored = outExplored;
                    job->args->visited = counter;
                    job->args->graph = graph;
                    job->args->from_id = from_id;
                    job->args->to_id = to_id;
                    job->args->ht_info = ht_info;
                    job->args->type = STATIC;

                    submit_job (scheduler, job);     
                }  
                else {
                    
                    fprintf (stderr, "%s\n", "Wrong Input file action"); 
                }
            }
        }
    } else if (graphType == DYNAMIC) {
        
        cur_version = 0;
        estimateConnectedComponents (graph);
        scheduler = initialize_scheduler (num_threads);

        flag = 0;
        job_counter = 0;

        pthread_mutex_lock (&(scheduler->q->execjob));

        while (!feof (workload_file)) {

            if (fgets (buff, sizeof(buff), workload_file)) {               

                if (buff[0] == 'F') {

                    pthread_mutex_unlock (&(scheduler->q->execjob));
                    execute_all_jobs (scheduler, job_counter);
                    pthread_mutex_lock (&(scheduler->q->execjob));

                    if ((double) (graph->components->totalSuccSearches / (double) graph->components->totalSearches) > METRIC_VAL) {
                        
                        rebuildIndexes (graph);
                    }

                    job_counter = 0;
                    flag = 0;
                    continue;
                }

                sscanf (buff, "%c\t%u\t%u\n", &action, &from_id, &to_id);

                if (action == 'Q') {

                    job_counter++;
                    if (!(job = malloc (sizeof(Job)))) {

                        fprintf (stderr, "Error. Cannot allocate space for new job.\n" );
                        return FAILURE;
                    }

                    job->job_id = job_counter - 1;
                    job->function = &job_distributor;

                    if (!(job->args = malloc (sizeof(Arguments)))) {

                        fprintf (stderr, "Error. Cannot allocate space for job's arguments\n" );
                        return FAILURE;
                    }

                    job->args->incQueue = incQueue;
                    job->args->outQueue = outQueue;
                    job->args->incExplored = incExplored;
                    job->args->outExplored = outExplored;
                    job->args->visited = counter;
                    job->args->graph = graph;
                    job->args->from_id = from_id;
                    job->args->to_id = to_id;
                    job->args->ht_info = ht_info;
                    job->args->type = DYNAMIC;
                    job->args->version = cur_version;
                    flag = 1;

                    submit_job (scheduler, job);
                } 
                else if (action == 'A') {

                    if (flag) {                              // (flag == 1) indicates previous line of workload was a path query

                        cur_version++;
                    }
                    if ((insertEdge(graph, ht_info, from_id, to_id, cur_version, ADDITION)) == FAILURE) {

                        fprintf (stderr, "Unable to insert Edge. Programm will now terminate\n" );
                        return FAILURE;
                    }

                    flag = 0;
                } 
                else {

                    fprintf (stderr, "%s\n", "Wrong Input file action");
            	}
            }
        }
    }

    fclose (input_file);
    fclose (workload_file);
    close (output_fd);
    destroy_scheduler (scheduler);

    for (i = 0; i < num_threads; i++) {

        destroy_queue (incQueue[i]);
        destroy_queue (outQueue[i]);
        free (incExplored[i]);
        free (outExplored[i]);
    }

    free (incQueue);
    free (outQueue);
    free (incExplored);
    free (outExplored);
    free (counter);
    destroyHT_info (ht_info);
    destroy_graph (graph);

    return OK_SUCCESS;

}
