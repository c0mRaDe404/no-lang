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
    INTEGER,
    FLOAT,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    L_PAREN,
    R_PAREN,
    EQ,
    COLON,
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
    PLUS_PLUS,
    MINUS_MINUS,
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
    INT,
    FLT,
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
    PR_INC,
    PR_DEC,
    PO_INC,
    PO_DEC,
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


/*--------------- TYPES -----------------------------------*/


typedef enum{
    NONE_TYPE,
    INT_TYPE,
    FLOAT_TYPE,
    BOOL_TYPE,
    STR_TYPE,
}TYPE;



TYPE get_type(TOKEN_TYPE);

typedef struct{
    TYPE type;

    union{
        struct {
            int int_val;
            int *ref;
        }Int;

        struct {
            long double float_val;
            long double  *ref;
        }Float;

        struct {
            Boolean bool_val;
        }Bool;

        struct{
            char *string_val;
        }String;

    };

}VALUE;


TYPE ast_to_type(AST_TYPE type);
VALUE *mk_value(TYPE,long double,void *);
/*---------------------------------------------------------*/




/*----------------- SYMBOL TABLE ---------------------------*/




#define SYM_TABLE_S 50

typedef struct SYM_TABLE{
    char *id_name;
    TYPE type;
    long double value;
    long double *ref;

    VALUE *Value;

    struct SYM_TABLE *next;
    //struct AST_NODE *value_ptr;
}SYM_TABLE;

typedef struct{
    SYM_TABLE **sym_table;
    size_t counter;
}S_TABLE;


typedef struct{
    S_TABLE *sym_table;
    SYM_TABLE *entry;
}SYM_DATA;

size_t      hash(char*,size_t);
S_TABLE     *sym_create();
SYM_TABLE   *sym_entry(S_TABLE*,char*,long double);

SYM_DATA   *sym_fetch(S_TABLE*,char*);

int         sym_check(S_TABLE *,char *,int);
void        sym_update(void *,void *);

/*---------------------------------------------------------*/








/*------------------ AST TREE STRUCTURE ------------------*/

typedef struct AST_NODE{
    AST_TYPE ast_type;
    SYM_DATA *sym_data;

    union{

        VALUE *Value;

        struct {
            struct AST_NODE *left; //   left value
            struct AST_NODE *right; //  right value
        }Binary;

        struct{
            struct AST_NODE *factor; // Factor ast_node
        }Unary; // Unary

        struct{
            char *id_name; // identifier's name
            struct AST_NODE *value;  // value ast_node
        }Assign;
        
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
            char *keyword;
            struct AST_NODE *expr;
        }Print;


       
        struct{
            long double *ref; // address of the value
            long double num; // actual value
        }Number; // Number

        struct {
            char *string;
            size_t len;
        }Str;
        
        struct {
            Boolean value;
        }Bool;

        struct {
            struct AST_NODE **elements;
            size_t size;
            size_t capacity;
        }List;
    };

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

AST_NODE *mk_float_node(AST_TYPE type,long double num,long double *ref);

AST_NODE *mk_int_node(AST_TYPE type,int num,int *ref);

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
void    *check_assign();
long double eval_ast(AST_NODE*);
/*------------------------------------------------------*/


/*--------------- SHARED FUNCTIONS---------------------*/


extern int yylex();
extern SYM_DATA *sym_fetch(S_TABLE*,char*);
extern SYM_TABLE *sym_entry(S_TABLE *table,char *id,long double value);
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
