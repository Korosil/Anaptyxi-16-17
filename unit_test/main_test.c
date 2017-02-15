#include "../graph.h"
#include "../JobScheduler.h"


int main(int argc, char  *argv[]) {

    int             i, j, *incExplored, *outExplored, counter, depth;
    char            buff[100], action;
    uint32_t        to_id, from_id;
    FILE            *input_file, *workload_file;
    Graph           *graph;    
    HT_info         *ht_info;
    Queue           *incQueue, *outQueue;


    
    input_file = fopen(argv[1], "r");
    workload_file = fopen (argv[2], "r");

    printf("\n\n~~~Unit testing a dynamic graph.~~~\n\n");

    if (!(graph = create_graph(DYNAMIC))) {

        fprintf(stderr, "Programm will terminate, unable to create graph \n" );
        return FAILURE;
    }


    srand(time(NULL));
    ht_info = createHT_info();

    while (!feof(input_file)) {

            if (!fgets(buff, sizeof(buff), input_file)){  

                fprintf(stderr, "Error reading from input file\n" );
                return FAILURE;
            } 
            if (buff[0] == 'S')
                continue;

            sscanf(buff, "%u %u\n", &from_id, &to_id);

            if ((insertEdge(graph, ht_info, from_id, to_id, 0, HASHTABLES)) == FAILURE) {

                fprintf(stderr, "Unable to insert Edge. Programm will now terminate\n" );
                return FAILURE;
            }
    }
    fprintf(stderr, "Finished storing graph\n");
    

    incQueue = create_queue();
    outQueue = create_queue();

    incExplored = malloc(graph->index_out->maxNodes * sizeof(int));
    outExplored = malloc(graph->index_out->maxNodes * sizeof(int));

    for (j = 0; j < graph->index_out->maxNodes; j++) {

        incExplored[j] = -1;
        outExplored[j] = -1;
    }

    counter = -1;


    if (!fgets(buff, sizeof(buff), workload_file)){                                                 //Read first line from workload file

        fprintf(stderr, "Error reading from file \n" );
        return FAILURE;
    }        

    
    estimateConnectedComponents(graph);
    printf("\n");

    while (!feof(workload_file)) {

        if (fgets(buff, sizeof(buff), workload_file)) {               

            if (buff[0] == 'F'){
                rebuildIndexes(graph);
                printf("\n");
            }

            sscanf(buff, "%c\t%u\t%u\n", &action, &from_id, &to_id);

            if (action == 'A') {

                printf("Outcoming neighbors of %u are %u , incoming neighbors of %u are %u\n",from_id, graph->index_out->numNeighbors[from_id], to_id, graph->index_inc->numNeighbors[to_id] );
                printf("Adding edge from: %u to %u\n", from_id, to_id);
                insertEdge(graph, ht_info, from_id, to_id, 0, ADDITION);
                printf("Outcoming neighbors of %u are %u , incoming neighbors of %u are %u\n",from_id, graph->index_out->numNeighbors[from_id], to_id, graph->index_inc->numNeighbors[to_id] );
                printf("\n");
                
            }
            else if( action == 'Q'){

                counter++;
                

                depth = path(graph, from_id, to_id, incExplored, outExplored, &counter, GRAPH, 0, incQueue, outQueue);
                printf("Query from: %u , to: %u . Path length is : %d\n", from_id, to_id, depth);
                printf("\n");
            }
        }
    }


    fclose(input_file);
    fclose(workload_file);

    destroy_queue(incQueue);
    destroy_queue(outQueue);

    free(incExplored);
    free(outExplored);

    destroyHT_info(ht_info);
    destroy_graph(graph);


    printf("\n\n~~~Unit testing a static graph.~~~\n\n");
    input_file = fopen(argv[3], "r");
    workload_file = fopen (argv[4], "r");

    if (!(graph = create_graph(STATIC))) {

        fprintf(stderr, "Programm will terminate, unable to create graph \n" );
        return FAILURE;
    }

    ht_info = createHT_info();

    while (!feof(input_file)) {

            if (!fgets(buff, sizeof(buff), input_file)){  

                fprintf(stderr, "Error reading from input file\n" );
                return FAILURE;
            } 
            if (buff[0] == 'S')
                continue;

            sscanf(buff, "%u\t%u\n", &from_id, &to_id);

            if ((insertEdge(graph, ht_info, from_id, to_id, 0, HASHTABLES)) == FAILURE) {

                fprintf(stderr, "Unable to insert Edge. Programm will now terminate\n" );
                return FAILURE;
            }
    }
    fprintf(stderr, "Finished storing graph\n");
    

    incQueue = create_queue();
    outQueue = create_queue();

    incExplored = malloc(graph->index_out->maxNodes * sizeof(int));
    outExplored = malloc(graph->index_out->maxNodes * sizeof(int));

    for (j = 0; j < graph->index_out->maxNodes; j++) {

        incExplored[j] = -1;
        outExplored[j] = -1;
    }

    counter = -1;


    if (!fgets(buff, sizeof(buff), workload_file)){                                                 //Read first line from workload file

        fprintf(stderr, "Error reading from file \n" );
        return FAILURE;
    }        

    
    estimateStronglyConnectedComponents(graph);

    createHyperGraphSCC(graph);
    fprintf(stderr, "%s\n", "Finished creating HyperGraph for SCCs");
    printf("Number of HyperGraph nodes is: %u\n\n", graph->scc->components_count);
    for(i=0 ; i<graph->scc->components_count; i++)
        printf("Component %d , contains %u nodes and has %d neighbors\n",i,graph->scc->components[i].included_nodes_count,
                                                                        graph->scc->hyperGraph->index_out->numNeighbors[i] );

    buildGrailIndex(graph->scc);
    fprintf(stderr, "\n%s\n\n", "Finished building Grail");

    while (!feof(workload_file)) {
        
        if (fgets(buff, sizeof(buff), workload_file)){               
            
            sscanf(buff, "%c\t%u\t%u\n", &action, &from_id, &to_id);

            if (action == 'Q') {

                printf("Query from: %u , to: %u \n", from_id, to_id);
                counter++;
                if (findNodeStronglyConnectedComponentID(graph->scc, from_id) == findNodeStronglyConnectedComponentID(graph->scc, to_id)){

                    printf("Nodes are in the same SCC\n");
                    depth = estimateShortestPathStronglyConnectedComponents(graph, from_id, to_id, incExplored,
                                                                         outExplored, &counter, incQueue, outQueue);

                } else if (isReachableGrailIndex(graph->scc, from_id, to_id) == MAYBE){
                    
                    printf("Grail answered MAYBE, searching for path\n");
                    depth = estimateShortestPathStronglyConnectedComponents(graph, from_id, to_id, incExplored, outExplored, &counter, incQueue, outQueue);
                } else {
                    
                    printf("Grail answered NO\n");
                    depth = -1;
                }
                printf("Path length is : %d\n\n", depth);
            } 
        }
    }


    fclose(input_file);
    fclose(workload_file);

    destroy_queue(incQueue);
    destroy_queue(outQueue);

    free(incExplored);
    free(outExplored);

    destroyHT_info(ht_info);
    destroy_graph(graph);

    return OK_SUCCESS;
}
