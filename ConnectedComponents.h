typedef struct CC {

         uint32_t    *ccindex;                                  //an array that indicates the component of each node
     struct graph    *hyperGraph;                               //equivalent to the update index (in struct graph form)
  pthread_mutex_t    search_mutex;                              //mutex used for accessing the variables of "this" struct
         uint32_t    totalSearches;                             //total path searches made in the "update index" (hypergraph) till ths point
         uint32_t    totalSuccSearches;                         //total succesfull path searches in hyper graph
              int    maxCC;                                     //size of ccindex array (can be > number of components. Used for reallocating)

} CC;


/****************************************** PROTOTYPES ******************************************/

int    estimateConnectedComponents (struct graph*);             //runs BFS in initial graph and creates-initializes the struct CC 
                                                                //(returns succes or failure)
int    insertNewEdge (CC*, uint32_t, uint32_t, uint32_t);       //inserts a new edge in hypergraph (update index) that connects two components
                                                                //if they weren't already connected
int    findNodeConnectedComponentID (CC*, uint32_t);            //returns a component the node with given ID belongs to
int    rebuildIndexes (struct graph*);                          //recalls estimateConnectedComponents() to rebuild the CC index when needed
                                                                //(when totalSuccSearches/totalSearches > metric_val)
int    destroyConnectedComponents (CC*);                        //deallocates CC structs used memory
