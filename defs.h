#ifndef DEFS_H
#define DEFS_H
#include <stdio.h>

#define RULE(x,y) printf("%s : %s\n",#x,y);
#define ERROR 100
#define SUCCESS 200


struct AST_NODE;



/*-----------ENUMS FOR TOKENS -----------------*/

typedef enum {
    EOS,
    NUM,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    L_PAREN,
    R_PAREN,
    EQ,
    SEMI_COLON,
    AND_OP,
    OR_OP,
    NOT_OP,
    ID,
    LET,
    PRINT,
    DOUBLE_EQ,
    G_THAN,
    L_THAN,
    L_THAN_EQ,
    G_THAN_EQ,
    NOT_EQ,
    TRUE_EXP,
    FALSE_EXP,
    STRING,
    IF_KWD,
    ELSE_KWD,
    OPEN_CURLY,
    CLOSE_CURLY,
    WHILE_STMT,
    FOR_STMT,
    BREAK,
    CONTINUE,
    LIST_STMT
}TOKEN_TYPE;


typedef enum{
    STMT=1,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    U_PLUS,
    U_MIN,
    ASSIGN,
    NO,
    PRNT,
    STR,
    DEQ,
    GT,
    LT,
    LEQ,
    GEQ,
    NEQ,
    AND,
    OR,
    NOT,
    IF,
    ELSE,
    FOR,
    WHILE,
    BRK,
    CONT,
    TRUE,
    FALSE,
    LIST
}AST_TYPE;

typedef enum{
    false = 0,
    true = 1
}Boolean;

typedef enum{
    NONE,
    BRK_FLOW,
    CONT_FLOW
}FLOW;



/*-----------------------------------------------------------*/


/*----------------- SYMBOL TABLE ---------------------------*/




#define SYM_TABLE_S 50

typedef struct SYM_TABLE{
    char *id_name;
    long double value;
    long double *ref;
    struct AST_NODE *value_ptr;
    struct SYM_TABLE *next;
}SYM_TABLE;

typedef struct{
    SYM_TABLE **sym_table;
    size_t counter;
}S_TABLE;


size_t      hash(char*,size_t);
S_TABLE     *sym_create();
void        sym_entry(S_TABLE*,char*,long double);

SYM_TABLE   *sym_fetch(S_TABLE*,char*);

int         sym_check(S_TABLE *,char *,int);
void        sym_update(void *,void *);

/*---------------------------------------------------------*/








/*------------------ AST TREE STRUCTURE ------------------*/

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
            S_TABLE *sym_tab; 
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
        
        struct {
            struct AST_NODE *exp;
            struct AST_NODE *left;
            struct AST_NODE *right;
        }If;

        struct {
            struct AST_NODE *init;
            struct AST_NODE *cond;
            struct AST_NODE *exp;
            struct AST_NODE *stmts;
        }For;

        struct {
            Boolean value;
        }Bool;
        
        struct{
            struct AST_NODE **elements;
            size_t size;
            size_t capacity;
        }List;
    }node;

}AST_NODE;


/*----------------------------------------------------------*/




/*-------------- SCOPE STACK ---------------------------*/

typedef struct{
    S_TABLE **stack;
    size_t size;
    size_t top;
}SCOPE_STACK;

void *mk_stack(SCOPE_STACK *,size_t);
S_TABLE *push(SCOPE_STACK *,S_TABLE *);
S_TABLE *pop(SCOPE_STACK *);

/*-------------------------------------------------------*/


/*------------------ AST FUNCTIONS ----------------------*/

AST_NODE *mk_binary_node(AST_TYPE,AST_NODE *,AST_NODE *);
AST_NODE *mk_num_node(AST_TYPE ,long double,void *);

AST_NODE *mk_unary_node(AST_TYPE,AST_NODE *);

AST_NODE *mk_assign_node(AST_TYPE,char *,AST_NODE *,S_TABLE *);

AST_NODE *mk_print_node(AST_TYPE,AST_NODE *);

AST_NODE *mk_string_node(AST_TYPE,char *,size_t);

AST_NODE *mk_if_node(AST_TYPE,AST_NODE*,AST_NODE *,AST_NODE *);

AST_NODE *mk_for_node(AST_TYPE,AST_NODE *,AST_NODE *,AST_NODE *,AST_NODE *);

AST_NODE *mk_bool_node(AST_TYPE ,Boolean );


AST_NODE *mk_flow_node(AST_TYPE);

int check_binary(AST_TYPE);
int check_unary(AST_TYPE);

/*--------------------------------------------------*/

/*---------------- PARSER FUNCTIONS -----------------*/

void    match(TOKEN_TYPE); // match token and move lookahead to next token
void    *statement();
void    *assignment();
void    *expression();
void    *logical_expression();
void    *logical_expression_prime();
void    *logical_term();
void    *logical_term_prime();
void    *logical_factor();
void    *conditional_expression();
void    *conditional_expression_prime();
void    *Arithmetic_expression();
void    *Arithmetic_expression_prime();
void    *term();
void    *term_prime();
void    *factor();
void    *epsilon();
void    *program();
void    *declaration();
void    *block();
long double    eval_ast(AST_NODE*);
/*------------------------------------------------------*/


/*--------------- SHARED FUNCTIONS---------------------*/


extern int yylex();
extern SYM_TABLE *sym_fetch(S_TABLE*,char*);
extern void sym_entry(S_TABLE *table,char *id,long double value);
extern S_TABLE *sym_create();
extern void sym_view(S_TABLE *);


/*----------------------------------------------------*/


/*-------------- SHARED VARIABLES --------------------*/

extern S_TABLE *sym_tab;
extern char *yytext;
extern FILE *yyin;
extern S_TABLE *sym_tab;
extern TOKEN_TYPE cur_token;
extern int yyleng;
extern int line;
extern char *prev_token;
extern int yylineno;
extern int line;

/*------------------------------------------------------*/

#endif
