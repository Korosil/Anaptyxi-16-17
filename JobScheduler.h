#include "JobQueue.h"


typedef struct JobScheduler {

              int    execution_threads;               // number of worker threads
         JobQueue    *q;                              // a queue that holds submitted jobs / tasks
        pthread_t    *tids;                           // worker threads pthreadIDs array
  pthread_mutex_t    jobs_mutex;                      // mutex used for changing jobs_left value and sending singal 
                                                      // to main thread that the last job is finished
   pthread_cond_t    jobs_finished;                   // main thread waits on this condition variable after submiting 
                                                      // queries to scheduler until they are all finished
              int    jobs_left;                       // number of non finished jobs (a thread decreases it after completing a job)
              int    keep_threads_alive;              // "boolean" value indicating the worker threads should exit or keep working 
                                                      // (when it sets to 0 by main thread workers deallocate their arguments and exit)
              int    *index_results;                  // an array containing the results of finished jobs for each batch 
                                                      // (printed by main after batch is finished)
    
} JobScheduler;


typedef struct ThreadWorksArguments {                 //arguments for threads function

     JobScheduler    *sch;
              int    threadID;

}threadArgs;


/****************************************** PROTOTYPES ******************************************/

JobScheduler*    initialize_scheduler (int);               //allocate and initialize scheduler struct contents and create worker threads
         void    submit_job (JobScheduler*, Job*);         //pushes a job in schedulers queue
         void    execute_all_jobs (JobScheduler*, int);    //main thread signals the workers to start pulling jobs from queue and 
                                                           //execute them, call wait_all_tsasks_finish() and print results
         void    wait_all_tasks_finish (JobScheduler*);    //waits all submitted tasks to finish
          int    destroy_scheduler (JobScheduler*);        //deallocate used memory for job scheduler struct
        void*    thread_work (void*);                      //worker threads routine
