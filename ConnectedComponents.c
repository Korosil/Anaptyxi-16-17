#include "graph.h"


int estimateConnectedComponents (Graph* graph) {

    uint32_t   i, neighbor, components = 0;
    int        j;
    qnode      node;
    ptr        offset;
    Queue      *frontier;

    if (!(graph->components->ccindex = malloc (graph->index_inc->maxNodes * sizeof(uint32_t)))) {

        fprintf(stderr, "Error: allocating connectedComponents array\n");
        return FAILURE;
    }

    for (i = 0; i < graph->index_inc->maxNodes; i++) {

        graph->components->ccindex[i] = NOT_EXISTS;
    }

    frontier = create_queue();
    i = 0;

    while (1) {

        for (i = i; i < graph->index_inc->maxNodes; i++) {                                           //continue from the last node the iteration was
                                                                                                     //to check if the node belongs to a component
            if ((graph->components->ccindex[i] == NOT_EXISTS) && (graph->index_inc->array[i] != NOT_EXISTS)) {

                break;
            }
        }

        if (i == graph->index_inc->maxNodes) {                                                       //if we are at the last node of the graph , we finish

            fprintf (stderr,"Finished estimating wcc, found : %d\n",components );
            break;

        }

        graph->components->ccindex[i] = ++components;
        queue_push (frontier,i);				

        while (frontier->first > -1) {                                                                //start a BFS from the node we found not belonging
                                                                                                      //to a component, pushing both inc and outcoming
            node = queue_pop (frontier);                                                              //neighbors, finding all nodes of this component
            offset = getListHead(graph->index_inc, node);                                             //push all "parents"

            if (offset != EMPTY_NODE) {

                while (1) {

                    for (j = 0; j < NEIGHBORS; j++) {

                        neighbor = graph->buffer_inc->array[offset].neighbor[j];

                        if (neighbor != EMPTY_NEIGHBOR) {			
							
                            if (graph->components->ccindex[neighbor] == NOT_EXISTS) {

                                graph->components->ccindex[neighbor] = components;
                                queue_push(frontier, neighbor);
                            }
                        } 
                        else {

                            break;
                        }
                    }

                    offset = graph->buffer_inc->array[offset].nextListNode;

                    if (offset == NOT_EXISTS) {

                        break;
                    }
                }
            }
			
            offset = getListHead(graph->index_out, node);                                              //push all "children"
			
            if (offset != EMPTY_NODE) {

                while (1) {

                    for (j = 0; j < NEIGHBORS; j++) {

                        neighbor = graph->buffer_out->array[offset].neighbor[j];
                        
                        if (neighbor != EMPTY_NEIGHBOR) {			
						
                            if (graph->components->ccindex[neighbor] == NOT_EXISTS) {

                                graph->components->ccindex[neighbor] = components;
                                queue_push(frontier, neighbor);
                            }
                        }
                        else { 

                            break;
                        }
                    }

                    offset = graph->buffer_out->array[offset].nextListNode;

                    if (offset == NOT_EXISTS) {
                        
                        break;
                    }
                }
            }
        }	
    }	

    destroy_queue (frontier);
    
    graph->components->hyperGraph = create_graph (HYPERGRAPH);                           //create an empty "hypergraph"
    graph->components->maxCC = components;                                               //equivalent to update index
    graph->components->totalSearches = 0;                                                //the edges of this graph will be the connections
    graph->components->totalSuccSearches = 0;                                            //between the components

    return OK_SUCCESS;

}


int insertNewEdge(CC* components, uint32_t nodeIdS, uint32_t nodeIdE, uint32_t version)  //insert an edge to hypergraph(update index)
{
    int retval; 

    retval = insertEdge (components->hyperGraph, NULL, nodeIdS, nodeIdE, version, LINEAR_SEARCH);
	
    if(retval != OK_SUCCESS) {

        return FAILURE;
    }
    else {

        retval = insertEdge (components->hyperGraph, NULL, nodeIdE, nodeIdS, version, NO_CHECKING);
    }

    return retval;

}


int findNodeConnectedComponentID (CC* components, uint32_t nodeId) {

    return components->ccindex[nodeId];

}


int rebuildIndexes (Graph* graph) {

    fprintf (stderr,"Rebuilding Connected components\n");

    destroy_graph (graph->components->hyperGraph);

    if (graph->components->ccindex) {
        
        free (graph->components->ccindex);
    }

    graph->components->maxCC = 0;
	
    return estimateConnectedComponents (graph);

}


int destroyConnectedComponents(CC* components) {

    if (components->ccindex) {

        free (components->ccindex);
    }

    if (components->hyperGraph) {

        destroy_graph (components->hyperGraph);
    }

    free (components);
	
    return OK_SUCCESS;

}
