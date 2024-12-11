#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define FLOW(flow)  (flow != BRK_FLOW && flow != CONT_FLOW)
#define BINARY(head)   ((head !=NULL) && (check_binary(head->ast_type)))
#define LOGIC(head)    ((head->ast_type == AND) || (head->ast_type == OR))
#define UNARY(head)  ((head != NULL) && check_unary(head->ast_type))

#define BINARY_OR_UNARY(head) (UNARY(head) || BINARY(head) || LOGIC(head))

FLOW flow = NONE;


int check_unary(AST_TYPE type){
        switch(type){
            case NO:
            case INT:
            case FLT:
            case NOT:
            case U_PLUS:
            case U_MIN:
                return 1;
            default:
                return 0;
        }
}

int check_binary(AST_TYPE type){
        switch(type){
            case ADD:
            case MUL:
            case DIV:
            case SUB:
            case STMT:
            case DEQ:
            case GT:
            case LT:
            case MOD:
            //case AND:
            //case OR:
            case GEQ:
            case LEQ:
            case NEQ:
                return 1;
            default:
                return 0;
        }
}


AST_NODE *mk_binary_node(AST_TYPE type,AST_NODE *left,AST_NODE *right){

    AST_NODE *new_node = malloc(sizeof(AST_NODE));
    new_node->ast_type = type;
    new_node->node.Binary.left = left;
    new_node->node.Binary.right = right;
    return new_node;
}

AST_NODE *mk_num_node(AST_TYPE type,long double num,void *ref){

    AST_NODE *new_node = malloc(sizeof(AST_NODE));
    new_node->ast_type = type;
    new_node->node.Number.num = num;
    new_node->node.Number.ref = ref;
    return new_node;
}

AST_NODE *mk_bool_node(AST_TYPE type,Boolean value){
    
    AST_NODE *new_node = malloc(sizeof(AST_NODE));
    new_node->ast_type = type;
    new_node->node.Bool.value = value;
    return new_node;
}

AST_NODE *mk_unary_node(AST_TYPE type,AST_NODE *factor){
    AST_NODE *new_node = malloc(sizeof(AST_NODE));

    new_node->ast_type = type;
    new_node->node.Unary.factor = factor;
    return new_node;
}

AST_NODE *mk_assign_node(AST_TYPE type,char *id,AST_NODE *expr,S_TABLE *sym_tab){
    AST_NODE *new_node = malloc(sizeof(AST_NODE));
    new_node->sym_data = malloc(sizeof(SYM_DATA));
    new_node->ast_type = type;
    new_node->node.Assign.id_name = id;
    new_node->node.Assign.value = expr;

    if(sym_tab != NULL) 
        new_node->sym_data->sym_table = sym_tab;
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



AST_NODE *mk_if_node(AST_TYPE type,AST_NODE *exp,AST_NODE *if_true,AST_NODE *if_false){

    AST_NODE *new_node = malloc(sizeof(AST_NODE));
    new_node->ast_type = type;
    new_node->node.If.exp = exp;
    new_node->node.If.left = if_true;
    new_node->node.If.right = if_false;
    return new_node;
}

AST_NODE *mk_for_node(AST_TYPE type,AST_NODE *init,AST_NODE *cond,AST_NODE *exp,AST_NODE *stmts){

    AST_NODE *new_node = malloc(sizeof(AST_NODE));
    new_node->ast_type = type;
    new_node->node.For.init = init;
    new_node->node.For.cond = cond;
    new_node->node.For.exp = exp;
    new_node->node.For.stmts = stmts;
    return new_node;
}

AST_NODE *mk_flow_node(AST_TYPE type){
    AST_NODE *new_node = malloc(sizeof(AST_NODE));
    new_node->ast_type = type;
    return new_node; 
}



