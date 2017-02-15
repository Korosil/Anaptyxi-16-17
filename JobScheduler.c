#include "JobScheduler.h"
#include "queue.h"


JobScheduler* initialize_scheduler (int execution_threads) {

    JobScheduler   *scheduler;
    threadArgs     *args;
    int            i; 

    if(!(scheduler = malloc (sizeof(JobScheduler)))) {

        fprintf(stderr, "Error: allocating scheduler\n");
        return NULL;

    }

    scheduler->execution_threads = execution_threads;
    scheduler->q = create_Jobqueue ();
    scheduler->index_results = NULL;

    if (!(scheduler->tids = malloc (execution_threads * sizeof(pthread_t)))) {

        fprintf(stderr, "%s\n","error initialize_scheduler" );
        return NULL;

    }

    scheduler->keep_threads_alive = 1;
    scheduler->jobs_left = 0;

    pthread_mutex_init (&(scheduler->jobs_mutex), NULL);
    pthread_cond_init (&(scheduler->jobs_finished), NULL);

    for (i = 0; i < execution_threads; i++) {                                               //for every thread

        if (!(args = malloc (sizeof(threadArgs)))) {                                        //allocate arguments needed for each threads routine
                                                                                            //a pointer to scheduler and an ID (0 - number of workers)
            fprintf(stderr, "Error: allocating thread work arguments\n");
            return NULL;

        }
        args->sch = scheduler;
        args->threadID = i;
        pthread_create (&(scheduler->tids[i]), NULL, (void *)thread_work, args);

    }

    return scheduler;

}


void submit_job (JobScheduler* sch, Job* j) {

    Jobqueue_push (sch->q, j);

}

void execute_all_jobs (JobScheduler* sch, int job_counter) {

    int i;

    pthread_mutex_lock (&(sch->q->hasjobs));

    sch->jobs_left = job_counter;
    if (!(sch->index_results = malloc (job_counter * sizeof(int)))) {

        fprintf (stderr, "%s\n", "Error: allocating index results\n");
        return ;

    }

    pthread_cond_broadcast (&(sch->q->cond));                                    //signals workers to wake up and execute jobs
	pthread_mutex_unlock (&(sch->q->hasjobs));
    wait_all_tasks_finish (sch);
    //pthread_mutex_lock (&(sch->q->execjob));

	for (i = 0; i < job_counter; i++) {

        printf ("%d\n",sch->index_results[i] );                                   //print the results in the correct order

    }

    free (sch->index_results);

}


void wait_all_tasks_finish( JobScheduler* sch) {			                      //waits all submitted tasks to finish

    pthread_mutex_lock (&(sch->jobs_mutex));

    while (sch->jobs_left > 0) {

        pthread_cond_wait (&(sch->jobs_finished), &(sch->jobs_mutex));

    }
	
	pthread_mutex_unlock (&(sch->jobs_mutex));
}


int destroy_scheduler (JobScheduler* sch) {

    int i;

    sch->keep_threads_alive = 0;
	
    pthread_mutex_lock (&(sch->q->hasjobs));                                     //signal all workers to wake up, but because
    pthread_cond_broadcast (&(sch->q->cond));                                    //and since the "boolean" is set to 0 they exit
    pthread_mutex_unlock (&(sch->q->hasjobs));

    for (i = 0; i < sch->execution_threads; i++) {

        pthread_join (sch->tids[i], NULL);                                       //wait for threads to exit

    }

    destroy_Jobqueue (sch->q);
    pthread_cond_destroy (&(sch->jobs_finished));
    free (sch->tids);
    free (sch);

    return OK_SUCCESS;

}


void* thread_work (void* args) {

    int           threadID, i, retval;
    JobScheduler  *sch;
    Job           *work;

    sch      = ((threadArgs *)args)->sch;
    threadID = ((threadArgs *)args)->threadID;
	
    while (sch->keep_threads_alive) {

        pthread_mutex_lock (&(sch->q->hasjobs));

        while (!sch->q->num) {

            if (!sch->keep_threads_alive) {                                    //ready to exit

                break;

            }

            pthread_cond_wait (&(sch->q->cond), &(sch->q->hasjobs));               //wait until job for execution exist
        }
		
        pthread_mutex_unlock (&(sch->q->hasjobs));

        if (sch->keep_threads_alive) {

            pthread_mutex_lock (&(sch->q->execjob));
            work = Jobqueue_pop (sch->q);
            pthread_mutex_unlock (&(sch->q->execjob));

            if (work) {

                retval = work->function (work->args, threadID);
                sch->index_results[work->job_id] = retval;
				
                free (work->args);
                free (work);				
				
                pthread_mutex_lock (&(sch->jobs_mutex));

                sch->jobs_left--;                                                 //decrease the number of jobs left
                if (!sch->jobs_left) {                                            //if jobs left == 0 signal the main thread to wake

                    pthread_cond_signal (&sch->jobs_finished);

                }

                pthread_mutex_unlock (&(sch->jobs_mutex));
            }

        }
    }

    free (args);                                                                  //free the struct with the arguments of worker function

    return NULL;                                                                

}

