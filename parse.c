#include <stdio.h>
#include "defs.h"
#include <stdlib.h>
#include <string.h>

#define DEBUG
#undef DEBUG

#define RELATIONAL_OP(token) (token == DOUBLE_EQ || token == G_THAN || token == L_THAN)
#define END_OF_FILE(token) (token == EOS) 
#define END_OF_STMT(token) (token == CLOSE_CURLY)

AST_TYPE token_to_ast(TOKEN_TYPE type){
        switch(type){
            case PLUS:
                return ADD;
             case MINUS:
                return SUB;
             case MULTIPLY:
                return MUL;
             case DIVIDE:
                return DIV;
             case DOUBLE_EQ:
                return DEQ;
             case G_THAN:
                return GT;
             case L_THAN:
                return LT;
             case PRINT:
                return PRNT;
             case MODULO:
                return MOD;
             case EQ :
                return ASSIGN;
            case AND_OP:
                return AND;
            case OR_OP:
                return OR;
             default:
                return 0;
        }
}


void match(TOKEN_TYPE token) {
    
    if(token == cur_token) {
        prev_token = strndup(yytext,yyleng);
        cur_token = yylex();
    }else{
        printf("Syntax error in line %d near %s\n",line,prev_token);
        exit(ERROR);
    }
}


void *block(){
    
    AST_NODE *node = NULL;
    if(cur_token == OPEN_CURLY){
    match(OPEN_CURLY);
    node = program();
    match(CLOSE_CURLY);
    return node;
    }else {
        return program();
    } 
    return NULL;
}

void *program(){
    AST_NODE *node1,*node2;
    #define END(token) (END_OF_FILE(token) || END_OF_STMT(token))
    if(END(cur_token)) return NULL;
    node1 = statement();
    node2 = program();
    return mk_binary_node(STMT,node1,node2);
}


void *statement(){
    
    #ifdef DEBUG
    RULE(STATEMENT,"stmt");
    #endif

    AST_NODE *node;
    AST_NODE *left;
    AST_NODE *right;

    switch(cur_token){
        case LET:
            node = declaration();
            match(SEMI_COLON);
            return node;
        case PRINT:
            match(PRINT);
            match(L_PAREN);
            node = expression();
            match(R_PAREN);
            match(SEMI_COLON);
            return mk_print_node(PRNT,node);
        case IF_KWD:
            match(IF_KWD);
            node = expression();
            left = block();
            right = NULL;
            if(cur_token == ELSE_KWD){
                match(ELSE_KWD);
                right = block();
            }
            return mk_if_node(IF,node,left,right);
        case WHILE_STMT:
            match(WHILE_STMT);
            left = expression();
            right = block();
            return mk_binary_node(WHILE,left,right);
        default:
            node = expression();
            match(SEMI_COLON);
            return node;
    }

    //node = expression();
    //match(SEMI_COLON);
    //return node;
}


void *declaration(){

    match(LET);
    AST_NODE *node;
    char *id = strndup(yytext,yyleng);
    sym_entry(sym_tab,id,0);
    #ifdef DEBUG
    RULE(ASSIGN,"assign");
    RULE(ID,yytext);
    #endif
    match(ID);
    match(EQ);
    return mk_assign_node(ASSIGN,id,expression());
}


void *assignment(){
    
    AST_NODE *node;
    char *id = strndup(yytext,yyleng);
    if(sym_check(sym_tab,id)){
            match(ID);
            match(EQ);
            return mk_assign_node(ASSIGN,id,expression());
    }else return NULL;
}


void *expression(){
    #ifdef DEBUG
    RULE(EXPRESSION,"exp");
    #endif

    AST_NODE *node1 = term();
    AST_NODE *node2 = exp_prime();
    TOKEN_TYPE type;

    if(node2 == NULL){

        if(RELATIONAL_OP(cur_token)){
                type = cur_token;
                match(type);
                return  mk_binary_node(token_to_ast(type),node1,expression());
        }else if(cur_token == EQ){
            type = cur_token;
            char *id = strndup(yytext,yyleng);
            match(type);
            return mk_assign_node(ASSIGN,id,assignment());
        }
        return node1;
    }else{
        node2->node.Binary.left = node1;

        if(RELATIONAL_OP(cur_token)){
            type = cur_token;
            match(type);
            return  mk_binary_node(token_to_ast(type),node2,expression());
        }

        return node2;
    }
    return NULL;
}

void *exp_prime(){

    #define check_op(op) (op==PLUS||op==MINUS||op==AND_OP||op==OR_OP)
    #define find_op_type(op) ((op == PLUS)? PLUS : (op== MINUS)? MINUS : (op==AND_OP)? AND_OP : OR_OP)

    if(check_op(cur_token)) {

        TOKEN_TYPE type = find_op_type(cur_token);
        
        #ifdef DEBUG
        RULE(SYMBOL,yytext);
        #endif

        match(type);
        AST_NODE *node1 = term();
        AST_NODE *node2 = exp_prime();

        if(node2 == NULL){
            return mk_binary_node(token_to_ast(type),NULL,node1);
        }else{

            node2->node.Binary.left = node1;
            node2 = mk_binary_node(token_to_ast(type),node1,node2);
            return node2;
        }

    }else{
        //expr_prime follow set

        switch(cur_token){
            case EOS:
            case SEMI_COLON:
            case R_PAREN :
            case DOUBLE_EQ:
            case G_THAN:
            case L_THAN:
            case OPEN_CURLY:
                return epsilon();
            default:
                printf("Syntax error at line %d near %s\n",line,prev_token);
                exit(ERROR);
        }    }
    
    #undef check_op
    #undef find_op_type
    return NULL;
}

void *term(){
    
    #ifdef DEBUG
    RULE(TERM,"term");
    #endif
    AST_NODE *node1 = factor();
    AST_NODE *node2 = term_prime();
    if(node2 == NULL){
        return node1;
    }else{
        node2->node.Binary.left = node1;
        return node2;
    }
    return NULL;
}

void *term_prime(){

    #define check_op(op) (op==DIVIDE||op==MULTIPLY||op==MODULO)
    #define find_op_type(op) (op == DIVIDE)? DIVIDE:(op == MODULO) ? MODULO : MULTIPLY

    if(check_op(cur_token)){
        TOKEN_TYPE type = find_op_type(cur_token);        
        #ifdef DEBUG
        RULE(SYMBOL,yytext);
        #endif
        match(type);
        AST_NODE *node1 = factor();
        AST_NODE *node2 = term_prime();

        if(node2 == NULL){
            return mk_binary_node(((type == DIVIDE) ? DIV:(type == MODULO) ? MOD : MUL),NULL,node1);
        }else{
            node2->node.Binary.left = node1;
            node2 = mk_binary_node(((type == DIVIDE) ? DIV: (type == MODULO)? MOD : MUL),node1,node2);
            return node2;
        }

    }else{

        //term prime follow set
        switch(cur_token){
            case PLUS :
            case MINUS:
            case AND_OP:
            case OR_OP:
            case EOS:
            case SEMI_COLON:
            case R_PAREN:
            case DOUBLE_EQ:
            case G_THAN:
            case L_THAN:
            case OPEN_CURLY:
                return epsilon();
            default:
                printf("Syntax error at line %d near %s\n",line,prev_token);
                exit(ERROR);
        }
    }

    #undef check_op
    #undef find_op_type
    return NULL;
}

void *factor(){

    #ifdef DEBUG
    RULE(FACTOR,"factor");
    #endif
    long double num;
    AST_NODE *node;
    TOKEN_TYPE type;
    SYM_TABLE *table;
    switch(cur_token){
        case NUM:

            #ifdef DEBUG
            RULE(NUMBER,yytext);
            #endif
            num = atof(yytext);
            match(NUM);
            return mk_num_node(NO,num); 
        case L_PAREN:

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
        case PLUS:
        case MINUS:
            type = (cur_token == PLUS) ? PLUS:MINUS;
            match(type);
            node = factor();
            return mk_unary_node((type == PLUS)? U_PLUS:U_MIN,node);
        case ID:
            match(ID);   //need to make num node;
            table = sym_fetch(sym_tab,prev_token);
            node = mk_num_node(NO,table->value);
            node->node.Unary.ref = &table->value; 
            return node;
        case NOT_OP:
            type = cur_token;
            match(type);
            node = expression();
            return mk_unary_node(NOT,node);
        case STRING:
            match(STRING);            
            return mk_string_node(STR,strndup(prev_token+1,strlen(prev_token)-2),strlen(prev_token)-2);
        default:
            printf("Syntax error at line %d\n near %s\n",line,prev_token);
            exit(ERROR);
            
    }
    return NULL;
}

void *epsilon(){
    return NULL;
}


