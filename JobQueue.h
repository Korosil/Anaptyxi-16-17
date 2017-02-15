#include "header.h"

#define INIT_JOBQUEUE_SIZE   100                 //initial size of queue array



typedef struct Job {

              int    job_id;
              int    (*function) (void*, int);   //function pointer          
           struct    Arguments  *args;           //function's arguments      

} Job;


typedef struct Job* qjnode;


typedef struct JobQueue {

          qjnode*    array;                      //nodes of "this" queue are saved here in a circle array format (O(1) access)
  pthread_mutex_t    hasjobs;                    //mutex used for synchronizing access to queue contents by multiple threads
  pthread_mutex_t    execjob;                    //mutex used for synchronizing push/pop by main thread and worker threads
   pthread_cond_t    cond;                       //condition variable indicating jobs existance in queue (workers wait on this 
                                                 //variable while queue is empty)
              int    first;                      //first element in queue array index
              int    last;                       //last element in queue array index
              int    size;                       //arrays current size
              int    num;                        //number of elements in queue

} JobQueue;


/****************************************** PROTOTYPES ******************************************/

JobQueue*    create_Jobqueue (void);             //allocates memory for Jobqueue struct and initializes its contents
     void    destroy_Jobqueue (JobQueue*);       //deallocates memory used by Jobqueue struct
     void    Jobqueue_push (JobQueue*, qjnode);  //push an ellement after the last in queue (realloc the array if needed)
   qjnode    Jobqueue_pop (JobQueue*);           //pop the first element of the queue
     void    empty_Jobqueue (JobQueue*);         //empty the queue from its contents by seting first an last index to -1 (initial values)
