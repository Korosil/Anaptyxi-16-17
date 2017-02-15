#include "index.h"
#include "queue.h"
#include "stack.h"
#include "ConnectedComponents.h"
#include "SCC.h"

typedef struct graph {

       Buffer    *buffer_inc;               //buffer for incoming edges
    NodeIndex    *index_inc;                //index for incoming edges
       Buffer    *buffer_out;               //buffer for outcoming edges
    NodeIndex    *index_out;                //index for outcoming edges
           CC    *components;               //CC struct for dynamic graph mode
          SCC    *scc;                      //SCC struct for static graph mode

} Graph;


/****************************************** PROTOTYPES ******************************************/

Graph*    create_graph (int);                                                                   //creates and initializes a graph
   int    insertEdge (Graph*, HT_info*, uint32_t, uint32_t, uint32_t, int);                     //inserts an edge connecting the given nodes 
                                                                                                //(uses hashtable to indicate if it already existed)

   int    path (Graph*, uint32_t, uint32_t, int*, int*, int*, int, uint32_t, Queue*, Queue*);   //find the shortest path between the given nodes
                                                                                                //(if there is not a path connecting them, returns -1)

   int    destroy_graph (Graph*);                                                               //destroys a graph by deallocating used memory

/************************************************************************************************/


typedef struct Arguments {                  //struct containing all possible arguments a job may need to either run
	                                        //a path function in a static or dynamic graph

    struct graph    *graph;
    struct Queue    **incQueue;
    struct Queue    **outQueue;
  struct ht_info    *ht_info;
        uint32_t    from_id;
        uint32_t    to_id;
             int    *visited;
             int    **incExplored;
             int    **outExplored;
             int    type;                   //indicates whether the path that "this" job will call is going to be in a static or a dynamic graph
        uint32_t    version;                //version of edge property for the current paths neighbors

} Arguments;


/****************************************** PROTOTYPES ******************************************/

int    job_distributor (void*, int);        //takes a struct Arguments pointer and the "ID" of the calling thread
                                            //checks what type of graph the job refers to and uses the id of the thread to
                                            //indicate which frontier queues and explored info arrays are equivalent to the caller thread
                                            //(we have as many frontiers and explored sets as the threads number 
                                            //given to main from command line arguments). This way multiple threads will run seperate path queries
                                            //simultaneously and each uses each own frontier and explored set.
