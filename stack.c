#include "header.h"
#include "stack.h"


Stack* create_stack () {

    Stack *stack;

    if (!(stack = malloc (sizeof(Stack)))) {

        fprintf(stderr,"Error allocating stack\n");
        return NULL;

    }

    if (!(stack->array = malloc (INIT_STACK_SIZE * sizeof(snode)))) {

        fprintf(stderr,"Error allocating array in stack\n");
        return NULL;

    }

    stack->first = -1;
    stack->last = -1;
    stack->size = INIT_STACK_SIZE;
    stack->num = 0;

    return stack;
	
}


void destroy_stack (Stack* stack) {

    free (stack->array);
    free (stack);

    return ;

}


void stack_push (Stack* stack, snode nodeid) {

    int size, first, next;

    size = stack->size;
    first = stack->first;
    next = stack->last + 1;

    if (next == stack->size) {                                               //if last element of stack was in the last position of the array
                                                                             //the "newly" added element should be added at the start of the array
        next = 0;                                                            //(circle array format)

    }

    if (next == stack->first) {                                              //if stack array is full (last + 1 == first), we need to realloc the
                                                                             //stack's array
        if (!(stack->array = realloc (stack->array, 2 * stack->size * sizeof(snode)))) {

            fprintf(stderr, "Error: reallocating stack array\n");
            return ;

        }

        if (stack->array == NULL) {

            fprintf(stderr,"Error re-allocating stack\n");
            return ;

        }

        if (stack->last != stack->size - 1) {                                //when reallocing the stack's array , we fix the position of the elements
                                                                             //inside the array (cause of circle format)
            memmove (&(stack->array[size + first]), &(stack->array[first]), (size - first) * sizeof(snode));			
            stack->first = size + first;

        }

        stack->size *= 2;

    }

    stack->last++;

    if (stack->last == stack->size) {

        stack->last = 0;

    }

    stack->array[stack->last] = nodeid;

    if (first == -1) {

        stack->first = 0;

    }

    stack->num ++;
    return ;

}


snode stack_pop (Stack* stack) {

    snode retval = stack->array[stack->last];

    if (stack->first == stack->last) {

        stack->first = -1;
        stack->last = -1;

    }
	else {

        stack->last--;
        if (stack->last == -1) {

            stack->last = stack->size - 1;

        }

    }

    stack->num --; 
    return retval;

}


void empty_stack (Stack* stack) {

    stack->first = -1;
    stack->last = -1;
    stack->num = 0;

}
