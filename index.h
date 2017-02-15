#include "buffer.h"
#include "hashTable.h"

#define INIT_INDEX_SIZE   30      //number of "lines" the index has when it is first created
    

typedef struct index {

     int    maxNodes;             //index's size
    ptr*    lastOffset;           //array of offsets to the last line in the buffer for each node
    ptr*    array;                //array of offsets to the first line in the buffer for each node
    int*    numNeighbors;         //number of total neighbors for each node
                                                    
} NodeIndex;


/****************************************** PROTOTYPES ******************************************/

NodeIndex*    createNodeIndex (void);                  //allocates memory for index struct and initializes its contents                               
       int    insertNode (NodeIndex*, Buffer*, HT_info*, uint32_t, uint32_t, uint32_t, int);        //inserts a non existing node to index or/and 
                                                                                                    //calls buffer level insert for its neighbor
       ptr    getListHead (NodeIndex*, uint32_t);      //returns an offset to the first buffer line refered to the given node
       int    destroyNodeIndex (NodeIndex*);           //deallocates memory used by index sturct
