#include    <stdint.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <fcntl.h>
#include    <string.h>
#include    <sys/types.h>
#include    <sys/stat.h>
#include    <time.h>
#include    <math.h>
#include    <pthread.h>

/******************************* flags used in insertEdge ***************************************/

#define    ADDITION                10                 // used in order to connect two components if needed 
                                                      // It is used when an addition is triggered from workload file :)

#define    LINEAR_SEARCH           9                  // check for duplicates in a linear way
#define    HASHTABLES              8                  // use the hashtables
#define    NO_CHECKING             7                  // do not check for duplicates

/************************************************************************************************/


/******************************* flags used in path functions ***********************************/

#define    HYPERGRAPH              6                  // execute path for connected components' hypergraph 
#define    GRAPH                   5                  // execute path for initial graph

/************************************************************************************************/


/******************************* graph type *****************************************************/

#define    STATIC                  4
#define    DYNAMIC                 3

/************************************************************************************************/


/******************************* return values **************************************************/

#define    DUPLICATE               2                  //a duplicate has been found
#define    OK_REALLOC              1                  //indicates that a realloc happened in an insert
#define    OK_SUCCESS              0                  //the success return value in most of our functions
#define    FAILURE                 -1                 //the failure return value in most of our functions

/************************************************************************************************/


#define    NOT_EXISTS              -2                 //fixed value indicating that a specific graph node doesn't exist (index struct)
#define    EMPTY_NODE              -3                 //fixed value indicating that a specific node has no neighbors (index struct)
#define    EMPTY_NEIGHBOR          (UINT32_MAX - 1)   //fixed value indicating an empty neighbor position in our the buffer struct
#define    NO_COMPONENT            (UINT32_MAX - 2)   //fixed value indicating that a node does not belong to a component yet


#define    METRIC_VAL              1                  //Metric value checked to rebuild connected components(rebuilding is not worth timewise)
#define    NUM_OF_GRAIL_INDEXES    5                  //number of grail indexes. Options for fastest programm execution are: 
                                                      //medium dataset -> 1 , large dataset -> 5

typedef int ptr;                                      //ptr is an offset not an actual pointer

