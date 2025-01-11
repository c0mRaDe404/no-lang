#include <stdlib.h>
#include <setjmp.h>
#include "defs.h"

#define FLOW(flow)  (flow != BRK_FLOW && flow != CONT_FLOW)
#define BINARY(ast_node)   ((ast_node !=NULL) && (check_binary(ast_node->ast_type)))
#define LOGIC(ast_node)    ((ast_node->ast_type == AND) || (ast_node->ast_type == OR))
#define UNARY(ast_node)  ((ast_node != NULL) && check_unary(ast_node->ast_type))

#define BINARY_OR_UNARY(ast_node) (UNARY(ast_node) || BINARY(ast_node) || LOGIC(ast_node))

extern FLOW flow;
int is_return = 0;


typedef struct jmp_context{
    jmp_buf ret_state;
}jmp_context;

typedef struct jmp_stack{
    jmp_context jmp_c[1000];
    size_t top;
}jmp_stack;


jmp_stack j_stack = {0};


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
    
    AST_NODE *ast_node = root;
    AST_TYPE type;
    SYM_DATA *table;
    VALUE *value;
    long double temp_value;
    long double *temp_ptr;
    VALUE *l_val,*r_val;
    long double left,right;
    AST_NODE *left_node,*right_node;
    left_node  = NULL;
    right_node = NULL;

    
    if(BINARY(ast_node)){
        left_node = ast_node->Binary.left;
        right_node = ast_node->Binary.right;
    }


    if(left_node != NULL && FLOW(flow)) left = eval_ast(ast_node->Binary.left);
    if(right_node != NULL && FLOW(flow)) right = eval_ast(ast_node->Binary.right);

    type = ast_node->ast_type;
    switch(type){
        case NO:
        case INT:
        case FLT:
            #define val_r(h) h->Number.ref
            #define val(h) h->Number.num

            if(ast_node->Number.ref != NULL){
                temp_value =  *(ast_node->Number.ref);
            }else{
                temp_value = ast_node->Number.num;
            }
            return temp_value;
        case TRUE:
            return (Boolean) ast_node->Bool.value;
        case FALSE:
            return (Boolean)ast_node->Bool.value;
        case ADD:
            temp_value = (left+right);
            return temp_value;
        case SUB:
            temp_value = (left-right);
            return temp_value;
        case MUL:
            temp_value = (left+right);
            return temp_value;
        case DIV:
            return (left/right);
        case MOD:
            return ((int)left % (int)right);
        case U_MIN:
            return -(eval_ast(ast_node->Unary.factor));
        case U_PLUS:
            return eval_ast(ast_node->Unary.factor);
        case NOT:
            return !(eval_ast(ast_node->Unary.factor));
        case AND:
            left = eval_ast(ast_node->Binary.left);
            if(left) 
                return eval_ast(ast_node->Binary.right);
            else 
                return left;
        case OR:  
            left = eval_ast(ast_node->Binary.left);
            if(left)
                return left;
            else 
                return eval_ast(ast_node->Binary.right);
        case DEQ:
            temp_value =  (left == right);
            return temp_value;
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
            #define value(ast_node) ast_node->Assign.value
            if(value(ast_node)!= NULL){
                temp_value = eval_ast(value(ast_node));
                sym_update(get_cur_scope(s_ptr),ast_node->Assign.id_name,temp_value);
            }          
            return temp_value;
        case PRNT:
            #define print_node(ptr) ptr->Print.expr

            if(BINARY_OR_UNARY(print_node(ast_node))){
                temp_ptr = malloc(sizeof(long double));
                *temp_ptr = eval_ast(print_node(ast_node));
                print_value(NO,temp_ptr);
                return temp_value;
            }else if(print_node(ast_node)->ast_type == STR){
                #define str_node(ast_node) print_node(ast_node)->Str 
                print_value(STR,print_node(ast_node));
            }
            #undef print_node
            return 1;
        case IF:
            #define condition_node ast_node->If.exp
            #define true_node     ast_node->If.left
            #define false_node    ast_node->If.right
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
            #define init_stmt ast_node->For.init
            #define cond_stmt ast_node->For.cond
            #define expr_stmt ast_node->For.exp
            #define statement ast_node->For.stmts

            if(init_stmt != NULL) eval_ast(init_stmt);
            if(cond_stmt != NULL){
                while(eval_ast(cond_stmt)){
                   if(statement != NULL && FLOW(flow)) {
                       eval_ast(statement);
                       eval_ast(expr_stmt);
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
            }
  
            #undef init_stmt 
            #undef cond_stmt 
            #undef expr_stmt 
            #undef statement
            return 0;
        case WHILE:
            #define condition_node ast_node->Binary.left
            #define value_node     ast_node->Binary.right

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
        case BLOCK:
            scope_enter(s_ptr,ast_node->sym_table);
            eval_ast(ast_node->Block.block);
            scope_exit(s_ptr);
            return 0;
        case ID:
            table = sym_fetch(get_cur_scope(s_ptr),ast_node->Id.name);
            return table->entry->value;
        case FDEF:
            return 0;
        case FCALL:
          #define arg_v(ast_node) (ast_node->Func_call.argv)
          #define arg_c(ast_node) (ast_node->Func_call.argc)
          #define par_v(tab)  (tab->entry->node->Func_def.f_params)
          #define fun_body(tab) (tab->entry->node->Func_def.f_body)
          table = sym_fetch(get_cur_scope(s_ptr),ast_node->Func_call.f_name);
          scope_enter(s_ptr,sym_create());
          for(int i = 0;i<arg_c(ast_node);i++){
              sym_entry(par_v(table)[i],eval_ast(arg_v(ast_node)[i]));
          }            
          temp_value = setjmp(j_stack.jmp_c[j_stack.top++].ret_state);
          if(!is_return)
              temp_value = eval_ast(fun_body(table));
            
          is_return = 0;
          scope_exit(s_ptr);
          return temp_value;
        case RET:
          if(ast_node->Ret.value != NULL){
                temp_value = eval_ast(ast_node->Ret.value);
          }
          is_return = 1;
          longjmp(j_stack.jmp_c[--j_stack.top].ret_state,temp_value);
          return temp_value;
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

