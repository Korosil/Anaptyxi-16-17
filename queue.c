#include "header.h"
#include "queue.h"


Queue* create_queue () {

    Queue *queue;

    if (!(queue = malloc (sizeof(Queue)))) {

		fprintf(stderr,"Error allocating queue\n");
		return NULL;

	}

	if (!(queue->array = malloc (INIT_QUEUE_SIZE * sizeof(qnode)))) {

      fprintf(stderr,"Error allocating array in queue\n");
      return NULL;

    }

    queue->first = -1;
    queue->last = -1;
    queue->size = INIT_QUEUE_SIZE;
    queue->num = 0;

    return queue;

}


void destroy_queue (Queue* queue) {

    free (queue->array);
    free (queue);

    return ;

}


void queue_push (Queue* queue, qnode nodeid) {
	
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
        if (!(queue->array = realloc (queue->array, 2 * queue->size * sizeof(qnode)))) {

            fprintf(stderr, "Error: reallocating queue array\n");
            return ;

        }

        if (queue->array == NULL) {

            fprintf(stderr,"Error re-allocating queue\n");
            return ;
		
        }

        if (queue->last != queue->size - 1) {                                //when reallocing the queue's array , we fix the position of the elements
                                                                             //inside the array (cause of circle format)
            memmove (&(queue->array[size + first]), &(queue->array[first]), (size - first) * sizeof(qnode));			
            queue->first = size + first;

        }
		
        queue->size *= 2;

    }

    queue->last++;
    if (queue->last == queue->size) {

        queue->last = 0;

    }

    queue->array[queue->last] = nodeid;
	
    if (first == -1) {

        queue->first = 0;
    }
    
    queue->num ++;
    return ;

}


qnode queue_pop (Queue* queue) {

    qnode retval = queue->array[queue->first];

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


void empty_queue (Queue* queue) {

    queue->first = -1;
    queue->last = -1;
    queue->num = 0;

}
