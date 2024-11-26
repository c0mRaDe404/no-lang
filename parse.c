#include <stdio.h>
#include "defs.h"
#include <stdlib.h>


#define DEBUG
extern TOKEN_TYPE cur_token;




void match(TOKEN_TYPE token) {

    if(token == cur_token) {
        cur_token = yylex();
    }else{
        printf("Syntax error in line %d near %s\n",yylineno,yytext);
        exit(ERROR);
    }
}

void *statement(){
    
    #ifdef DEBUG
    RULE(STATEMENT,"stmt");
    #endif

    //AST_NODE *node;

    switch(cur_token){
        case LET:
            match(LET);
            return assignment();
        default:
            return expression();
    }

    //node = expression();
    //match(SEMI_COLON);
    //return node;
}

void *assignment(){
    
    char *id = yytext;
    AST_NODE *node1;
    #ifdef DEBUG
    RULE(ASSIGN,"assign");
    #endif

    match(ID);
    match(EQ);
    node1 = expression();
    return node1;
}


void *expression(){
    #ifdef DEBUG
    RULE(EXPRESSION,"exp");
    #endif

    AST_NODE *node1 = term();
    AST_NODE *node2 = exp_prime();

    if(node2 == NULL){
        return node1;
    }else{
        node2->node.Binary.left = node1;
        return node2;
    }
    return NULL;
}

void *exp_prime(){

    if(cur_token == PLUS || cur_token == MINUS) {

        TOKEN_TYPE type = (cur_token == PLUS)? PLUS:MINUS;
        
        #ifdef DEBUG
        RULE(SYMBOL,yytext);
        #endif

        match(type);
        AST_NODE *node1 = term();
        AST_NODE *node2 = exp_prime();

        if(node2 == NULL){
            return mk_binary_node(((type == PLUS) ? ADD:SUB),NULL,node1);
        }else{

            node2->node.Binary.left = node1;
            node2 = mk_binary_node(((type == PLUS) ? ADD:SUB),node1,node2);
            return node2;
        }

    }else{
        //expr_prime follow set

        switch(cur_token){
            case EOS:
            case SEMI_COLON:
            case R_PAREN :
                return epsilon();
            default:
                printf("Syntax error at line %d near %s\n",yylineno,yytext);
                exit(ERROR);
        }    }

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

    if(cur_token == DIVIDE || cur_token == MULTIPLY){
        TOKEN_TYPE type = (cur_token == DIVIDE)? DIVIDE:MULTIPLY;
        
        #ifdef DEBUG
        RULE(SYMBOL,yytext);
        #endif
        match(type);
        AST_NODE *node1 = factor();
        AST_NODE *node2 = term_prime();

        if(node2 == NULL){
            return mk_binary_node(((type == DIVIDE) ? DIV:MUL),NULL,node1);
        }else{
            node2->node.Binary.left = node1;
            node2 = mk_binary_node(((type == DIVIDE) ? DIV:MUL),node1,node2);
            return node2;
        }

    }else{

        //term prime follow set
        switch(cur_token){
            case PLUS :
            case MINUS:
            case EOS:
            case SEMI_COLON:
            case R_PAREN:
                return epsilon();
            default:
                printf("Syntax error at line %d near %s\n",yylineno,yytext);
                exit(ERROR);
        }
    }

    return NULL;
}

void *factor(){

    #ifdef DEBUG
    RULE(FACTOR,"factor");
    #endif
    long double num;
    AST_NODE *node;
    TOKEN_TYPE type;
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
        default:
               printf("Syntax error at line %d\n near %s\n",yylineno,yytext);
                exit(ERROR);
            
    }
    return NULL;
}

void *epsilon(){
    return NULL;
}


