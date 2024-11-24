#include <stdio.h>
#include "defs.h"
#include <stdlib.h>

AST_NODE *mk_binary_node(AST_TYPE type,AST_NODE *left,AST_NODE *right){

    AST_NODE *new_node = malloc(sizeof(AST_NODE));
    new_node->ast_type = type;
    new_node->node.Binary.left = left;
    new_node->node.Binary.right = right;
    return new_node;
}

AST_NODE *mk_num_node(AST_TYPE type,int num){

    AST_NODE *new_node = malloc(sizeof(AST_NODE));
    new_node->ast_type = NO;
    new_node->node.Unary.num = num;
    return new_node;
}

int eval_ast(AST_NODE *root){
    
    AST_NODE *head = root;
    int type;
    long double left,right;
    
    if(head != NULL && head->ast_type == NO) {
        return head->node.Unary.num;
    }

    left = eval_ast(head->node.Binary.left);
    right = eval_ast(head->node.Binary.right);

    type = head->ast_type;
    switch(type){
        case ADD:
            return (left+right);
        case SUB:
            return (left-right);
        case MUL:
            return (left*right);
        case DIV:
            return (left/right);
        default:
            return 0;
    }
}
