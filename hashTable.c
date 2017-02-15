#include "hashTable.h"


hashTable* createHashTable (void) {
    
    hashTable  *hashtable;
    
    if (!(hashtable = malloc (sizeof(hashTable)))) {

	fprintf(stderr, "Error: creating hash table\n");
        return NULL;
    }

    hashtable->global_depth = 0;

    if (!(hashtable->index = malloc (sizeof(bucket*)))) {

        fprintf(stderr, "Error: creating hash table\n");
        return NULL;
    }

    if (!(hashtable->index[0] = malloc (sizeof(bucket)))) {

        fprintf(stderr, "Error: creating hash table\n");
        return NULL;
    }

    hashtable->index[0]->num = 0;
    hashtable->index[0]->local_depth = 0;

    return hashtable;

}


void destroyHashTable (hashTable* hashtable) {

    int i, j;
    
    for (i = 0; i < (int)pow (2, hashtable->global_depth); i++) {                                   //iteration on hashtable array

        if(hashtable->index[i]) {                                                                   // array's size == 2^global_depth

            for (j = i+1; j < (int)pow (2, hashtable->global_depth); j++) {

                if(hashtable->index[j] == hashtable->index[i]) {
        
                    hashtable->index[j] = NULL;
                }
            }

            free (hashtable->index[i]);
            hashtable->index[i] = NULL;
        }	
    }

    free (hashtable->index);
    free (hashtable);

}


int insertHashTable (hashTable* hashtable, uint32_t id) {

    int       i, flag, new_size, old_size, hash;
    bucket    *tmp_bucket, *new_bucket; 

    hash = id % (int)pow (2,hashtable->global_depth);

    for (i = 0; i < hashtable->index[hash]->num; i++) {

        if (hashtable->index[hash]->array[i] == id) {

            return FAILURE;
        }
    }

    if (hashtable->index[hash]->num < NUM_NEIGHBORS) {

		hashtable->index[hash]->array[hashtable->index[hash]->num++] = id;				        //bucket not full , insert node ID at bucket
    }
    else {
	
        if (hashtable->index[hash]->local_depth == hashtable->global_depth) {    				//splitting a bucket with local depth == global depth 
                                                                                                // so we need to double(realloc) the hashtable array
            old_size = pow (2, hashtable->global_depth);

            if (!(hashtable->index = realloc (hashtable->index, 2 * old_size * sizeof (bucket*)))) {

                fprintf(stderr, "Error: reallocating hash table\n");
                return FAILURE;
            }

            hashtable->global_depth ++;
            new_size = pow (2,hashtable->global_depth);

            for (i = old_size; i < new_size ; i++) {

                hashtable->index[i] = hashtable->index[i - old_size];
            }
        }

        tmp_bucket = hashtable->index[hash];  
            
        if (!(new_bucket = malloc (sizeof(bucket)))) {

            fprintf(stderr, "Error: allocating new bucket for hash table\n");
            return FAILURE;
        }

        new_bucket->num = 0;
        new_bucket->local_depth = ++(tmp_bucket->local_depth);

        flag = 0;

        for (i = 0; i < pow (2,hashtable->global_depth); i++) {                                       //fixing new pointer to buckets

            if (hashtable->index[i] == tmp_bucket) {

                if (!flag) {

                    flag = 1;
                }
                else {
        
                    hashtable->index[i] = new_bucket;
                    flag = 0;
                }
            }
        }

        uint32_t tmp_array[NUM_NEIGHBORS];

	    memcpy (tmp_array, tmp_bucket->array, NUM_NEIGHBORS * sizeof(uint32_t));                     //copy "old" bucket's contents

        tmp_bucket->num = 0;
        hash = id % (int) pow(2,hashtable->global_depth);
        hashtable->index[hash]->array[0] = id;
        hashtable->index[hash]->num ++;

        for (i = 0; i < NUM_NEIGHBORS; i++) {                                                        //re-inserting old bucket's contents
                                                                                                     //after split was made
            insertHashTable (hashtable, tmp_array[i]);
        }
    }

    return OK_SUCCESS;

}


HT_info* createHT_info (void) {

    int      i;
    HT_info  *ht_info;

    if (!(ht_info = malloc(sizeof(HT_info)))) {

        fprintf(stderr, "Error: allocating HT_info\n");
        return NULL;
    }

    ht_info->size = HT_INIT_SIZE;
    
    if (!(ht_info->hash_array = malloc(ht_info->size * sizeof(hashTable *)))) {

        fprintf(stderr, "Error: initializing ht_info\n" );
        return NULL;
    }
    
    for (i = 0; i < ht_info->size; i++)
        ht_info->hash_array[i] = NULL;

    return ht_info;

}


void destroyHT_info(HT_info* ht_info) {

    int i;

    for (i = 0; i < ht_info->size; i++) {

        if (ht_info->hash_array[i]) { 

            destroyHashTable(ht_info->hash_array[i]);
        }
    }

    free(ht_info->hash_array);
    free(ht_info);

}
