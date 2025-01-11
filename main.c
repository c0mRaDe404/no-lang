#include <stdlib.h>
#include "defs.h"
#include <stdio.h>

#define S_VIEW

TOKEN_TYPE cur_token;
char *prev_token;
SYM_TABLE *sym_tab;
size_t loop_counter = 0;
SCOPE_STACK *s_ptr;


int main(int argc,char **argv){
    FILE *file;
    s_ptr = mk_scope_stack(100);
    scope_enter(s_ptr,sym_create());
    
    if(argc < 2) {
        //printf("usage : calc <file> ");
        
        //return 1;
    }else {
        file = fopen(argv[1],"r");
        yyin = file;
    }
    cur_token = yylex();    
    AST_NODE *root = program();
    if(root != NULL)
        eval_ast(root);
    
    #ifdef S_VIEW
    sym_view(s_ptr);
    #endif

    #ifdef DEBUG
    printf("%0.15Lf\n",value);
    #endif
    return 0;
}
