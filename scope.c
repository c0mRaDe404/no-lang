#include "defs.h"
#include <stdlib.h>
#define STACK_SIZE (100)

#define TOP(s) (s->top)


void *mk_stack(SCOPE_STACK *stack,size_t stack_size){
    
    stack->stack = malloc(stack_size*sizeof(S_TABLE*));
    stack->top = 0;
    stack->size = stack_size;
    return stack; 
}

S_TABLE *push(SCOPE_STACK *stack,S_TABLE *cur_symbol_table){
    if(TOP(stack) <= stack->size){
        stack->stack[TOP(stack)++] = cur_symbol_table;
        return sym_create();
    }else {
        fprintf(stderr,"STACK LIMIT EXCEEDED !!!\n");
        exit(1000);
    }
}

S_TABLE *pop(SCOPE_STACK *stack){
    if(stack->top >=0)
        return stack->stack[--TOP(stack)];
    else
        return NULL;
}
