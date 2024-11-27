#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



#define BINARY(head)  head->ast_type == ADD  || head->ast_type == MUL \
                || head->ast_type == DIV || head ->ast_type==SUB \
                || head->ast_type == STMT

#define UNARY(head)  head != NULL && head->ast_type == NO

#define BINARY_OR_UNARY(head) BINARY(head) || UNARY(head)



void print_value(AST_TYPE type,void *value){ 
    AST_NODE *node;
    switch(type){
        case NO:
            fprintf(stdout,"%Lf\n",*((long double*)value));
            break;
        case STR:
            node = (AST_NODE*) value;
            fprintf(stdout,"%s\n",node->node.Str.string);
            break;
        default:
            break;
    }
}


AST_NODE *mk_binary_node(AST_TYPE type,AST_NODE *left,AST_NODE *right){

    AST_NODE *new_node = malloc(sizeof(AST_NODE));
    new_node->ast_type = type;
    new_node->node.Binary.left = left;
    new_node->node.Binary.right = right;
    return new_node;
}

AST_NODE *mk_num_node(AST_TYPE type,long double num){

    AST_NODE *new_node = malloc(sizeof(AST_NODE));
    new_node->ast_type = NO;
    new_node->node.Unary.num = num;
    return new_node;
}

AST_NODE *mk_unary_node(AST_TYPE type,AST_NODE *factor){
    AST_NODE *new_node = malloc(sizeof(AST_NODE));

    new_node->ast_type = type;
    new_node->node.Signed.factor = factor;
    return new_node;
}

AST_NODE *mk_assign_node(AST_TYPE type,char *id,AST_NODE *expr){
    AST_NODE *new_node = malloc(sizeof(AST_NODE));
    new_node->ast_type = type;
    new_node->node.Assign.id_name = id;
    new_node->node.Assign.value = expr;
    return new_node;
}


AST_NODE *mk_print_node(AST_TYPE type,AST_NODE *expr){
    
    AST_NODE *new_node = malloc(sizeof(AST_NODE));
    new_node->ast_type = type;
    new_node->node.Print.expr = expr;
    return new_node;
}

AST_NODE *mk_string_node(AST_TYPE type,char *string,size_t length){
    AST_NODE *new_node = malloc(sizeof(AST_NODE));
    new_node->ast_type = STR;
    new_node->node.Str.string = string;
    new_node->node.Str.len = length;
    return new_node;
}

long double eval_ast(AST_NODE *root){
    
    AST_NODE *head = root;
    int type;
    char *temp_ch;
    long double temp_value;
    long double *temp_ptr;
    long double left,right;
    AST_NODE *left_node,*right_node;
    left_node  = NULL;
    right_node = NULL;

    if(BINARY(head)){
        left_node = head->node.Binary.left;
        right_node = head->node.Binary.right;
    }

    if(UNARY(head)) {
        //if i add the pointer support instead long this doesn't needed anymore;
        if(head->node.Unary.ref != NULL) head->node.Unary.num = *(head->node.Unary.ref);
        return head->node.Unary.num;
    }

    if(left_node != NULL) left = eval_ast(head->node.Binary.left);
    if(right_node != NULL) right = eval_ast(head->node.Binary.right);

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
        case U_MIN:
            return -(eval_ast(head->node.Signed.factor));
        case U_PLUS:
            return eval_ast(head->node.Signed.factor);
        case ASSIGN:
            temp_value = eval_ast(head->node.Assign.value);
            sym_entry(sym_tab,head->node.Assign.id_name,temp_value);          
            return temp_value;
        case PRNT:
            #define print_node(ptr) ptr->node.Print.expr

            if(BINARY_OR_UNARY(print_node(head))){
                temp_ptr = malloc(sizeof(long double));
                *temp_ptr = eval_ast(print_node(head));
                print_value(NO,temp_ptr);
                return temp_value;
            }else if(print_node(head)->ast_type == STR){
                #define str_node(head) print_node(head)->node.Str
                str_node(head).len = str_node(head).len-2;
                temp_ch = malloc(sizeof(char));
                memmove(temp_ch,(++str_node(head).string),str_node(head).len);
                str_node(head).string = temp_ch;
                print_value(STR,print_node(head));
            }    
            
        default:
            return 0;
    }
}
