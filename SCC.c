#include "graph.h"


int estimateStronglyConnectedComponents (Graph* graph) { 

    int    i, index, *onStack;
    Stack  *tarStack;
    Node   *nodeArray;

    index = 1;
    tarStack = create_stack();

    if (!(graph->scc->components = malloc (INIT_SCC_COUNT * sizeof(Component)))) {
        
        fprintf (stderr, "Error: allocating connected components\n");
        return FAILURE;
    }

    graph->scc->size = INIT_SCC_COUNT;

    if (!(graph->scc->id_belongs_to_component = malloc (graph->index_out->maxNodes * sizeof(uint32_t)))) {
        
        fprintf (stderr, "Error: allocating connected components\n");
        return FAILURE;
    }

    for (i = 0; i < graph->index_out->maxNodes; i++) {
        
        graph->scc->id_belongs_to_component[i] = NO_COMPONENT;
    }

    if (!(onStack = malloc (graph->index_out->maxNodes * sizeof(int)))) {

        fprintf(stderr, "Error: allocating stack\n");
        return FAILURE;
    }

    if (!(nodeArray = malloc(graph->index_out->maxNodes * sizeof(Node)))) {

        fprintf(stderr, "Error: allocating nodes' array\n");
        return FAILURE;
    }

    for (i = 0; i < graph->index_out->maxNodes; i++) {                                //initializing structs used for tarjan's algorithm

        onStack[i] = 0;
        nodeArray[i].id = i;
        nodeArray[i].lowlink = 0;
        nodeArray[i].index = 0;
        nodeArray[i].vindex = 0;
    }

    for (i = 0; i < graph->index_out->maxNodes; i++) {                                //start a tarjan iteration from every node that doesnt 
                                                                                      //belong to an SCC already
        if (graph->scc->id_belongs_to_component[i] == NO_COMPONENT) {

            if(graph->index_out->array[i] != NOT_EXISTS) {

                index = 1;
                tarjan_iteration (graph, tarStack, &nodeArray[i], &index, onStack, nodeArray);
            }
        }
    }

    destroy_stack (tarStack);
    free (onStack);
    free (nodeArray);
  	
    if (!(graph->scc->cursor = malloc(sizeof(ComponentCursor)))) {

        fprintf(stderr, "Error: allocating components' cursor\n");
        return FAILURE;
    }

    graph->scc->cursor->component_ptr = NULL;
    graph->scc->cursor->iter_counter = -1;
    fprintf(stderr, "Finished estimating scc, found: %u\n",graph->scc->components_count);

    return OK_SUCCESS;

}


void tarjan_iteration (Graph *graph, Stack *tarStack, Node *u, int *index, int *onStack, Node* nodeArray) {

    int      pos;
    uint32_t childID,topID;
    Node     *last, *w, *top, *newLast;

    u->index = *index;                                                      // u is the node we start the traversal node 
    u->lowlink = *index;
    (*index)++;
    u->vindex = 0; 
    stack_push(tarStack, u->id);
    u->caller = NULL;           											// u does not have a parent , since we started the
    onStack[u->id] = 1;                                                     //algorithm from that node
    last = u;

    while (1) {

        if (last->vindex < graph->index_out->numNeighbors[last->id]) {     

            if (!last->vindex) {                                            //if vindex = 0 (we are at the first child of nthe current node)
                                                                            //we get the buffer line where this child is
                last->offset = getListHead(graph->index_out, last->id);
            }
            else if (!(last->vindex % NEIGHBORS)) {                         //if vindex mod NEIGHBORS = 0 means that next child is
                                                                            //in a different buffer line than the previous child
                last->offset = graph->buffer_out->array[last->offset].nextListNode;  // so we need to get a new offset from the buffer
            }

            pos = last->vindex % NEIGHBORS;                                 //we get the position in the buffer line , for the current child
            childID = graph->buffer_out->array[last->offset].neighbor[pos];
            w = &nodeArray[childID];
            (last->vindex)++;                                   			//increase the index so next time we get the next child etc..

            if(!(w->index)) {                                               //set the values of the struct for the child, and push it in stack
            
                w->caller = last;                     
                w->vindex = 0;
                w->index = *index;
                w->lowlink = *index;
                (*index)++;
                stack_push(tarStack, w->id);
                onStack[w->id] = 1;
                last = w;
            } 
            else if(onStack[w->id]) {

                if(last->lowlink > w->index) {

                    last->lowlink = w->index;
                }
            }
        } 
        else {  																	//Tarjan algorithms check , to create a new SCC

            if(last->lowlink == last->index) {

                pos = (graph->scc->components_count)++;
                topID = stack_pop (tarStack);
                top = &nodeArray[topID];
                onStack[topID] = 0;
                graph->scc->id_belongs_to_component[topID] = pos;

                if (pos == graph->scc->size ) {
    
                    if (!(graph->scc->components = realloc (graph->scc->components, 2 * graph->scc->size * sizeof(Component)))) {

                        fprintf(stderr, "Error: reallocating strongly connected components\n");
                        return;
                    }

                    graph->scc->size *= 2;
                }

                if (!(graph->scc->components[pos].included_node_ids = malloc (INIT_SCC_SIZE * sizeof (uint32_t)))) {

                    fprintf(stderr, "Error: reallocating strongly connected components\n");
                    return;
                }            

                graph->scc->components[pos].component_id = pos;
                graph->scc->components[pos].included_nodes_count = 1;
                graph->scc->components[pos].size = INIT_SCC_SIZE;
                graph->scc->components[pos].included_node_ids[0] = topID;

                while(topID != last->id) {                                         //pop the nodes from the stack that belong to the same SCC

                    topID = stack_pop(tarStack);
                    top = &nodeArray[topID];
                    onStack[topID] = 0;
                    graph->scc->id_belongs_to_component[topID] = pos;
                    graph->scc->components[pos].included_nodes_count ++;

                    if (graph->scc->components[pos].included_nodes_count > graph->scc->components[pos].size ) {

                        if (!(graph->scc->components[pos].included_node_ids = realloc (graph->scc->components[pos].included_node_ids, 
                                                                                      2 * graph->scc->components[pos].size * sizeof (uint32_t)))) {
                    
                            fprintf(stderr, "Error: reallocating strongly connected components\n");
                            return;
                        }

                        graph->scc->components[pos].size *= 2;
                    }

                    graph->scc->components[pos].included_node_ids[graph->scc->components[pos].included_nodes_count - 1] = topID;
                }
            }

            newLast = last->caller;   													//Equivalent to going up one recursive call in recursive version
            
            if(newLast) {

                if(newLast->lowlink > last->lowlink) {
        
                    newLast->lowlink = last->lowlink;
                }
                
                last = newLast;
            }
            else { 																		//We have seen all the nodes , finish

                break;
            }
        }
    }
}


int findNodeStronglyConnectedComponentID (SCC* scc, uint32_t nodeId) {

    if (nodeId >= scc->size) {
		
        fprintf(stderr, "%u does not belong to any component\n", nodeId);
        return FAILURE;
	}

    return (int) scc->id_belongs_to_component[nodeId];

}


int createHyperGraphSCC (Graph * graph) {
	
    int       retval, *array;
    uint32_t  comp, nodeId, k, neighbors_comp;
    List_node *tmp;
    ptr offset;

    graph->scc->hyperGraph = create_graph (HYPERGRAPH);

    if(!graph->scc->hyperGraph) {

        fprintf(stderr, "Error: creating hypergraph for strongly connected components\n");
        return FAILURE;
    }	

    if (!(array = malloc(graph->scc->components_count * sizeof(int)))) {

        fprintf(stderr, "%s\n", "Error: creating hypergraph for strongly connected components\n");
        return FAILURE;
    }
	
    for (comp = 0 ; comp < graph->scc->components_count ; comp++ ) {                  //array used as temporary "explored set"
                                                                                      //so we dont insert the same edges on the hypergraph
        array[comp] = -1;                                                             //twice , and we check if we already inserted fast
    }

    for (comp = 0 ; comp < graph->scc->components_count ; comp++ ) {                   //for each node in hypergraph

        for(nodeId = 0; nodeId < graph->scc->components[comp].included_nodes_count; nodeId++) {    //for each graph node inside it

            offset = getListHead(graph->index_out, graph->scc->components[comp].included_node_ids[nodeId]);

            if (offset != EMPTY_NODE) {

                while (offset != NOT_EXISTS) {

                    tmp = getListNode (offset , *(graph->buffer_out));

                    for (k = 0; k < NEIGHBORS; k++)	{

                        if(tmp->neighbor[k] == EMPTY_NEIGHBOR) {

                            break;
                        }
                        else {	

                            neighbors_comp = graph->scc->id_belongs_to_component[tmp->neighbor[k]];

                            if (comp != neighbors_comp) {	

                                if(array[neighbors_comp] < (int)comp) {	 //insert all edges needed in hypergraph 

                                    retval = insertEdge (graph->scc->hyperGraph, NULL, comp, neighbors_comp, 0, NO_CHECKING);

                                    if (retval == FAILURE) {

                                        fprintf(stderr, "Error: inserting edge in strongly connected components' hypergraph\n");
                                        return FAILURE;
                                    }

                                    array[neighbors_comp] = comp;
                                }
                            }
                        }	
                    }

                    offset = tmp->nextListNode;
                }
            }
        }
    }

    free(array);

    return OK_SUCCESS;
}


int estimateShortestPathStronglyConnectedComponents (Graph* graph, uint32_t start, uint32_t goal, int* incExplored, int* outExplored, int* counter, Queue * incQueue, Queue * outQueue ) {

    int      i, j, tmp, same_scc, outDepth, incDepth, incNumNeighbors, outNumNeighbors;
    qnode    node_out, node_inc;
    uint32_t neighbor;
	ptr      offset;
	
	
    if (findNodeStronglyConnectedComponentID(graph->scc, start) == findNodeStronglyConnectedComponentID(graph->scc, goal)) {
    
        same_scc = 1;                                            //start and goal are inside the same SCC , so we serach only
    }                                                            //inside this particular SCC to find the path between them
    else {	
		
        same_scc = 0;                                           //start and goal are not in the same SCC, so each time a neighbor is in
    }                                                           // a different SCC from start/goal , we ask grail to exclude the neighbor
                                                                // if grailAnswer is NO.
	if (start == goal) {

        return 0;
	}
    else if ((graph->index_out->numNeighbors[start] == 0) || (graph->index_inc->numNeighbors[goal] == 0)) {																//if S or G have no neighbors, there is no need for BFS

        return -1;
    }
    else if ((start >= graph->index_out->maxNodes) || (goal >= graph->index_out->maxNodes)) {
		
        return -1;
    }
    else if ((graph->index_out->array[start] == NOT_EXISTS) || (graph->index_out->array[goal] == NOT_EXISTS)) {

        return -1;
    }

    if (!incQueue || !outQueue) {

        fprintf(stderr,"Error: Queues are NULL\n");
        return FAILURE;
    }

    node_out = start;
    node_inc = goal;
    queue_push (outQueue, start);																//push start node in this queue
    queue_push (incQueue, goal);																//push goal node in the other queue
    outExplored[start] = *counter;
    incExplored[goal] = *counter;
    outDepth = incDepth = 0;
    incNumNeighbors = graph->index_inc->numNeighbors[goal];
    outNumNeighbors = graph->index_out->numNeighbors[start];

    while (1) {

        if (incNumNeighbors < outNumNeighbors) {                              //We chose chosee which "side" of the BFS to expand
                                                                              //depending on which frontier contains the nodes 
            incDepth ++;                                                      //that have a smaller total number of childern (added)
            tmp = incQueue->num;

            for (i = 0; i < tmp; i++) {
                
                node_inc = queue_pop (incQueue);	
                offset = getListHead (graph->index_inc, node_inc);
				
                while (1) {

                    for (j = 0; j < NEIGHBORS; j++) {

                        neighbor = graph->buffer_inc->array[offset].neighbor[j];
                    
                        if (neighbor != EMPTY_NEIGHBOR) {

                            if (outExplored[neighbor] == *counter) {

                                empty_queue(incQueue);
                                empty_queue(outQueue);

                                return (incDepth + outDepth);                 //return path cost, which is the total depth from each side of the
                            }                                                 // BFS , in the node that the two sides "met"
									
                            if (incExplored[neighbor] < *counter) {

                                incExplored[neighbor] = *counter;

                                if (graph->index_inc->numNeighbors[neighbor]) {

                                    if (!same_scc) {

                                        if(isReachableGrailIndex(graph->scc, start, neighbor) == MAYBE) {

                                            queue_push(incQueue, neighbor);
                                        }
                                    } 
                                    else {
									
                                        if (findNodeStronglyConnectedComponentID(graph->scc, start) == 
                                            findNodeStronglyConnectedComponentID(graph->scc, neighbor)) {
										
                                            queue_push(incQueue, neighbor);
                                        }
                                    }
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
                
                empty_queue(incQueue);
                empty_queue(outQueue);
				
                return -1;
            }

            i = incQueue->first;                                                         //calculating total children of all the nodes in the
            incNumNeighbors = graph->index_inc->numNeighbors[incQueue->array[i]];        //frontier , for this side of the BFS(same for the other)
            
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

                        if (neighbor != EMPTY_NEIGHBOR) {

                            if (incExplored[neighbor] == *counter) {
                                
                                empty_queue(incQueue);
                                empty_queue(outQueue);

                                return (incDepth + outDepth);
                            }
									
                            if (outExplored[neighbor] < *counter) {

                                outExplored[neighbor] = *counter;

                                if (graph->index_out->numNeighbors[neighbor]) {

                                    if (!same_scc) {

                                        if (isReachableGrailIndex (graph->scc, neighbor, goal) == MAYBE) {

                                            queue_push(outQueue, neighbor);
                                        }
                                    } 
                                    else {

                                        if (findNodeStronglyConnectedComponentID(graph->scc, neighbor) == 
                                            findNodeStronglyConnectedComponentID(graph->scc, goal)) {
										
                                            queue_push(outQueue, neighbor);
                                        }
                                    }
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

            if(!outQueue->num) {

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

void iterateStronglyConnectedComponentID (SCC* scc, ComponentCursor* cursor) {

    cursor->iter_counter = 0;
    cursor->component_ptr = &(scc->components[0]);

}    


int next_StronglyConnectedComponentID (SCC* scc, ComponentCursor* cursor) {
	
    cursor->iter_counter++;
    if (cursor->iter_counter == scc->components_count) {

        return FAILURE;
    
    }
    else {
		
        cursor->component_ptr = &(scc->components[cursor->iter_counter]);
       
        return OK_SUCCESS;
	}
}

int destroyStronglyConnectedComponents (SCC* scc) {

    int i;
	
    if(!scc->components) {
		
		free(scc);
		
        return OK_SUCCESS;
    }

    for (i = 0; i < scc->components_count; i++) {

        free (scc->components[i].included_node_ids);
    }

    if (scc->hyperGraph) {

        destroy_graph(scc->hyperGraph);
    }

    if (scc->grailIndex) {

        destroyGrailIndex(scc->grailIndex);
	}

    free (scc->components);
    free (scc->id_belongs_to_component);
    free (scc->cursor);
    free (scc);

    return OK_SUCCESS;

}


int buildGrailIndex (SCC* scc) {

    int       i, j, k, n, flag, rank;
    uint32_t  id, childID, *shuffle_components;
    ptr       offset;
    List_node *tmp;
    Stack     *stack;

    rank = 0;

    for (j=0; j < NUM_OF_GRAIL_INDEXES ; j++) {

        if (!(scc->grailIndex[j].min_rank = malloc(scc->components_count * sizeof(int)))) {

            fprintf(stderr, "Error: allocating GrailIndex\n");
            
            return FAILURE;
        }
    
        if (!(scc->grailIndex[j].rank = malloc(scc->components_count * sizeof(int)))) {

            fprintf(stderr, "Error: allocating GrailIndex\n");
    
            return FAILURE;
        }

        for (i = 0; i < scc->components_count; i++) {

            scc->grailIndex[j].min_rank[i] = 0;
            scc->grailIndex[j].rank[i] = 0;
        }
    }

    if (!(shuffle_components = malloc(scc->components_count * sizeof(uint32_t)))) {

        fprintf(stderr, "Error: allocating grail array\n");
        return FAILURE;
    }

    for (i = 0 ; i<scc->components_count ; i++) {                            //shuffle components is used to start each traversal from
                                                                             //a random node , so we create multiple labels
        shuffle_components[i] = i;                                           //we initialize it as the nodeIDS of the components are
    }                                                                        //(0-components_count), and before each traversal
	                                                                         //we shuffle it
    for (j = 0 ; j < NUM_OF_GRAIL_INDEXES ; j++) {

        stack = create_stack ();
        shuffleArray (shuffle_components, (int)(scc->components_count));

        for (i = 0; i < scc->components_count; i++)	{						  //for each component 
   	
            if (!scc->grailIndex[j].rank[shuffle_components[i]]) {			  //if "this" components does not have a rank yet
    
                stack_push (stack, shuffle_components[i]);

                while (stack->last > -1) {

                    if (scc->hyperGraph->index_out->numNeighbors[stack->array[stack->last]]) {  //if the node has at least one neighbor
    
                        flag = 0;		
                        offset = getListHead (scc->hyperGraph->index_out, stack->array[stack->last]);
    
                        while (offset != NOT_EXISTS) {						   //push in stack the neighbors that do not have a rank yet 
							
                            tmp = getListNode (offset , *(scc->hyperGraph->buffer_out));
    
                            for (k = 0; k < NEIGHBORS; k++) {
    
                                if(tmp->neighbor[k] == EMPTY_NEIGHBOR) {
    
                                    break;
                                }
                                else {

                                    if (!scc->grailIndex[j].rank[tmp->neighbor[k]]) {

                                        stack_push(stack, tmp->neighbor[k]);
                                        flag = 1;
								    }
                                }
                            }
                        
                            offset = tmp->nextListNode;	

                        }
                        if (!flag) {								//flag indicating all the neighbors of "this" node got already ranks
						
                            id = stack_pop (stack);
                        
                            if (!scc->grailIndex[j].rank[id]) {

                                rank ++;
                                scc->grailIndex[j].rank[id] = rank;
                                scc->grailIndex[j].min_rank[id] = rank;
                                offset = getListHead (scc->hyperGraph->index_out, id);
							
                                while (offset != NOT_EXISTS) {

                                    tmp = getListNode (offset , *(scc->hyperGraph->buffer_out));

                                    for (k = 0; k < NEIGHBORS; k++) {

                                        if (tmp->neighbor[k] == EMPTY_NEIGHBOR) {

                                            break;
                                        }
                                        else {

                                            if (scc->grailIndex[j].min_rank[tmp->neighbor[k]] < scc->grailIndex[j].min_rank[id]) {

                                                scc->grailIndex[j].min_rank[id] = scc->grailIndex[j].min_rank[tmp->neighbor[k]];
                                            }
                                        }
                                    }

                                    offset = tmp->nextListNode;	
                                }
                            }
                        }
                    }
                    else {                                                 //if the node does not have neighbors
						
                        id = stack_pop (stack);

                        if (!scc->grailIndex[j].rank[id]) {

                            rank ++;
                            scc->grailIndex[j].rank[id] = rank;
                            scc->grailIndex[j].min_rank[id] = rank;
                        }
                    }
                }
            }
        }    
        destroy_stack(stack);
    }

    free(shuffle_components);		
	
    return OK_SUCCESS;

}


grailAnswer isReachableGrailIndex (SCC* scc, uint32_t fromID, uint32_t toID) {
	
    int         i;
    uint32_t    fromID_comp,toID_comp;
	grailAnswer answer;

    answer = NO;

    fromID_comp = scc->id_belongs_to_component[fromID];             //get the hypergraph nodes in which the given nodes belong to
    toID_comp = scc->id_belongs_to_component[toID];

    for(i=0 ; i<NUM_OF_GRAIL_INDEXES; i++) {

        if ((scc->grailIndex[i].min_rank[toID_comp] < scc->grailIndex[i].min_rank[fromID_comp]) || 
            (scc->grailIndex[i].rank[toID_comp] > scc->grailIndex[i].rank[fromID_comp])) {
    
                return answer;                                     //if any off the grail indexes answers NO , return NO
        }	
    }

    answer = MAYBE;	                                               //if none of the grail indexes answers NO , return MAYBE
    
    return answer;
}


int destroyGrailIndex (GrailIndex* index) {

    int i;

    for (i=0 ; i < NUM_OF_GRAIL_INDEXES ; i++) {

        free (index[i].rank);
        free (index[i].min_rank);
	}

    return OK_SUCCESS;

}


void shuffleArray (uint32_t* array, int last) {                     //shuffle the given array

    int      i, randPos;
    uint32_t tmp;
                                                                    //start with last being the last element of the array
    while(last > 2) {                                               //iterate through the elements of the array
		                                                            //and in each iteration swap a random element with the "last"
        last--;                                                     //then last becomes last-1 etc
        randPos = rand() % last;
        tmp = array[last];
        array[last] = array[randPos];
        array[randPos] = tmp;
    }

}