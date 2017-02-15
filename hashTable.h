#include "header.h"

#define NUM_NEIGHBORS  120                                   //Initial bucket size (it is quite big because in given datasets some odes have 
                                                             //hundreds of thousands of neighbors and the bigger "this" number the fewer
                                                             //the splits will occur in expendible hashing for these nodes) this results 
                                                             //to augmented memory use but minimizes the time spent to create hashtables.
#define HT_INIT_SIZE   1000                                  //Initial number of hashtables (each one used for quick access to the 
                                                             //neighbors of each node in graph).


typedef struct bucket {

         int    num;                                         //number of elements in "this" bucket
    uint32_t    array[NUM_NEIGHBORS];                        //array containing neighborIDs
         int    local_depth;                                 //local depth for "this" bucket used for spliting bucket

} bucket;

typedef struct hash {

      bucket    **index;                                     //the hashtable array, containing pointers to buckets
         int    global_depth;                                //global depth of the hashtable array (used for reallocating the index array)
                                                             //2 ^ global_depth == (index arrays size)
                                                             //hash function: mod(2 ^ global_depth)

} hashTable;

typedef struct ht_info {                                     //an array of hashtables and its size (one hash table for each node of the graph)

   hashTable    **hash_array;
         int    size;

} HT_info;


/****************************************** PROTOTYPES ******************************************/

hashTable*    createHashTable (void);                        //allocates memory for one hashtable and initializing its contents
      void    destroyHashTable (hashTable*);                 //deallocates memory used for a hashtable
       int    insertHashTable (hashTable*, uint32_t);        //insert a neighborID to the given hash table. Splits bucket or reallocates hashtable
                                                             //if necessary and returns success or failure (in case the node was already contained)
  HT_info*    createHT_info (void);                          //allocates memory for HT_info (hashtables array) and initializes its contents
      void    destroyHT_info (HT_info*);                     //deallocates the used memory of HT_info
