#include <stdlib.h>
#include "defs.h"
#include <stdio.h>

TOKEN_TYPE cur_token;

void match(TOKEN_TYPE token) {

    if(token == cur_token) {
        cur_token = yylex();
    }else{
        printf("Syntax error in line %d\n",yylineno);
        exit(ERROR);
    }
}
void *statement(){
    //RULE(STATEMENT,"stmt");
    AST_NODE *root = expression();
    return root;
}


void *expression(){
    //RULE(EXPRESSION,"exp");
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
        //RULE(SYMBOL,yytext);
        match(type);
        AST_NODE *node1 = term();
        AST_NODE *node2 = exp_prime();

        if(node2 == NULL){
            return mk_binary_node(((type == PLUS) ? ADD:SUB),NULL,node1);
        }else{
            node2 = mk_binary_node(((type == PLUS) ? ADD:SUB),node1,node2);
            return node2;
        }

    }else{
        //expr_prime follow set

        switch(cur_token){
            case EOS:
            case R_PAREN :
                return epsilon();
            default:
                printf("Syntax error at line %d\n",yylineno);
                exit(ERROR);
        }    }

    return NULL;
}

void *term(){
    //RULE(TERM,"term");
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
        //RULE(SYMBOL,yytext);
        match(type);
        AST_NODE *node1 = factor();
        AST_NODE *node2 = term_prime();

        if(node2 == NULL){
            return mk_binary_node(((type == DIVIDE) ? DIV:MUL),NULL,node1);
        }else{
            node2 = mk_binary_node(((type == DIVIDE) ? DIV:MUL),node1,node2);
            return node2;
        }

    }else{

        //term prime follow set
        switch(cur_token){
            case PLUS :
            case MINUS:
            case EOS:
            case R_PAREN:
                return epsilon();
            default:
                printf("Syntax error at line %d\n",yylineno);
                exit(ERROR);
        }
    }

    return NULL;
}

void *factor(){
    //RULE(FACTOR,"factor");
    int num;
    switch(cur_token){
        case NUM:
            //RULE(NUMBER,yytext);
            num = atoi(yytext);
            match(NUM);
            return mk_num_node(NO,num); 
        case L_PAREN:
            //RULE(L_PAREN,yytext);
            match(L_PAREN);
            expression();
            //RULE(R_PAREN,yytext);
            match(R_PAREN);
            return NULL;
        default:
            printf("Syntax error at line %d\n",yylineno);
            exit(ERROR);
    }
    return NULL;
}

void *epsilon(){
    return NULL;
}



int main(int argc,char **argv){
    FILE *file;

    if(argc < 2) {
        printf("usage : calc <file> ");
        return 1;
    }else {
        file = fopen(argv[1],"r");
        yyin = file;
    }
    cur_token = yylex();    
    AST_NODE *root = statement();
    long double value = eval_ast(root);
    printf("%Lf\n",value);
    return 0;
}
