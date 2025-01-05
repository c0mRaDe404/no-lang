#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "defs.h"


#define EPSILON (NULL);
#define LOGICAL_OP(token) (token == AND_OP || token == OR_OP || token == NOT_OP)

#define RELATIONAL_OP(token) (token == DOUBLE_EQ || token == G_THAN || token == L_THAN\
                            || token == L_THAN_EQ || token == G_THAN_EQ || token == NOT_EQ)


#define END_OF_FILE(token) (token == EOS) 
#define END_OF_STMT(token) (token == CLOSE_CURLY)
#define END(token) (END_OF_FILE(token) || END_OF_STMT(token))


jmp_buf panic_recovery_state;

AST_TYPE token_to_ast(TOKEN_TYPE type){
    switch(type){
        case PLUS      : return ADD;
        case MINUS     : return SUB;
        case MULTIPLY  : return MUL;
        case DIVIDE    : return DIV;
        case DOUBLE_EQ : return DEQ;
        case G_THAN    : return GT;
        case G_THAN_EQ : return GEQ;
        case L_THAN    : return LT;
        case L_THAN_EQ : return LEQ;
        case NOT_EQ    : return NEQ;
        case PRINT     : return PRNT;
        case MODULO    : return MOD;
        case EQ        : return ASSIGN;
        case AND_OP    : return AND;
        case OR_OP     : return OR;
        case NOT_OP    : return NOT;
        case TRUE_EXP  : return TRUE;
        case FALSE_EXP : return FALSE;
        case BREAK     : return BRK;
        case CONTINUE  : return CONT;
        case INTEGER   : return INT;
        case FLOAT     : return FLT;
        case STRING    : return STR;
        default        : return 0;
    }
}

void discard_tokens(char *exp_tok);

#define enter_panic_mode  discard_tokens
#define throw_error(cur_token,err_msg,expected_token) \
    fprintf(stderr,"%s : at line %d : Expected \"%s\" : got \"%s\" \n",\
            err_msg,line,expected_token,cur_token) 


int is_sync_point(TOKEN_TYPE token){

    switch(token){
        case LET:case IF_KWD:
        case WHILE:case FOR:
        case PRINT:
            return 1;
        default:
            return 0;
    }
}

/* common syntax errors and error messages */
char *error_msg(TOKEN_TYPE e_tok){
    switch(e_tok){
        case ID: return "identifier";
        case OPEN_CURLY : return "{";
        case CLOSE_CURLY : return "}";
        case L_PAREN : return "(";
        case R_PAREN : return ")";
        case SEMI_COLON: return ";";
        case EQ: return "=";
        default:
            return "expression";
    }

}

void consume_token(){
    prev_token = strndup(yytext,yyleng);
    cur_token = yylex();
}


int match(TOKEN_TYPE expected_token) {
    if(expected_token == cur_token) {
       consume_token();
       return 1;
    }else{
        enter_panic_mode(error_msg(expected_token));
        return 0;
    }
}
    
void discard_tokens(char *err_msg){
    #define PANIC_MODE 10
    if(!END(cur_token)) {
        throw_error(yytext,"Syntax Error",err_msg);
    }
    while(!END(cur_token) && !is_sync_point(cur_token)){
        consume_token();
    }

    longjmp(panic_recovery_state,PANIC_MODE);
    return;
}


/* program  ::=  stmt stmt* */
void *program(){
    
    AST_NODE *n1,*n2;
    
    /* sets the checkpoint for panic mode recovery */
    if(setjmp(panic_recovery_state) != PANIC_MODE){
        n1 = statement();
    }

    while(!END(cur_token)){
       n1 = mk_binary_node(STMT,n1,statement());
    }
    return n1;
}


/* block ::= '{' stmt stmts* '}' */
void *block(){

    AST_NODE *node = NULL;
    if(cur_token == OPEN_CURLY){
        sym_tab = push(s_ptr,sym_tab);
        match(OPEN_CURLY);
        if(cur_token != CLOSE_CURLY) 
                node = program(); /* reusing program ::= stmt stmt* */
        match(CLOSE_CURLY);
        sym_tab = pop(s_ptr);
        return node;
    }else {
        return statement();
    } 
    return node;
}





                
void *statement(){

    #ifdef DEBUG
    RULE(STATEMENT,"stmt");
    #endif

    AST_NODE *n1,*n2,*n3,*n4;
    AST_TYPE type;

    switch(cur_token){
        case LET:
            n1 = declaration();
            match(SEMI_COLON);
            return n1;
        case PRINT:
            match(PRINT);
            //match(L_PAREN);
            n1 = expression();
            //match(R_PAREN);
            match(SEMI_COLON);
            return mk_print_node(PRNT,n1);
        case IF_KWD:
            match(IF_KWD);
            //match(L_PAREN);
            n1 = expression();
            //match(R_PAREN);
            n2 = block();
            n3 = NULL;
            if(cur_token == ELSE_KWD){
                match(ELSE_KWD);
                n3 = block();
            }
            return mk_if_node(IF,n1,n2,n3);
        case FOR_STMT:
            match(FOR_STMT);
            loop_counter++;
            match(L_PAREN);
            sym_tab = push(s_ptr,sym_tab);
            if(cur_token != SEMI_COLON) 
                n1 = declaration();
            match(SEMI_COLON);
            if(cur_token != SEMI_COLON) 
                n2 = expression();
            match(SEMI_COLON);
            if(cur_token != R_PAREN)    
                n3 = expression();
            match(R_PAREN);
            n4 = block();
            loop_counter--;
            sym_tab = pop(s_ptr);
            return mk_for_node(FOR,n1,n2,n3,n4);
        case WHILE_STMT:
            match(WHILE_STMT);
            loop_counter++;
            n1 = expression();
            n2 = block();
            loop_counter--;
            return mk_binary_node(WHILE,n1,n2);
        case BREAK:
        case CONTINUE:
            if(loop_counter == 0){
                fprintf(stdout,"Syntax Error:%s outside the loop\n",yytext);
                exit(0);
            }else{
                type = token_to_ast(cur_token); 
                match(cur_token);
                match(SEMI_COLON);
                return mk_flow_node(type);
            }
            return NULL;
        case OPEN_CURLY:
            return block();
        default:
            n1 = expression();
            match(SEMI_COLON);
            return n1;
    }

}


void *declaration(){
    AST_NODE  *node;
    SYM_TABLE *entry;
    TYPE       sym_type;
    char      *id;

    match(LET);
    id = strndup(yytext,yyleng);
    match(ID);
    entry = sym_entry(sym_tab,id,0);
    match(EQ);
    return mk_assign_node(ASSIGN,id,expression(),sym_tab);
}


void *assignment(){
        #define ASSIGN(tok) (cur_token == EQ)
        char     *id = strndup(yytext,yyleng);
        AST_NODE *n1;
        SYM_DATA *s_tab;
        n1 = logical_expression();

        if (ASSIGN(cur_token)){
            match(EQ);
            s_tab = sym_fetch(sym_tab,id); 
            n1 = mk_assign_node(ASSIGN,id,assignment(),s_tab->sym_table);
        }
        return n1;
}




void *expression(){
    AST_NODE *n1 = assignment();
    return n1;
}



void *logical_expression(){
    #define op(cur_token) (cur_token == OR_OP)
    TOKEN_TYPE type;
    AST_NODE *n1 = logical_term();

    while(op(cur_token)){
        type = cur_token;
        match(type);
        n1 = mk_binary_node(token_to_ast(type),n1,logical_term());
    }
    #undef op
    return n1;
}



void *logical_term(){
    #define op(cur_token) (cur_token == AND_OP)
    TOKEN_TYPE type;
    AST_NODE *n1 = logical_factor();

    while(op(cur_token)){
        type = cur_token;
        match(type);
        n1 = mk_binary_node(token_to_ast(type),n1,logical_factor());
    }
    #undef op
    return n1;
}




void *logical_factor(){
    TOKEN_TYPE type = cur_token;

    switch(cur_token){
        case NOT_OP:
            match(type);
            return mk_unary_node(token_to_ast(type),equality_expression());
        default:
            return equality_expression();
    }
}



void *equality_expression(void){
    #define EQ_OP(tok) ( tok == DOUBLE_EQ || tok == NOT_EQ )

    TOKEN_TYPE type;
    AST_NODE *n1;
    n1 = conditional_expression();
    while(EQ_OP(cur_token)){
        type = cur_token;
        match(type);
        n1 = mk_binary_node(token_to_ast(type),n1,conditional_expression());
    }

    #undef EQ_OP
    return n1;

}

void *conditional_expression(void){
    #define CMP_OP(tok) (tok == G_THAN || tok == L_THAN || tok == G_THAN_EQ || tok == L_THAN_EQ)
    TOKEN_TYPE type;
    AST_NODE *n1;
    n1 = Arithmetic_expression();
    while(CMP_OP(cur_token)){
        type = cur_token;
        match(type);
        n1 = mk_binary_node(token_to_ast(type),n1,Arithmetic_expression());
    }
    #undef CMP_OP
    return n1;
}




void *Arithmetic_expression(){

    #define check_op(op) (op == PLUS || op == MINUS)
    TOKEN_TYPE type;
    AST_NODE *n1 = term();

    while(check_op(cur_token)){
        type = cur_token;
        match(type);
        n1 = mk_binary_node(token_to_ast(type),n1,term());
    }
    #undef check_op
    return n1;
}








void *term(void){
    #define check_op(op) (op==DIVIDE||op==MULTIPLY||op==MODULO)
    TOKEN_TYPE type;
    AST_NODE *n1 = factor();
    while(check_op(cur_token)){
        type = cur_token;
        match(type);
        n1 = mk_binary_node(token_to_ast(type),n1,factor());
    }
    return n1;
}



void *factor(){

    #ifdef DEBUG
    RULE(FACTOR,"factor");
    #endif
    long double *num;
    AST_NODE *node;
    TOKEN_TYPE type;
    SYM_DATA *s_tab;
    
    switch(cur_token){
        case FLOAT:{

            num = malloc(sizeof(long double));
            *num = atof(yytext);

            match(FLOAT);
            return mk_float_node(token_to_ast(FLOAT),*num,num); 
        }
        case INTEGER:{

            #ifdef DEBUG
            RULE(NUMBER,yytext);
            #endif

            num = malloc(sizeof(long double));
            *num = atof(yytext);

            match(INTEGER);
            return mk_num_node(token_to_ast(INTEGER),*num,num); 
        
        }
        case L_PAREN:{

            #ifdef DEBUG
            RULE(L_PAREN,yytext);
            #endif

            match(L_PAREN);
            node = expression();

            #ifdef DEBUG
            RULE(R_PAREN,yytext);
            #endif

            match(R_PAREN);
            return node;
        }
        case PLUS:
            match(PLUS);
            node = expression();
            return mk_unary_node(U_PLUS,node);

        case MINUS:
            match(MINUS);
            node = expression();
            return mk_unary_node(U_MIN,node);
        case ID:
            match(ID);   //need to make num node;
            if(cur_token == EQ) 
                break;

            s_tab = sym_fetch(sym_tab,prev_token);
            node = mk_num_node(NO,s_tab->entry->value,&s_tab->entry->value);
            return node;
        case TRUE_EXP:
            type = cur_token;
            match(type);
            return mk_bool_node(token_to_ast(type),true);   
        case FALSE_EXP:
            type = cur_token;
            match(type);
            return mk_bool_node(token_to_ast(type),false);   
            
        case STRING:
            match(STRING);            
            return mk_string_node(STR,strndup(prev_token+1,strlen(prev_token)-2),strlen(prev_token)-2);
        default:
            enter_panic_mode("factor");
    }
    return NULL;
}



