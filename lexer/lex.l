%{
#include <stdio.h>
#include "defs.h"

int line = 1;

%}


%%


"=="                                                {    return DOUBLE_EQ; }
">"                                                 {    return G_THAN;    }
"<"                                                 {    return L_THAN;    }
"<="                                                {    return L_THAN_EQ; }
">="                                                {    return G_THAN_EQ; }
"!="                                                {    return NOT_EQ;    }
"++"                                                {    return PLUS_PLUS;     }
"--"                                                {   return MINUS_MINUS;    }
"+"                                                 {    return PLUS;      }
"-"                                                 {    return MINUS;     }
"/"                                                 {    return DIVIDE;    }
"*"                                                 {    return MULTIPLY;  }
"%"                                                 {    return MODULO;    }
")"                                                 {    return R_PAREN;   }
"("                                                 {    return L_PAREN;   }
"="                                                 {    return ASSIGN;    }
":"                                                 {   return COLON;      }
";"                                                 {    return SEMI_COLON;    }
\{                                                  {    return OPEN_CURLY;    }
\}                                                  {    return CLOSE_CURLY;   }


"let"                                               {return LET;}
"int"                                               {return INTEGER;}
"float"                                             {return FLOAT;}
"str"                                               {return STRING;}
"true"                                              {return TRUE_EXP;}
"false"                                             {return FALSE_EXP;}
"print"                                             {return PRINT;}
"if"                                                {return IF_KWD;}
"else"                                              {return ELSE_KWD;}
"for"                                               {return FOR_STMT;}
"while"                                             {return WHILE_STMT;}
"and"                                               {return AND_OP;}
"or"                                                {return OR_OP;}
"not"                                               {return NOT_OP;}
"break"                                             {return BREAK;}
"continue"                                          {return CONTINUE;}



(\"|\')(.?+)(\"|\')                                 {       return STRING;      }
[a-zA-Z_][a-zA-Z_]*                                 {       return ID;          }
(0|[1-9][0-9]*)(\.[0-9]+)                           {       return FLOAT;         }
(0|[1-9][0-9]*)                                     {       return INTEGER;     }
[ \t]                                               {       ;                   }
(\/\/|#)[^\\n]*                                     {       line++;             }
\/\*([^*]|\*+[^*/])*\*+\/                           {       line++;             }
\n                                                  {       line++;             }


%%
