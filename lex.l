%{
#include <stdio.h>
#include "defs.h"

%}


%%

[0-9]+      {return NUM;}
"+"         {return PLUS;}
"-"         {return MINUS;}
"/"         {return DIVIDE;}
"*"         {return MULTIPLY;}
")"         {return R_PAREN;}
"("         {return L_PAREN;}
[ \n\t]          { ; }
%%
