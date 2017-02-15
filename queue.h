#define INIT_QUEUE_SIZE   50             //queues array initial size

typedef uint32_t qnode;                  //queues element


typedef struct Queue {                   //FIFO queue

    qnode    *array;                     //nodes of "this" queue are saved here in a circle array format (O(1) access)
      int    first;                      //first element in queue array index
      int    last;                       //last element in queue array index
      int    size;                       //queues array current size
      int    num;                        //number of elements in queue

} Queue;


/****************************************** PROTOTYPES ******************************************/

   Queue*    create_queue (void);        //allocate memory for queue struct and initialize its contents
     void    destroy_queue (Queue*);     //deallocate memory used by queue struct
     void    queue_push (Queue*, qnode); //push an ellement after the last in queue (realloc the array if needed)
    qnode    queue_pop (Queue*);         //pop the first element of the queue
     void    empty_queue (Queue*);       //empty the queue from its contents by seting first an last index to -1 (initial values)