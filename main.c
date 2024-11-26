#include <stdlib.h>
#include "defs.h"
#include <stdio.h>


TOKEN_TYPE cur_token;


S_TABLE *sym_create(){
    S_TABLE *table;
    table = malloc(sizeof(S_TABLE));
    table->sym_table = malloc(SYM_TABLE_S*sizeof(S_TABLE*));
    table->counter = 0;
    return table;
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
