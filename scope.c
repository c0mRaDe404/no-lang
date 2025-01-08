#include "defs.h"
#include <stdlib.h>
#define STACK_SIZE (100)

#define TOP(s) (s->top)


void *mk_scope_stack(size_t stack_size){
    
    SCOPE_STACK *new_stack = malloc(sizeof(SCOPE_STACK));
    new_stack->stack = malloc(stack_size*sizeof(SYM_TABLE*));
    new_stack->top = 0;
    new_stack->size = stack_size;
    return new_stack; 
}


size_t scope_level(SCOPE_STACK *stack){
    return stack->top;
}

SYM_TABLE *get_cur_scope(SCOPE_STACK *stack){
    return stack->stack[stack->top-1];
}

SYM_TABLE *scope_enter(SCOPE_STACK *stack,SYM_TABLE *new_scope_table){
    if(TOP(stack) <= stack->size){
        if(TOP(stack) == 0){
            stack->stack[TOP(stack)++] = new_scope_table;
            return new_scope_table;
        }else{
            SYM_TABLE *cur_scope_table = get_cur_scope(stack);
            stack->stack[TOP(stack)++] = new_scope_table;
            new_scope_table->parent = cur_scope_table;
            cur_scope_table->child = new_scope_table;
            return new_scope_table;
        }
    }else {
        fprintf(stderr,"STACK LIMIT EXCEEDED !!!\n");
        exit(1000);
    }
}

SYM_TABLE *scope_exit(SCOPE_STACK *stack){
    if(stack->top >=0)
        return stack->stack[--TOP(stack)];
    else
        return NULL;
}
