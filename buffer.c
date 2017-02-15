#include "buffer.h"


Buffer* createBuffer () {
	
    int    i, j;
    Buffer *buffer;

    if (!(buffer = malloc (sizeof(Buffer)))) {                                                          //allocating buffer struct

        fprintf (stderr, "Error: allocating buffer\n");
        return NULL;
    
    } 
    else if (!(buffer->array = malloc (INIT_BUFFER_SIZE * sizeof(List_node)))) {                        //allocating buffers array

        fprintf(stderr, "Error: allocating buffers array\n");
        return NULL;
    
    }

    buffer->firstFree = -1;                                                                             //initializing buffers info
    buffer->bufSize = INIT_BUFFER_SIZE;	

    for (j = 0; j < INIT_BUFFER_SIZE; j++) {                                                            //initializing all list nodes as empty
	
        for (i = 0; i < NEIGHBORS; i++) {

            buffer->array[j].neighbor[i] = EMPTY_NEIGHBOR;
            buffer->array[j].edgeProperty[i] = 0;
        }
        buffer->array[j].nextListNode = NOT_EXISTS;
    }

    return buffer;

}


ptr allocNewNode (Buffer* buffer) {
	
    int    i, j;

    if ((buffer->firstFree + 1) == buffer->bufSize) {                                                   //checking if buffers array is full
	
        if (!(buffer->array = realloc (buffer->array, 2 * (buffer->bufSize) * sizeof(List_node)))) {    //double the buffers array size
            fprintf(stderr, "Error: allocating new node\n");
            return FAILURE;
        }    
        for(j = buffer->bufSize ; j < 2 * buffer->bufSize ; j++) {                                      //initializing the new second half  	
                                                                                                        //of the buffer with empty values
            for (i = 0; i < NEIGHBORS; i++) {
                                     
                buffer->array[j].neighbor[i] = EMPTY_NEIGHBOR;
                buffer->array[j].edgeProperty[i] = 0;
            }

            buffer->array[j].nextListNode = NOT_EXISTS;
        }

    buffer->bufSize = 2 * buffer->bufSize;                                                              //updating the bufSize field of buffer	
    }

    buffer->firstFree += 1;

    return buffer->firstFree;                                                                           //returning the first free arrays line

}


List_node* getListNode (ptr position, Buffer buffer) {
	
    return &(buffer.array[position]);

}


int destroyBuffer (Buffer* buffer) {

    free (buffer->array);                                                                               //free the memory the array of the buffer 
    free (buffer);                                                                                      //free the memory the rest of the buffer 
	
    return OK_SUCCESS;

}
