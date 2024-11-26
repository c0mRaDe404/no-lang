%{
#include <stdio.h>
#include "defs.h"

%}


%%

"let"                   {return LET;}

[a-zA-Z_][a-zA-Z_]*     {return ID;}
[0-9]+("."[0-9]+)?      {return NUM;}
"+"                     {return PLUS;}
"-"                     {return MINUS;}
"/"                     {return DIVIDE;}
"*"                     {return MULTIPLY;}
")"                     {return R_PAREN;}
"("                     {return L_PAREN;}
"="                     {return EQ;     }
[ \n\t]                 {       ;       }
";"                     {return SEMI_COLON;}

%%
