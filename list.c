#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"

#define INIT_SIZE 4

#define LIST(ast) (ast->node.List)
#define COUNTER(arr) (arr->node.List.size)
#define CAP(arr) (arr->node.List.capacity)

void *allocate_list(AST_NODE *arr_node){
        
        AST_NODE *new_node = malloc(sizeof(AST_NODE));
        new_node->ast_type = LIST;
        LIST(new_node).elements = malloc(INIT_SIZE*sizeof(AST_NODE*));
        LIST(new_node).size     = 0;
        LIST(new_node).capacity = INIT_SIZE;
        return new_node;
}


void *grow_list(AST_NODE *arr_node){
    
    #define new_size(arr,size) (sizeof(AST_NODE*)* CAP(arr) *size)
    
    size_t n_size = new_size(arr_node,2);
    AST_NODE **new_list = realloc(LIST(arr_node).elements,n_size);
    if(new_list != NULL){
        LIST(arr_node).elements = new_list;
        LIST(arr_node).capacity = new_size(arr_node,2)/sizeof(AST_NODE*);
    }
    return arr_node;
}

void insert_val(AST_NODE *arr_node,AST_NODE *value){
    
    if(COUNTER(arr_node) == CAP(arr_node)){
        arr_node = grow_list(arr_node);
        if(arr_node == NULL){
            fprintf(stdout,"Allocation failed !");
            exit(0);
        }
    }

    LIST(arr_node).elements[COUNTER(arr_node)++] = value;
    return;
}
