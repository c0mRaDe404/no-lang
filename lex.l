%{
#include <stdio.h>
#include "defs.h"

int line = 1;

%}


%%


"=="                        {return DOUBLE_EQ;}
">"                         {return G_THAN;}
"<"                         {return L_THAN;}
"+"                         {return PLUS;}
"-"                         {return MINUS;}
"/"                         {return DIVIDE;}
"*"                         {return MULTIPLY;}
")"                         {return R_PAREN;}
"("                         {return L_PAREN;}
"="                         {return EQ;     }
";"                         {return SEMI_COLON;}
\{                         {return OPEN_CURLY;}
\}                         {return CLOSE_CURLY;}


"let"                       {return LET;}
"true"                      {return TRUE;}
"false"                     {return FALSE;}
"print"                     {return PRINT;}
"if"                        {return IF_KWD;}
"else"                      {return ELSE_KWD;}


(\"|\')(.+?)([^\\n\t\"]+)(\"|\') {return STRING;}
[a-zA-Z_][a-zA-Z_]*         {return ID;}
[0-9]+(\.[0-9]+)?           {return NUM;}
[ \t]                       {       ;       }
\n                          {     line++;  }
%%
