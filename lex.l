%{
#include <stdio.h>
#include "defs.h"

int line = 1;

%}


%%

"let"                       {return LET;}
"true"                      {return TRUE;}
"false"                     {return FALSE;}
"print"                     {return PRINT;}



^(\"|\')(.+?)[^\"]+(\"|\')$ {return STRING;}
[a-zA-Z_][a-zA-Z_]*         {return ID;}
[0-9]+(\.[0-9]+)?           {return NUM;}
"+"                         {return PLUS;}
"-"                         {return MINUS;}
"/"                         {return DIVIDE;}
"*"                         {return MULTIPLY;}
")"                         {return R_PAREN;}
"("                         {return L_PAREN;}
"="                         {return EQ;     }
[ \t]                       {       ;       }
\n                          {     line++;  }
";"                         {return SEMI_COLON;}
"=="                        {return DEQ;}
">"                         {return GT;}
"<"                         {return LT;}

%%
