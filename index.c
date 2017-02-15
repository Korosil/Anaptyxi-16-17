#include "index.h"


NodeIndex* createNodeIndex () {
    int        i;
    NodeIndex  *index;		

    if (!(index = malloc (sizeof(NodeIndex)))) {      									//allocating index struct & its attributes
																						//initialization
        fprintf (stderr, "Error: allocating index\n");
        return NULL;
	
    }
    
    if (!(index->array = malloc (INIT_INDEX_SIZE * sizeof(ptr)))) {

        fprintf (stderr, "Error: allocating index\n");
        return NULL;
    }
    
    if ((index->lastOffset = malloc (INIT_INDEX_SIZE * sizeof(ptr))) == NULL) {

        fprintf (stderr, "Error: allocating index\n");
        return NULL;
    }

    if ((index->numNeighbors = malloc (INIT_INDEX_SIZE * sizeof(int))) == NULL) {

        fprintf (stderr, "Error: allocating index\n");
        return NULL;
    }

    index->maxNodes = INIT_INDEX_SIZE;

    for (i = 0; i < INIT_INDEX_SIZE; i++) {

        index->array[i] = NOT_EXISTS;
        index->numNeighbors[i] = 0;
    }

    return index;		
}



int insertNode (NodeIndex* index, Buffer* buffer, HT_info* ht_info, uint32_t from_nodeId, uint32_t to_nodeId, uint32_t version, int insertFlag) {

	int       i, retval, size;
	ptr       offset, old_offset;
	List_node *tmp;
	
    retval = OK_SUCCESS;

    if ((from_nodeId >= (uint32_t)index->maxNodes) || (to_nodeId >= (uint32_t)index->maxNodes))	{    		//checking if nodes can fit 
																											//in the existing indexes
        size = 2 * index->maxNodes;
		
        while ((from_nodeId >= (uint32_t)size) || (to_nodeId >= (uint32_t)size)) {							//if not , calculate the size needed
																											//for them to fit
            size *= 2;
        }

        if (!(index->array = realloc (index->array, size * sizeof(ptr)))) {                                  //do the reallocation of the indexes
            
            fprintf(stderr, "Error: reallocating index\n");

            return FAILURE;
        }

        if (!(index->lastOffset = realloc (index->lastOffset, size * sizeof(ptr)))) {
            
            fprintf(stderr, "Error: reallocating index\n");

            return FAILURE;
        }

        if (!(index->numNeighbors = realloc(index->numNeighbors, size * sizeof(int)))) {
                
            fprintf(stderr, "Error: reallocating index\n");

            return FAILURE;
        }
		
		for (i = index->maxNodes; i < size; i++) {															//initialize the newly allocated part 
                                                                                                            //of the indexes
            index->array[i] = NOT_EXISTS;
            index->numNeighbors[i] = 0;
        }
		
        index->maxNodes = size;
    }
	else if (insertFlag == LINEAR_SEARCH) {																	//check for duplicates linearlly 	                                                                                                    //used in hyperGraph of dynamic mode(update index)
		
        offset = getListHead (index, from_nodeId);
        
        if (offset != EMPTY_NODE) {                                                                           //because its a small graph , and we didnt 
		                                                                                                   //need to create hashtables for it.
            while (offset != NOT_EXISTS) {
			
                tmp = getListNode (offset , *buffer);

                for (i = 0; i < NEIGHBORS; i++) {
					
                    if (tmp->neighbor[i] == to_nodeId) {

                        return DUPLICATE;						
                    
                    }
                }
                offset = tmp->nextListNode;
            }
        }
    }
	else if ((insertFlag == HASHTABLES) || (insertFlag == ADDITION)) {                                    //check for duplicates through hashtables
	                                                                                                     //in both "initial" graph given edges
        if (from_nodeId >= ht_info->size) {                                                                //and also edges from workload additions
	
            if (!(ht_info->hash_array = realloc( ht_info->hash_array, 2* ht_info->size * sizeof(hashTable*)))) {

                fprintf(stderr, "Error: reallocating hashtable\n");
                
                return FAILURE;

            }
			
            for (i = ht_info->size; i < 2* ht_info->size; i++) {

                ht_info->hash_array[i] = NULL;
            }

            ht_info->size *= 2;
        }
        else {

            if (!ht_info->hash_array[from_nodeId]) {

                ht_info->hash_array[from_nodeId] = createHashTable ();
            }

            retval = insertHashTable (ht_info->hash_array[from_nodeId], to_nodeId);
			
			if (retval == FAILURE) {
                
                return DUPLICATE;
			
			}
        }
    }

    offset = getListHead (index, from_nodeId);

    if ((offset == NOT_EXISTS) || (offset == EMPTY_NODE)) {													//a new line has to be allocated 																											 																										//in buffer for this node
		
        offset = allocNewNode (buffer);
        index->array[from_nodeId] = offset;
        index->lastOffset[from_nodeId] = offset;
    }

    tmp = getListNode (index->lastOffset[from_nodeId] , *buffer);
	
	for (i = 0; i < NEIGHBORS; i++)	{																			//search for first empty neighbors' 																										
																											//spot and fill it
        if (tmp->neighbor[i] == EMPTY_NEIGHBOR) {

            tmp->neighbor[i] = to_nodeId;
            tmp->edgeProperty[i] = version;
            break;
        }
    }

    if (i == NEIGHBORS) {																					//a new line has to be allocated in the buffer 

        old_offset = index->lastOffset[from_nodeId];													//old_offset = last line's offset for current node
																											//(so that it is not get lost in case of "realloc")
        offset = allocNewNode (buffer);

        index->lastOffset[from_nodeId] = offset;

        tmp = getListNode (old_offset , *buffer);
        tmp->nextListNode = offset;

        tmp = getListNode (offset , *buffer);
        tmp->neighbor[0] = to_nodeId;
        tmp->edgeProperty[0] = version;
    }

    index->numNeighbors[from_nodeId]++;

    if (index->array[to_nodeId] == NOT_EXISTS) {                                                             //set the neighbor of the node "empty" if it didn't already exist
		
	    index->array[to_nodeId] = EMPTY_NODE;
    }

    return retval;

}


ptr getListHead (NodeIndex* index, uint32_t nodeId)	{														//return value: offset in buffer (for the given node)

    if (index->array[nodeId] == NOT_EXISTS) {

        return NOT_EXISTS;
    
    }

    if (index->array[nodeId] == EMPTY_NODE) {

        return EMPTY_NODE;
    
    }

    return index->array[nodeId];

}


int destroyNodeIndex (NodeIndex* index) {

    free (index->array);
    free (index->lastOffset);
    free (index->numNeighbors);
    free (index);

    return OK_SUCCESS;

}
