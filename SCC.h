#define INIT_SCC_COUNT   100                //initial size of components array
#define INIT_SCC_SIZE    10                 //initial size of every components included_node_ids array


typedef struct Node {

       uint32_t    id;                      //nodes ID
            int    index;                   //tarjan algorithms index value
            int    lowlink;                 //tarjan algorithms lowlink value
    struct Node    *caller;                 //"father" of current node during graphs traversal in iterative tarjans algorithm 
       uint32_t    vindex;                  //index indicating in which of "this" nodes children the traversal is at current point
       uint32_t    offset;                  //offset of the line of buffer vindex indicated "child" is

} Node;


typedef struct Component {

       uint32_t    component_id;            //"this" components id
       uint32_t    included_nodes_count;    //number of nodes in component
       uint32_t    *included_node_ids;      //ids of included nodes
            int    size;                    //current size of included_node_ids array

} Component;


typedef struct ComponentCursor {

      Component    *component_ptr;          //pointer to current iteration component
            int    iter_counter;            //index of current itaration component

} ComponentCursor;


typedef struct GrailIndex {

            int    *min_rank;               //an array that is filled with the min ranks of every component  
            int    *rank;                   //an array that is filled with the ranks of every component

} GrailIndex;


typedef struct SCC {

      Component    *components;                       //Components index - a vector which stores the components information
       uint32_t    components_count;                  //number of components
       uint32_t    *id_belongs_to_component;          //inverted index
            int    size;                              //size of components array (used for realloc)
ComponentCursor    *cursor;                           //cursor struct (not used)
   struct graph    *hyperGraph;                       //a pointer to a graph struct containing hypergraphs info
     GrailIndex    grailIndex[NUM_OF_GRAIL_INDEXES];  //an array of grail indexes

} SCC;


typedef enum {                                        //enumaration return values for isReachableGrailIndex() (MAYBE == 0, NO == 1)

         MAYBE,    NO

} grailAnswer;


/****************************************** PROTOTYPES ******************************************/

        int    estimateStronglyConnectedComponents (struct graph*);                              //allocates and initializes SCC structs and calls tarjan iteration
                                                                                                 //for every node of initial graph that does not already belong to a SCC
       void    tarjan_iteration (struct graph*, Stack*, Node*, int*, int*, Node*);               //uses an iterative form of tarjans algorithm to find the strongly 
                                                                                                 //connected components in a component of the graph
        int    findNodeStronglyConnectedComponentID (SCC*, uint32_t);                            //given a nodes ID and the struct SCC, returns the SCC "this" node belongs to
        int    createHyperGraphSCC (struct graph*);                                              //allocates and initializes hypergraph struct filling its index and buffer with
                                                                                                 //existing edges
        int    estimateShortestPathStronglyConnectedComponents (struct graph* , uint32_t , uint32_t, int* , int*, int*, Queue* , Queue*);     //static graph path finding function
       

       void    iterateStronglyConnectedComponentID (SCC*, ComponentCursor*);                     //iterating through components (not used)
        int    next_StronglyConnectedComponentID (SCC*, ComponentCursor*);                       //iterating through components (not used)

        int    destroyStronglyConnectedComponents (SCC*);                                        //deallocates memory used for SCC struct
        int    buildGrailIndex (SCC*);                                                           //makes some "pseudorandom" postorder traversals to
                                                                                                 //create diffirent labeled grail indexes
        int    destroyGrailIndex (GrailIndex*);                                                  //deallocates memory used for Grail indexes structs
grailAnswer    isReachableGrailIndex (SCC*, uint32_t, uint32_t);                                 //answers wether to nodes of the hypergraph are posibly connected
       void    shuffleArray (uint32_t*, int);                                                    //shuffles the elements of a given array
