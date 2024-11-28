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
    R_PAREN,
    EQ,
    SEMI_COLON,
    ID,
    LET,
    PRINT,
    DOUBLE_EQ,
    G_THAN,
    L_THAN,
    TRUE,
    FALSE,
    STRING,
    IF_KWD,
    ELSE_KWD,
    OPEN_CURLY,
    CLOSE_CURLY,
    WHILE_STMT
}TOKEN_TYPE;


typedef enum{
    STMT=1,
    ADD,
    SUB,
    MUL,
    DIV,
    U_PLUS,
    U_MIN,
    ASSIGN,
    NO,
    PRNT,
    STR,
    DEQ,
    GT,
    LT,
    IF,
    ELSE,
    WHILE
}AST_TYPE;


typedef struct AST_NODE{
    AST_TYPE ast_type;
    union{

        struct {
            struct AST_NODE *left;
            struct AST_NODE *right;
        }Binary;

        struct{
            long double *ref;
            long double num;
        }Unary;

        struct{
            struct AST_NODE *factor;
        }Signed;

        struct{
            char *id_name;
            struct AST_NODE *value;
        }Assign;

        struct Program{
            struct AST_NODE *node;
            struct Program  *stmt;
        }Stmt;

        struct {
            char *keyword;
            struct AST_NODE *expr;
        }Print;

        struct {
            char *string;
            size_t len;
        }Str;

    }node;

}AST_NODE;

/*-----------------------------*/

#define SYM_TABLE_S 50

typedef struct SYM_TABLE{
    char *id_name;
    long double value;
    long double *ref;
    AST_NODE *value_ptr;
    struct SYM_TABLE *next;
}SYM_TABLE;

typedef struct{
    SYM_TABLE **sym_table;
    size_t counter;
}S_TABLE;


S_TABLE *sym_create();
size_t hash(char*,size_t);
void sym_entry(S_TABLE*,char*,long double);

SYM_TABLE *sym_fetch(S_TABLE*,char*);

int sym_check(S_TABLE *,char *);

/*------------------------------*/

AST_NODE *mk_binary_node(AST_TYPE type,AST_NODE *left,AST_NODE *right);
AST_NODE *mk_num_node(AST_TYPE type,long double num);

AST_NODE *mk_unary_node(AST_TYPE type,AST_NODE *factor);

AST_NODE *mk_assign_node(AST_TYPE type,char *id,AST_NODE *expr);

AST_NODE *mk_print_node(AST_TYPE type,AST_NODE *expr);

AST_NODE *mk_string_node(AST_TYPE type,char *string,size_t length);

#define RULE(x,y) printf("%s : %s\n",#x,y);
#define ERROR 100
#define SUCCESS 200


extern int yylex();
extern char *yytext;
extern FILE *yyin;
extern S_TABLE *sym_tab;
extern SYM_TABLE *sym_fetch(S_TABLE*,char*);
extern TOKEN_TYPE cur_token;
extern int yyleng;
extern int line;
extern char *prev_token;
extern int yylineno;
extern int line;
extern void sym_entry(S_TABLE *table,char *id,long double value);
extern S_TABLE *sym_tab;
extern S_TABLE *sym_create();
extern void sym_view(S_TABLE *);


void    match(TOKEN_TYPE); // match token and move lookahead to next token
void    *statement();
void    *assignment();
void    *expression();
void    *exp_prime();
void    *term();
void    *term_prime();
void    *factor();
void    *epsilon();
void    *program();
void    *declaration();
void    *block();
long double    eval_ast(AST_NODE*);

#endif
