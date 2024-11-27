#include <stdio.h>
#include "defs.h"
#include <stdlib.h>
#include <string.h>

#define DEBUG
#undef DEBUG

#define RELATIONAL_OP(x) (x == DOUBLE_EQ || x == G_THAN || x == L_THAN)

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
             case EQ :
                return ASSIGN;
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


void *program(){
    AST_NODE *node1,*node2;
    if(cur_token == EOS) return NULL;
    node1 = statement();
    node2 = program();
    return mk_binary_node(STMT,node1,node2);
}


void *statement(){
    
    #ifdef DEBUG
    RULE(STATEMENT,"stmt");
    #endif

    AST_NODE *node;

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
    #ifdef DEBUG
    RULE(ASSIGN,"assign");
    RULE(ID,yytext);
    #endif
    match(ID);
    match(EQ);
    return mk_assign_node(ASSIGN,id,expression());
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
            case DOUBLE_EQ:
            case G_THAN:
            case L_THAN:
                return epsilon();
            default:
                printf("Syntax error at line %d near %s\n",line,prev_token);
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
            case DOUBLE_EQ:
            case G_THAN:
            case L_THAN: 
                return epsilon();
            default:
                printf("Syntax error at line %d near %s\n",line,prev_token);
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
        case STRING:
            match(STRING); 
            return mk_string_node(STR,prev_token,strlen(prev_token));
        default:
            printf("Syntax error at line %d\n near %s\n",line,prev_token);
            exit(ERROR);
            
    }
    return NULL;
}

void *epsilon(){
    return NULL;
}


