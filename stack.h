#define INIT_STACK_SIZE    50                         //stacks array initial size

typedef uint32_t snode;                               //stacks element


typedef struct stack {                                //LIFO stack

  snode*    array;                                    //nodes of "this" stack are saved here in a circle array format (O(1) access)
     int    first;                                    //first element in stack array index
     int    last;                                     //last element in stack array index
     int    size;                                     //stacks array current size
     int    num;                                      //number of elements in stack

} Stack;


/****************************************** PROTOTYPES ******************************************/

Stack*    create_stack (void);                        //allocate memory for stack struct and initialize its contents
  void    destroy_stack (Stack*);                     //deallocate memory used by stack struct
  void    stack_push (Stack*, snode);                 //push an ellement after the last in stack (realloc the array if needed)
 snode    stack_pop (Stack*);                         //pop the last element of the stack
  void    empty_stack (Stack*);                       //empty the stack from its contents by seting first an last index to -1 (initial values)
