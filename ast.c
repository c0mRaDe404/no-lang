#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define FLOW(flow)  (flow != BRK_FLOW && flow != CONT_FLOW)
#define BINARY(head)   ((head !=NULL) && (check_binary(head->ast_type)))

#define UNARY(head)  ((head != NULL) && check_unary(head->ast_type))

#define BINARY_OR_UNARY(head) BINARY(head) || UNARY(head)

FLOW flow = NONE;


int check_unary(AST_TYPE type){
        switch(type){
            case NO:
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
            case AND:
            case OR:
            case GEQ:
            case LEQ:
            case NEQ:
                return 1;
            default:
                return 0;
        }
}

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

AST_NODE *mk_num_node(AST_TYPE type,long double num,void *ref){

    AST_NODE *new_node = malloc(sizeof(AST_NODE));
    new_node->ast_type = NO;
    new_node->node.Unary.num = num;
    new_node->node.Unary.ref = ref;
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
    new_node->node.Signed.factor = factor;
    return new_node;
}

AST_NODE *mk_assign_node(AST_TYPE type,char *id,AST_NODE *expr,S_TABLE *sym_tab){
    AST_NODE *new_node = malloc(sizeof(AST_NODE));
    new_node->ast_type = type;
    new_node->node.Assign.id_name = id;
    new_node->node.Assign.value = expr;
    new_node->node.Assign.sym_tab = sym_tab;
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

long double eval_ast(AST_NODE *root){
    
    AST_NODE *head = root;
    AST_TYPE type;
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


    if(left_node != NULL && FLOW(flow)) left = eval_ast(head->node.Binary.left);
    if(right_node != NULL && FLOW(flow)) right = eval_ast(head->node.Binary.right);

    type = head->ast_type;
    switch(type){
        case NO:
            if(head->node.Unary.ref != NULL){
                return *(head->node.Unary.ref);
            }else return head->node.Unary.num;
        case TRUE:
            return (Boolean) head->node.Bool.value;
        case FALSE:
            return (Boolean)head->node.Bool.value;
        case ADD:
            return (left+right);
        case SUB:
            return (left-right);
        case MUL:
            return (left*right);
        case DIV:
            return (left/right);
        case MOD:
            return ((int)left % (int)right);
        case U_MIN:
            return -(eval_ast(head->node.Signed.factor));
        case U_PLUS:
            return eval_ast(head->node.Signed.factor);
        case NOT:
            return !(eval_ast(head->node.Signed.factor));
        case AND:
            return (left && right);
        case OR:
            return (left || right);
        case DEQ:
            return (left == right);    
        case GT:
            return (left > right);
        case LT:
            return (left < right);
        case LEQ:
            return (left <= right);
        case GEQ:
            return (left >= right);
        case NEQ:
            return (left != right);
        case ASSIGN:
            temp_value = eval_ast(head->node.Assign.value);
            sym_entry(head->node.Assign.sym_tab,head->node.Assign.id_name,temp_value);          
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
                print_value(STR,print_node(head));
            }
            #undef print_node
            return 1;
        case IF:
            #define condition_node head->node.If.exp
            #define true_node     head->node.If.left
            #define false_node    head->node.If.right
            if(eval_ast(condition_node)){ 
                if(true_node != NULL) 
                    return eval_ast(true_node);
            }else {
                if(false_node != NULL) 
                    return eval_ast(false_node);
            }
            #undef condition_node
            #undef true_node
            #undef false_node
            return 1;
        case FOR:
            #define init_stmt head->node.For.init
            #define cond_stmt head->node.For.cond
            #define expr_stmt head->node.For.exp
            #define statement head->node.For.stmts
            
            for(eval_ast(init_stmt);eval_ast(cond_stmt);eval_ast(expr_stmt)){
                if(statement != NULL && FLOW(flow)) {
                    eval_ast(statement);
                }else if(flow == BRK_FLOW){
                    flow = NONE;
                    break;
                }else if(flow == CONT_FLOW){
                    flow = NONE;
                    continue;
                }else{
                    return 0;
                }
            }

            #undef init_stmt 
            #undef cond_stmt 
            #undef expr_stmt 
            #undef statement     
        case WHILE:
            #define condition_node head->node.Binary.left
            #define value_node     head->node.Binary.right

            while(eval_ast(condition_node)){
                if(value_node != NULL && FLOW(flow)) {
                    eval_ast(value_node);
                }else if(flow == BRK_FLOW){
                    flow = NONE; 
                    break;
                }else if(flow == CONT_FLOW) {
                    flow = NONE;
                    continue;
                }else{
                    return 0;
                }
            }
            #undef condition_node
            #undef value_node 
            return 0;
    
        case BRK:
            flow = BRK_FLOW;
            return 0;
        case CONT:
            flow = CONT_FLOW;
            return 0;
        default:
            return 0;
    }
}
