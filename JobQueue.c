#include "header.h"
#include "JobQueue.h"


JobQueue* create_Jobqueue () {

    JobQueue* queue;

    if (!(queue = malloc (sizeof(JobQueue)))) {

        fprintf(stderr,"Error: allocating job queue\n");
        return NULL;
    }

    if (!(queue->array = malloc (INIT_JOBQUEUE_SIZE * sizeof(qjnode)))) { 

        fprintf(stderr,"Error: allocating job queue\n");
        return NULL;
    }

    queue->first = -1;
    queue->last = -1;
    queue->size = INIT_JOBQUEUE_SIZE;
    queue->num = 0;

    pthread_mutex_init (&(queue->hasjobs), NULL);
    pthread_mutex_init (&(queue->execjob), NULL);
    pthread_cond_init (&(queue->cond), NULL);

    return queue;

}


void destroy_Jobqueue (JobQueue* queue) {

    pthread_cond_destroy (&(queue->cond));
    free (queue->array);
    free (queue);

    return;

}


void Jobqueue_push (JobQueue* queue, qjnode job) {
	
    int size, first, next;

    size = queue->size;
    first = queue->first;
    next = queue->last + 1;

    if (next == queue->size) {                                               //if last element of queue was in the last position of the array
                                                                             //the "newly" added element should be added at the start of the array
        next = 0;                                                            //(circle array format)
    }

    if (next == queue->first) {                                              //if queue array is full (last + 1 == first), we need to realloc the
                                                                             //queue's array

        if (!(queue->array= realloc (queue->array, 2 * queue->size * sizeof(qjnode)))) {

            fprintf(stderr,"Error: reallocating job queue\n");
            return ;
        }
		
        if (queue->last != queue->size - 1) {                                //when reallocing the queue's array , we fix the position of the elements
                                                                             //inside the array (cause of circle format)
            memmove (&(queue->array[size + first]), &(queue->array[first]), (size - first) * sizeof(qjnode));			
            queue->first = size + first;
        }
		
        queue->size *= 2;
    }

    queue->last++;
    
    if (queue->last == queue->size) {

        queue->last = 0;
    }

    queue->array[queue->last] = job;
	
    if (first == -1) {
        
        queue->first = 0;
    }

    queue->num ++;

}


qjnode Jobqueue_pop (JobQueue* queue) {

    qjnode retval;

    if (queue->first == -1) {                                                   //if queue is empty return NULL
        return NULL;
    }
	
    retval = queue->array[queue->first];

    if (queue->first == queue->last) {

        queue->first = -1;
        queue->last = -1;
    }
    else {	
        queue->first++;
	
        if (queue->first == queue->size) { 

            queue->first = 0;
        }
    }

    queue->num --; 

    return retval;

}


void empty_Jobqueue (JobQueue* queue) {

    queue->first = -1;
    queue->last = -1;
    queue->num = 0;

}
