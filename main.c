#include <stdlib.h>
#include "defs.h"
#include <stdio.h>

#define S_VIEW

TOKEN_TYPE cur_token;
char *prev_token;
S_TABLE *sym_tab;



int main(int argc,char **argv){
    FILE *file;
    sym_tab = sym_create();

    if(argc < 2) {
        //printf("usage : calc <file> ");
        
        //return 1;
    }else {
        file = fopen(argv[1],"r");
        yyin = file;
    }
    cur_token = yylex();    
    AST_NODE *root = program();
    long double value = eval_ast(root);
    
    #ifdef S_VIEW
    sym_view(sym_tab);
    #endif

    #ifdef DEBUG
    printf("%0.15Lf\n",value);
    #endif

    return 0;
}
