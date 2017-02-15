#include "graph.h"


Graph* create_graph (int graphType) {
    
    int    i;
    Graph* graph;

    graph = NULL;																				//allocating space for the graph	
																										//calling constructors for each attributes
    if (!(graph = malloc (sizeof(Graph)))) {

        fprintf (stderr, "Error: creating graph\n");
        return NULL;
    }

    if (!(graph->buffer_out = createBuffer ())) {
        
        fprintf (stderr, "Error: creating graph\n");
        return NULL;
    }

    if (!(graph->index_out = createNodeIndex ())) {

        fprintf (stderr, "Error: creating graph\n");
        return NULL;
    }

    if (!(graph->buffer_inc = createBuffer ())) {

        fprintf (stderr, "Error: creating graph\n");
        return NULL;
    }

    if (!(graph->index_inc = createNodeIndex ())) {

        fprintf (stderr, "Error: creating graph\n");
        return NULL;
    }

    if (graphType == DYNAMIC) {

        if (!(graph->components = malloc (sizeof(CC)))) {

            fprintf (stderr, "Error: creating graph\n");
            return NULL;
        }
		
        graph->scc = NULL;
        graph->components->ccindex = NULL;
        graph->components->hyperGraph = NULL;

        pthread_mutex_init (&(graph->components->search_mutex), NULL);
    }
    else if (graphType == STATIC) {

        if (!(graph->scc = malloc (sizeof(SCC)))) {

            fprintf(stderr, "Error: creating graph\n");
            return NULL;
        }

        graph->components = NULL;
        graph->scc->components = NULL;
        graph->scc->components_count = 0;
        graph->scc->id_belongs_to_component = NULL;
        graph->scc->hyperGraph = NULL;
    }
    else {

        graph->scc = NULL;
        graph->components = NULL;
    }
	
    return graph;

}

int insertEdge (Graph* graph, HT_info* ht_info, uint32_t from_nodeId ,uint32_t to_nodeId, uint32_t version, int insertFlag) {

    int retval, i, hypeInsert, prevSize;

    prevSize = graph->index_inc->maxNodes;
    retval = insertNode (graph->index_out, graph->buffer_out, ht_info, from_nodeId, to_nodeId, version, insertFlag);

    if (retval >= 0){
        
        retval = insertNode (graph->index_inc, graph->buffer_inc, NULL, to_nodeId, from_nodeId, version, NO_CHECKING);
    }
    else {
        
        return FAILURE;
    }

    if (insertFlag == ADDITION) {

        if (retval == OK_REALLOC) {

            if (!(graph->components->ccindex = realloc(graph->components->ccindex, graph->index_inc->maxNodes * sizeof(uint32_t)))) {
            	
                fprintf(stderr, "Error: reallocating connected components' index in insertEdge\n");
                return FAILURE;
            }

            for (i = prevSize; i < graph->index_inc->maxNodes ; i++) {
                
                graph->components->ccindex[i] = NOT_EXISTS;
            }

            if (from_nodeId < to_nodeId) {

                if (from_nodeId < prevSize) {

                    graph->components->ccindex[to_nodeId] = findNodeConnectedComponentID (graph->components, from_nodeId);
                }
                else {

                    (graph->components->maxCC)++;
                    graph->components->ccindex[to_nodeId] = graph->components->maxCC;
                    graph->components->ccindex[from_nodeId] = graph->components->maxCC;
                }
            }
            else {

                if(to_nodeId < prevSize) {

                    graph->components->ccindex[from_nodeId] = findNodeConnectedComponentID (graph->components, to_nodeId);
                }
                else {

                    (graph->components->maxCC)++;
                    graph->components->ccindex[to_nodeId] = graph->components->maxCC;
                    graph->components->ccindex[from_nodeId] = graph->components->maxCC;
                }
            }
        }
    

        if(retval != FAILURE) {

            if (findNodeConnectedComponentID (graph->components, from_nodeId) != findNodeConnectedComponentID (graph->components, to_nodeId)) {

                retval = insertNewEdge (graph->components, 
                                        findNodeConnectedComponentID (graph->components, from_nodeId),
                                        findNodeConnectedComponentID (graph->components, to_nodeId),
                                        version);
            }
        }
    }

    return retval;

}



int path (Graph* graph, uint32_t start, uint32_t goal, int* incExplored, int* outExplored, int* counter, int graphType, uint32_t version, Queue * incQueue, Queue * outQueue ) {													//implement bidirectional breadth first search algorithm in C 

    int      i, j, tmp, componentsAreConnected, outDepth, incDepth, incNumNeighbors, outNumNeighbors;
    qnode    node_out, node_inc;
    uint32_t neighbor, edgeProperty;
	ptr      offset;
	
    if (start == goal) {
        
        return 0;
    }
    else if (graphType == GRAPH) {
		
        if ((graph->index_out->numNeighbors[start] == 0) || (graph->index_inc->numNeighbors[goal] == 0)) {																	//if S or G have no neighbors, there is no need for BFS

            return -1;
        }

        if (findNodeConnectedComponentID (graph->components, start) != findNodeConnectedComponentID (graph->components, goal)) {

            componentsAreConnected = path (graph->components->hyperGraph, 
                                           findNodeConnectedComponentID(graph->components, start), 
                                           findNodeConnectedComponentID(graph->components, goal),
                                           incExplored, outExplored, counter,  HYPERGRAPH, version, incQueue, outQueue);
            (*counter)++;
            pthread_mutex_lock(&(graph->components->search_mutex));
            (graph->components->totalSearches)++;

            if(componentsAreConnected == -1) {

                pthread_mutex_unlock(&(graph->components->search_mutex));
                return -1;
            }
            else {
				
                (graph->components->totalSuccSearches)++;	
            }

            pthread_mutex_unlock(&(graph->components->search_mutex));
        }
    }																							
    else {

        if ((start >= graph->index_out->maxNodes) || (goal >= graph->index_out->maxNodes)) {

            return -1;
        }
        else if ((graph->index_out->array[start] == NOT_EXISTS) || (graph->index_out->array[goal] == NOT_EXISTS)) {

            return -1;
        }
    }

	if (!incQueue || !outQueue) {

        fprintf (stderr,"Error in BFS , at least one Queue has not been initialised properly.\n");
        return FAILURE;
    }

    node_out = start;
    node_inc = goal;

    queue_push (outQueue, start);																			//push S node in this one
    queue_push (incQueue, goal);																				//push G node in the other one
    outExplored[start] = *counter;
    incExplored[goal] = *counter;
    outDepth = incDepth = 0;
    incNumNeighbors = graph->index_inc->numNeighbors[goal];
    outNumNeighbors = graph->index_out->numNeighbors[start];

    while (1) {

        if (incNumNeighbors < outNumNeighbors) {

            incDepth ++;
            tmp = incQueue->num;
            
            for (i = 0; i < tmp; i++) {

                node_inc = queue_pop (incQueue);	
                offset = getListHead (graph->index_inc, node_inc);

                while (1) {

                    for (j = 0; j < NEIGHBORS; j++) {

                        neighbor = graph->buffer_inc->array[offset].neighbor[j];
                        edgeProperty = graph->buffer_inc->array[offset].edgeProperty[j];
						
                        if ((neighbor != EMPTY_NEIGHBOR) && (edgeProperty <= version)) {

                            if (outExplored[neighbor] == *counter) {

                                empty_queue(incQueue);
                                empty_queue(outQueue);
                                return (incDepth + outDepth);
                            }
									
                            if (incExplored[neighbor] < *counter) {

                                incExplored[neighbor] = *counter;
    
                                if(graph->index_inc->numNeighbors[neighbor] > 0) {

                                    queue_push(incQueue, neighbor);
                                }
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

            if (!incQueue->num) {

                empty_queue (incQueue);
                empty_queue (outQueue);
				
                return -1;
            }

            i = incQueue->first;
            incNumNeighbors = graph->index_inc->numNeighbors[incQueue->array[i]];
			
            while (i != incQueue->last) {

                i++;
                if (i == incQueue->size) {

                    i = 0;
                }
                incNumNeighbors += graph->index_inc->numNeighbors[incQueue->array[i]];
            }
        }
        else {

            outDepth ++;
            tmp = outQueue->num;
			
            for (i = 0; i < tmp; i++) {
				
                node_out = queue_pop (outQueue);	
                offset = getListHead (graph->index_out, node_out);

                while (1) {

                    for (j = 0; j < NEIGHBORS; j++) {

                        neighbor = graph->buffer_out->array[offset].neighbor[j];
                        edgeProperty = graph->buffer_out->array[offset].edgeProperty[j];

                        if ((neighbor != EMPTY_NEIGHBOR) && (edgeProperty <= version)) {
    
                            if (incExplored[neighbor] == *counter) {

                                empty_queue (incQueue);
                                empty_queue (outQueue);
								
                                return (incDepth + outDepth);
                            
                            }
									
                            if (outExplored[neighbor] < *counter) {

                                outExplored[neighbor] = *counter;

                                if (graph->index_out->numNeighbors[neighbor]) {

                                    queue_push(outQueue, neighbor);
                                }
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

            if (!outQueue->num) {

                empty_queue(incQueue);
                empty_queue(outQueue);
                
                return -1;
            }

            i = outQueue->first;
            outNumNeighbors = graph->index_out->numNeighbors[outQueue->array[i]];

            while (i != outQueue->last) {
				
                i++;

                if (i == outQueue->size) {

                    i = 0;
                }
                outNumNeighbors += graph->index_out->numNeighbors[outQueue->array[i]];
            }
        }
    }
}


int destroy_graph (Graph* graph) {

    destroyBuffer (graph->buffer_out);
    destroyNodeIndex (graph->index_out);
	
    destroyBuffer (graph->buffer_inc);
    destroyNodeIndex (graph->index_inc);

    if (graph->components)
        destroyConnectedComponents (graph->components);
	
    if (graph->scc)
        destroyStronglyConnectedComponents (graph->scc);
    
    free (graph);
    
    return OK_SUCCESS;

}

int job_distributor(void *argp, int callerID) {

    Arguments   *args;
    Graph       *graph;
    Queue       *incQueue, *outQueue;
    HT_info     *ht_info;
    int         *incExplored, *outExplored, *visited;
    int         type, retval;
    uint32_t    from_id, to_id, version;
    
    args = argp;
    graph = args->graph;
    incQueue = args->incQueue[callerID];
    outQueue = args->outQueue[callerID];
    ht_info = args->ht_info;
    incExplored = args->incExplored[callerID];
    outExplored = args->outExplored[callerID];
    visited = &(args->visited[callerID]);
    type = args->type;
    from_id = args->from_id;
    to_id = args->to_id;
    version = args->version;
    (*visited)++;

    if (type == STATIC) {

        if ((graph->index_out->array[from_id] == NOT_EXISTS) || (graph->index_out->array[to_id] == NOT_EXISTS)) {

            retval = -1;
        }
        else {

            if (isReachableGrailIndex(graph->scc, from_id, to_id) == MAYBE) {
                
                retval = estimateShortestPathStronglyConnectedComponents (graph, from_id, to_id, incExplored,outExplored, visited, incQueue, outQueue);
            }
            else {

                retval = -1;
            }
        }
    } 
    else if (type == DYNAMIC) {

        retval = path(graph, from_id, to_id, incExplored, outExplored, visited, GRAPH, version, incQueue, outQueue);
    }

    return retval;

}
