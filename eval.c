#include "defs.h"
#include <stdlib.h>

#define FLOW(flow)  (flow != BRK_FLOW && flow != CONT_FLOW)
#define BINARY(head)   ((head !=NULL) && (check_binary(head->ast_type)))
#define LOGIC(head)    ((head->ast_type == AND) || (head->ast_type == OR))
#define UNARY(head)  ((head != NULL) && check_unary(head->ast_type))

#define BINARY_OR_UNARY(head) (UNARY(head) || BINARY(head) || LOGIC(head))

extern FLOW flow;



void print_value(AST_TYPE type,void *value){ 
    AST_NODE *node;
    switch(type){
        case NO:
            fprintf(stdout,"%Lf\n",*((long double*)value));
            break;
        case STR:
            node = (AST_NODE*) value;
            fprintf(stdout,"%s\n",node->Str.string);
            break;
        default:
            break;
    }
}





long double eval_ast(AST_NODE *root){
    
    AST_NODE *head = root;
    AST_TYPE type;
    VALUE *value;
    long double temp_value;
    long double *temp_ptr;
    VALUE *l_val,*r_val;
    long double left,right;
    AST_NODE *left_node,*right_node;
    left_node  = NULL;
    right_node = NULL;

    
    if(BINARY(head)){
        left_node = head->Binary.left;
        right_node = head->Binary.right;
    }


    if(left_node != NULL && FLOW(flow)) left = eval_ast(head->Binary.left);
    if(right_node != NULL && FLOW(flow)) right = eval_ast(head->Binary.right);

    type = head->ast_type;
    switch(type){
        case NO:
        case INT:
        case FLT:
            #define val_r(h) h->Number.ref
            #define val(h) h->Number.num

            if(head->Number.ref != NULL){
                return *(head->Number.ref);
            }else{
                return head->Number.num;
            }
        case TRUE:
            return (Boolean) head->Bool.value;
        case FALSE:
            return (Boolean)head->Bool.value;
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
            return -(eval_ast(head->Unary.factor));
        case U_PLUS:
            return eval_ast(head->Unary.factor);
        case NOT:
            return !(eval_ast(head->Unary.factor));
        case AND:
            left = eval_ast(head->Binary.left);
            if(left) 
                return eval_ast(head->Binary.right);
            else 
                return left;
        case OR:  
            left = eval_ast(head->Binary.left);
            if(left)
                return left;
            else 
                return eval_ast(head->Binary.right);
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
            #define value(head) head->Assign.value
            if(value(head)!= NULL){
            temp_value = eval_ast(value(head));
            sym_entry(head->sym_data->sym_table,head->Assign.id_name,temp_value);
            }          
            return temp_value;
        case PRNT:
            #define print_node(ptr) ptr->Print.expr

            if(BINARY_OR_UNARY(print_node(head))){
                temp_ptr = malloc(sizeof(long double));
                *temp_ptr = eval_ast(print_node(head));
                print_value(NO,temp_ptr);
                return temp_value;
            }else if(print_node(head)->ast_type == STR){
                #define str_node(head) print_node(head)->Str 
                print_value(STR,print_node(head));
            }
            #undef print_node
            return 1;
        case IF:
            #define condition_node head->If.exp
            #define true_node     head->If.left
            #define false_node    head->If.right
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
            #define init_stmt head->For.init
            #define cond_stmt head->For.cond
            #define expr_stmt head->For.exp
            #define statement head->For.stmts
            
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
            #define condition_node head->Binary.left
            #define value_node     head->Binary.right

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
