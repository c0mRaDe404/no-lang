#ifndef DEFS_H
#define DEFS_H
#include <stdio.h>

typedef enum {
    EOS,
    NUM,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    L_PAREN,
    R_PAREN
}TOKEN_TYPE;


typedef enum{
    ADD=1,
    SUB,
    MUL,
    DIV,
    NO
}AST_TYPE;


typedef struct AST_NODE{
    AST_TYPE ast_type;
    union{

        struct {
            struct AST_NODE *left;
            struct AST_NODE *right;
        }Binary;

        struct{
            int num;
        }Unary;

    }node;
}AST_NODE;


AST_NODE *mk_binary_node(AST_TYPE type,AST_NODE *left,AST_NODE *right);
AST_NODE *mk_num_node(AST_TYPE type,int num);


#define RULE(x,y) printf("%s : %s\n",#x,y);
#define ERROR 100
#define SUCCESS 200


extern int yylex();
extern char *yytext;
extern FILE *yyin;


extern int yylineno;

void    match(TOKEN_TYPE); // match token and move lookahead to next token
void    *statement();
void    *expression();
void    *exp_prime();
void    *term();
void    *term_prime();
void    *factor();
void    *epsilon();
long double     eval_ast(AST_NODE*);

#endif
