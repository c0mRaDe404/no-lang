#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>

#define RULE(x,y) printf("%s : %s\n",#x,y);
#define ERROR     100
#define SUCCESS   200





/*-----------ENUMS FOR TOKENS -----------------*/

typedef enum {
    EOS, NUM, INTEGER, FLOAT, PLUS,
    MINUS, MULTIPLY, DIVIDE, MODULO,
    L_PAREN, R_PAREN, EQ, COLON, SEMI_COLON,
    AND_OP, OR_OP, NOT_OP, ID, LET, PRINT,
    DOUBLE_EQ, G_THAN, L_THAN, L_THAN_EQ,
    G_THAN_EQ, NOT_EQ, TRUE_EXP, FALSE_EXP,
    STRING, IF_KWD, ELSE_KWD, OPEN_CURLY,
    CLOSE_CURLY, WHILE_STMT, FOR_STMT,
    BREAK, CONTINUE, PLUS_PLUS,FUNC_DEF,
    MINUS_MINUS,COMMA_OP,LIST_STMT,FUNC_CALL
}TOKEN_TYPE;


typedef enum{
    STMT=1, ADD  ,SUB,MUL, DIV,
    MOD , U_PLUS, U_MIN, ASSIGN,
    NO, INT, FLT, PRNT, STR, DEQ,
    GT, LT, LEQ, GEQ, NEQ, AND, 
    OR, NOT, IF, ELSE, FOR, WHILE,
    BRK, CONT, PR_INC, PR_DEC, PO_INC,
    PO_DEC, TRUE, FALSE, LIST,FDEF,FCALL
}AST_TYPE;

typedef enum{
    False = 0,
    True = 1
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
}DATA_TYPE;




typedef struct{
    DATA_TYPE type;
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
            size_t length;
        }String;
    };

}VALUE;


DATA_TYPE get_type(TOKEN_TYPE);
DATA_TYPE   ast_to_type  (AST_TYPE type);
VALUE *mk_value     (DATA_TYPE,long double,void *);
/*---------------------------------------------------------*/




/*----------------- SYMBOL TABLE ---------------------------*/




#define SYM_TABLE_S 50

/* symbol record */

typedef enum{
    SYMBOL_GLOBAL,
    SYMBOL_LOCAL,
    SYMBOL_PARAM
}SYM_TYPE;

typedef struct SYM_ENTRY{
    char *id_name; /* symbol name */
    SYM_TYPE symbol_type;
    long double value;   /* actual data */
    long double *ref;   /* pointer to the data */
    VALUE *Value;      /* struct for storing the data */
    struct AST_NODE *node;
    struct SYM_ENTRY *next; /* pointer for chaining the entries together */
}SYM_ENTRY;

/* symbol table - collection of symbol entries */
typedef struct SYM_TABLE{
    SYM_ENTRY **sym_table; /* array of symbol entries (each variable have its own) */
    struct SYM_TABLE *parent; /* pointer that points to its enclosing scope or parent scope */
    struct SYM_TABLE *child; /* pointer to the child scope */
    size_t counter; /* counter for symbol table */
}SYM_TABLE;


/* stores the entry and the symbol table of its scope */
typedef struct{
    SYM_TABLE *sym_table; /* current scope */
    SYM_ENTRY *entry; /* entry of one identifier */
}SYM_DATA;

size_t         hash        (char*,size_t);
SYM_TABLE     *sym_create  ();
SYM_ENTRY     *sym_entry   (char*,long double);

SYM_DATA      *sym_fetch   (SYM_TABLE*,char*);

int          sym_check   (SYM_TABLE *,char *,int);
SYM_ENTRY         *sym_update  (SYM_TABLE*,char *,long double);

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
        
        struct{
            char *f_name;
            char **f_params;
            size_t par_count;
            struct AST_NODE *f_body;
        }Func_def;

        struct{
            struct AST_NODE *callee;
            struct AST_NODE **argv;
            size_t argc;
        }Func_call;



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
    SYM_TABLE **stack;
    size_t size;
    size_t top;
}SCOPE_STACK;

void    *mk_scope_stack (size_t);
SYM_TABLE *scope_enter     (SCOPE_STACK *,SYM_TABLE *);
SYM_TABLE *scope_exit      (SCOPE_STACK *);

SYM_TABLE *get_cur_scope(SCOPE_STACK *stack);
/*-------------------------------------------------------*/


/*------------------ AST FUNCTIONS ----------------------*/

AST_NODE *mk_binary_node   (AST_TYPE,AST_NODE *,AST_NODE *);
AST_NODE *mk_num_node      (AST_TYPE ,long double,void *);

AST_NODE *mk_unary_node    (AST_TYPE,AST_NODE *);

AST_NODE *mk_assign_node   (AST_TYPE,char *,AST_NODE *,SYM_TABLE *);

AST_NODE *mk_print_node    (AST_TYPE,AST_NODE *);

AST_NODE *mk_string_node   (AST_TYPE,char *,size_t);

AST_NODE *mk_if_node       (AST_TYPE,AST_NODE*,AST_NODE *,AST_NODE *);

AST_NODE *mk_for_node      (AST_TYPE,AST_NODE *,AST_NODE *,AST_NODE *,AST_NODE *);

AST_NODE *mk_bool_node     (AST_TYPE ,Boolean );

AST_NODE *mk_float_node    (AST_TYPE type,long double num,long double *ref);

AST_NODE *mk_int_node      (AST_TYPE type,int num,int *ref);

AST_NODE *mk_func_call_node(AST_TYPE,AST_NODE*,AST_NODE **,size_t);
AST_NODE *mk_func_def_node(AST_TYPE,char*,char**,size_t,AST_NODE*,SYM_TABLE*);
AST_NODE *mk_flow_node     (AST_TYPE);

int       check_binary     (AST_TYPE);
int       check_unary      (AST_TYPE);

/*--------------------------------------------------*/

/*---------------- PARSER FUNCTIONS -----------------*/

int    match(TOKEN_TYPE); // match token and move lookahead to next token


void    *program                  (void);
void    *block                    (void);
void    *statement                (void);
void    *assignment               (void);
void    *declaration              (void);
void    *expression               (void);
void    *logical_expression       (void);
void    *logical_term             (void);
void    *logical_factor           (void);
void    *equality_expression      (void);
void    *conditional_expression   (void);
void    *Arithmetic_expression    (void);
void    *term                     (void);
void    *factor                   (void);
void    *parse_func_def           (void);
void    *parse_func_call                (void);
void    *check_assign             (void);



/* interpreter ( tree walker ) */

long double eval_ast              (AST_NODE*);

/*------------------------------------------------------*/


/*--------------- SHARED FUNCTIONS---------------------*/


extern int        yylex        ();
extern SYM_DATA  *sym_fetch    (SYM_TABLE*,char*);
extern SYM_ENTRY *sym_entry    (char *,long double);
extern SYM_TABLE   *sym_create   ();
extern void       sym_view     (SCOPE_STACK *);


/*----------------------------------------------------*/


/*-------------- SHARED VARIABLES --------------------*/

extern SYM_TABLE *sym_tab;
extern char *yytext;
extern FILE *yyin;
extern SYM_TABLE *sym_tab;
extern TOKEN_TYPE cur_token;
extern int yyleng;
extern int line;
extern char *prev_token;
extern int yylineno;
extern int line;

/* parser.c */

extern size_t loop_counter;
extern SCOPE_STACK *s_ptr;

/*------------------------------------------------------*/

#endif
