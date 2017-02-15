#include "header.h"

#define NEIGHBORS          5                 //size of the neighbor array in a buffer "line"
#define INIT_BUFFER_SIZE   30                //number of "lines" the buffer has when it is first created


typedef struct List_node {

   uint32_t    neighbor [NEIGHBORS];         //the ids of the neighbor nodes
   uint32_t    edgeProperty [NEIGHBORS];     //property for each edge
        ptr    nextListNode;                 //offset pointer to the next "line" of the buffer for this node

} List_node;

typedef struct buffer {

        int    bufSize;                      //current number of "lines" in the buffer
        int    firstFree;                    //the first empty "line" of the buffer
  List_node    *array;                       //the array of "lines" in the buffer
    
} Buffer;


/****************************************** PROTOTYPES ******************************************/



   Buffer*    createBuffer (void);           //allocates the initial buffer, fills it with initial values and 
                                             //returns a pointer to it

       ptr    allocNewNode (Buffer*);        //returns the offset of the first free buffer "line"
                                             //if this offset is the last available "line" of the buffer
                                             //this function reallocates the buffer and doubles its size

List_node*    getListNode (ptr, Buffer);     //returns a pointer to a "line" of the buffer with the offset
                                             //given as an argument

       int    destroyBuffer (Buffer*);       //frees the memory from the buffer and its components
