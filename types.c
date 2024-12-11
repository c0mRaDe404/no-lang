#include "defs.h"
#include <stdlib.h>


TYPE get_type(TOKEN_TYPE type){

    switch(type){
        case INTEGER:
            return INT_TYPE;
        case FLOAT:
            return FLOAT_TYPE;
        case TRUE_EXP:
        case FALSE_EXP:
            return BOOL_TYPE;
        case STRING:
            return STR_TYPE;
        default:
            return NONE_TYPE;
    }

}


TYPE ast_to_type(AST_TYPE type){

    switch(type){
        case INT:
            return INT_TYPE;
        case FLOAT:
            return FLOAT_TYPE;
        case STRING:
            return STR_TYPE;
        case TRUE:
        case FALSE: 
            return BOOL_TYPE;
        default:
            return NONE_TYPE;
    }
}


