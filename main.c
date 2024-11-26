#include <stdlib.h>
#include "defs.h"
#include <stdio.h>

TOKEN_TYPE cur_token;

void match(TOKEN_TYPE token) {

    if(token == cur_token) {
        cur_token = yylex();
    }else{
        printf("Syntax error in line %d near %s\n",yylineno,yytext);
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

            node2->node.Binary.left = node1;
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
                printf("Syntax error at line %d near %s\n",yylineno,yytext);
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
    //RULE(FACTOR,"factor");
    long double num;
    AST_NODE *node;
    switch(cur_token){
        case NUM:
            //RULE(NUMBER,yytext);
            num = atof(yytext);
            match(NUM);
            return mk_num_node(NO,num); 
        case L_PAREN:
            //RULE(L_PAREN,yytext);
            match(L_PAREN);
            node = expression();
            //RULE(R_PAREN,yytext);
            match(R_PAREN);
            return node;
        default:
               printf("Syntax error at line %d\n near %s\n",yylineno,yytext);
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
        //printf("usage : calc <file> ");
        
        //return 1;
    }else {
        file = fopen(argv[1],"r");
        yyin = file;
    }
    cur_token = yylex();    
    AST_NODE *root = statement();
    long double value = eval_ast(root);
    printf("%0.15Lf\n",value);
    return 0;
}
