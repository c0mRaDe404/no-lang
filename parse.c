#include <stdio.h>
#include "defs.h"
#include <stdlib.h>
#include <string.h>

extern size_t loop_counter;

extern SCOPE_STACK *s_ptr;

#define DEBUG
#undef DEBUG




#define EPSILON (NULL);
#define LOGICAL_OP(token) (token == AND_OP || token == OR_OP || token == NOT_OP)

#define RELATIONAL_OP(token) (token == DOUBLE_EQ || token == G_THAN || token == L_THAN\
                            || token == L_THAN_EQ || token == G_THAN_EQ || token == NOT_EQ)
#define END_OF_FILE(token) (token == EOS) 
#define END_OF_STMT(token) (token == CLOSE_CURLY)



AST_TYPE token_to_ast(TOKEN_TYPE type){
    switch(type){
        case PLUS:
            return ADD;
        case MINUS:
            return SUB;
        case MULTIPLY:
            return MUL;
        case DIVIDE:
            return DIV;
        case DOUBLE_EQ:
            return DEQ;
        case G_THAN:
            return GT;
        case G_THAN_EQ:
            return GEQ;
        case L_THAN:
            return LT;
        case L_THAN_EQ:
            return LEQ;
        case NOT_EQ:
            return NEQ;
        case PRINT:
            return PRNT;
        case MODULO:
            return MOD;
        case EQ :
            return ASSIGN;
        case AND_OP:
            return AND;
        case OR_OP:
            return OR;
        case NOT_OP:
            return NOT;
        case TRUE_EXP:
            return TRUE;
        case FALSE_EXP:
            return FALSE;
        case BREAK:
            return BRK;
        case CONTINUE:
            return CONT;
        case INTEGER:
            return INT;
        case FLOAT:
            return FLT;
        case STRING:
            return STR;
        default:
            return 0;
    }
}


void match(TOKEN_TYPE token) {

    if(token == cur_token) {
        prev_token = strndup(yytext,yyleng);
        cur_token = yylex();
    }else{
        printf("Syntax error in line %d near %s\n",line,prev_token);
        exit(ERROR);
    }
}








void *program(){
    AST_NODE *n1,*n2;
    #define END(token) (END_OF_FILE(token) || END_OF_STMT(token))
    
    if(END(cur_token)) return NULL;
    n1 = statement();
    n2 = program();
    return mk_binary_node(STMT,n1,n2);
}



void *block(){

    AST_NODE *node = NULL;
    if(cur_token == OPEN_CURLY){
        sym_tab = push(s_ptr,sym_tab);
        match(OPEN_CURLY);
        if(cur_token != CLOSE_CURLY) node = program();
        match(CLOSE_CURLY);
        sym_tab = pop(s_ptr);
        return node;
    }else {
        return program();
    } 
    return NULL;
}






void *statement(){

    #ifdef DEBUG
    RULE(STATEMENT,"stmt");
    #endif

    AST_NODE *n1,*n2,*n3,*n4;
    AST_TYPE type;

    switch(cur_token){
        case LET:
            n1 = declaration();
            match(SEMI_COLON);
            return n1;
        case PRINT:
            match(PRINT);
            match(L_PAREN);
            n1 = expression();
            match(R_PAREN);
            match(SEMI_COLON);
            return mk_print_node(PRNT,n1);
        case IF_KWD:
            match(IF_KWD);
            n1 = expression();
            n2 = block();
            n3 = NULL;
            if(cur_token == ELSE_KWD){
                match(ELSE_KWD);
                n3 = block();
            }
            return mk_if_node(IF,n1,n2,n3);
        case FOR_STMT:
            match(FOR_STMT);
            loop_counter++;
            match(L_PAREN);
            sym_tab = push(s_ptr,sym_tab);
            if(cur_token != SEMI_COLON) 
                n1 = declaration();
            match(SEMI_COLON);
            if(cur_token != SEMI_COLON) 
                n2 = expression();
            match(SEMI_COLON);
            if(cur_token != R_PAREN)    
                n3 = expression();
            match(R_PAREN);
            n4 = block();
            loop_counter--;
            sym_tab = pop(s_ptr);
            return mk_for_node(FOR,n1,n2,n3,n4);
        case WHILE_STMT:
            match(WHILE_STMT);
            loop_counter++;
            n1 = expression();
            n2 = block();
            loop_counter--;
            return mk_binary_node(WHILE,n1,n2);
        case BREAK:
        case CONTINUE:
            if(loop_counter == 0){
                fprintf(stdout,"Syntax Error:%s outside the loop\n",yytext);
                exit(0);
            }else{
                type = token_to_ast(cur_token); 
                match(cur_token);
                match(SEMI_COLON);
                return mk_flow_node(type);
            }
            return NULL;
        case OPEN_CURLY:
            return block();
        default:
            n1 = logical_expression();
            match(SEMI_COLON);
            return n1;
    }

}


void *declaration(){

    match(LET);
    AST_NODE *node;
    SYM_TABLE *entry;
    TYPE sym_type;
    char *id = strndup(yytext,yyleng);
    entry = sym_entry(sym_tab,id,0);
    #ifdef DEBUG
    RULE(ASSIGN,"assign");
    RULE(ID,yytext);
    #endif
    match(ID);
    match(EQ);

    return mk_assign_node(ASSIGN,id,expression(),sym_tab);
}


void *assignment(){
        char *id = prev_token;
        SYM_DATA *s_tab;
        match(EQ);
        s_tab = sym_fetch(sym_tab,id);
        return mk_assign_node(ASSIGN,id,expression(),s_tab->sym_table);
}




void *expression(){

    return logical_expression();
}



void *logical_expression(){
    #ifdef DEBUG
    RULE(EXPRESSION,"exp");
    #endif

    AST_NODE *n1 = logical_term();
    AST_NODE *n2 = logical_expression_prime();

    if(n2 == NULL){      
        return n1;
    }else{
        n2->Binary.left = n1; 
        return n2;
    }
    return NULL;


}


void *logical_expression_prime(){

    if(cur_token == OR_OP) {

        TOKEN_TYPE type = cur_token;

        #ifdef DEBUG
        RULE(SYMBOL,yytext);
        #endif

        match(type);
        AST_NODE *n1 = logical_term();
        AST_NODE *n2 = logical_expression_prime();

        if(n2 == NULL){
            return mk_binary_node(token_to_ast(type),NULL,n1);
        }else{

            n2->Binary.left = n1;
            n2 = mk_binary_node(token_to_ast(type),n1,n2);
            return n2;
        }

    }else{
        //expr_prime follow set

        switch(cur_token){
            case EOS:
            case SEMI_COLON:
            case R_PAREN :
            case OPEN_CURLY:
                return EPSILON;
            default:
                printf("Syntax error at line %d near %s\n",line,prev_token);
                exit(ERROR);
        }    }

    #undef check_op
    #undef find_op_type
    return NULL;


}


void *logical_term(){
    #ifdef DEBUG
    RULE(TERM,"term");
    #endif
    AST_NODE *n1 = logical_factor();
    AST_NODE *n2 = logical_term_prime();
    if(n2 == NULL){
        return n1;
    }else{
        n2->Binary.left = n1;
        return n2;
    }
    return NULL;

}

void *logical_term_prime(){
    

    if(cur_token == AND_OP){
        TOKEN_TYPE type = cur_token;        
        #ifdef DEBUG
        RULE(SYMBOL,yytext);
        #endif

        match(type);
        AST_NODE *n1 = logical_factor();
        AST_NODE *n2 = logical_term_prime();

        if(n2 == NULL){
            return mk_binary_node(token_to_ast(type),NULL,n1);
        }else{
            n2->Binary.left = n1;
            n2 = mk_binary_node(token_to_ast(type),n1,n2);
            return n2;
        }

    }else{

        //logical term prime follow set
        switch(cur_token){
            case EOS:
            case SEMI_COLON:
            case R_PAREN:
            case OPEN_CURLY:
            case OR_OP :
                return EPSILON;
            default:
                printf("Syntax error at line %d near %s\n",line,prev_token);
                exit(ERROR);
        }
    }

    return NULL;

}


void *logical_factor(){
    TOKEN_TYPE type = cur_token;

    switch(cur_token){
        case NOT_OP:
            match(type);
            return mk_unary_node(token_to_ast(type),conditional_expression());
        default:
            return conditional_expression();
    }
}



void *conditional_expression_prime(){
       if(RELATIONAL_OP(cur_token)){
        
        TOKEN_TYPE type = cur_token;
        match(type);
        AST_NODE *node1 = Arithmetic_expression();
        AST_NODE *node2 = conditional_expression_prime();

        if(node2 == NULL){
            return mk_binary_node(token_to_ast(type),NULL,node1);
        }else{

            node2->Binary.left = node1;
            node2 = mk_binary_node(token_to_ast(type),node1,node2);
            return node2;
        }

    }else {
        // follow set
        switch(cur_token){
            case EOS:
            case SEMI_COLON:
            case R_PAREN :
            case OPEN_CURLY:
            case AND_OP:
            case OR_OP:
            case NOT_OP:
                return EPSILON;
            default:
                printf("Syntax error at line %d near %s\n",line,prev_token);
                exit(ERROR);
        }

    }
}


void *conditional_expression(){

    AST_NODE *n1,*n2;
    n1 = Arithmetic_expression();
    n2 = conditional_expression_prime();

    if(n2 == NULL){      
        return n1;
    }else{
        n2->Binary.left = n1; 
        return n2;
    }
    return NULL;
}




void *Arithmetic_expression(){
    #ifdef DEBUG
    RULE(EXPRESSION,"exp");
    #endif

    AST_NODE *node1 = term();
    AST_NODE *node2 = Arithmetic_expression_prime();

    if(node2 == NULL){      
        return node1;
    }else{
        node2->Binary.left = node1; 
        return node2;
    }
    return NULL;
}




void *Arithmetic_expression_prime(){

    #define check_op(op) (op==PLUS||op==MINUS)


    if(check_op(cur_token)) {

        TOKEN_TYPE type = cur_token;

        #ifdef DEBUG
        RULE(SYMBOL,yytext);
        #endif

        match(type);
        AST_NODE *node1 = term();
        AST_NODE *node2 = Arithmetic_expression_prime();

        if(node2 == NULL){
            return mk_binary_node(token_to_ast(type),NULL,node1);
        }else{

            node2->Binary.left = node1;
            node2 = mk_binary_node(token_to_ast(type),node1,node2);
            return node2;
        }

    }else{
        //expr_prime follow set

        switch(cur_token){
            case EOS:
            case SEMI_COLON:
            case R_PAREN :
            case DOUBLE_EQ:
            case G_THAN:
            case L_THAN:
            case G_THAN_EQ:
            case L_THAN_EQ:
            case NOT_EQ:
            case AND_OP:
            case OR_OP:
            case NOT_OP:
            case OPEN_CURLY:
                return EPSILON;
            default:
                printf("Syntax error at line %d near %s\n",line,prev_token);
                exit(ERROR);
        }    }

    #undef check_op
    #undef find_op_type
    return NULL;
}

void *term(){

    #ifdef DEBUG
    RULE(TERM,"term");
    #endif
    AST_NODE *node1 = factor();
    AST_NODE *node2 = term_prime();
    if(node2 == NULL){
        return node1;
    }else{
        node2->Binary.left = node1;
        return node2;
    }
    return NULL;
}

void *term_prime(){

    #define check_op(op) (op==DIVIDE||op==MULTIPLY||op==MODULO)

    if(check_op(cur_token)){
        TOKEN_TYPE type = cur_token;        
        #ifdef DEBUG
        RULE(SYMBOL,yytext);
        #endif
        match(type);
        AST_NODE *node1 = factor();
        AST_NODE *node2 = term_prime();

        if(node2 == NULL){
            return mk_binary_node(token_to_ast(type),NULL,node1);
        }else{
            node2->Binary.left = node1;
            node2 = mk_binary_node(token_to_ast(type),node1,node2);
            return node2;
        }

    }else{

        //term prime follow set
        switch(cur_token){
            case PLUS :
            case MINUS:
            case DOUBLE_EQ:
            case G_THAN:
            case L_THAN:
            case G_THAN_EQ:
            case L_THAN_EQ:
            case NOT_EQ:
            case AND_OP:
            case OR_OP:
            case NOT_OP:
            case EOS:
            case SEMI_COLON:
            case R_PAREN:
            case OPEN_CURLY:
                return EPSILON;
            default:
                printf("Syntax error at line %d near %s\n",line,prev_token);
                exit(ERROR);
        }
    }

    #undef check_op
    #undef find_op_type
    return NULL;
}

void *factor(){

    #ifdef DEBUG
    RULE(FACTOR,"factor");
    #endif
    long double *num;
    AST_NODE *node;
    TOKEN_TYPE type;
    SYM_DATA *s_tab;
    
    switch(cur_token){
        case FLOAT:{

            num = malloc(sizeof(long double));
            *num = atof(yytext);

            match(FLOAT);
            return mk_float_node(token_to_ast(FLOAT),*num,num); 
        }
        case INTEGER:{

            #ifdef DEBUG
            RULE(NUMBER,yytext);
            #endif

            num = malloc(sizeof(long double));
            *num = atof(yytext);

            match(INTEGER);
            return mk_num_node(token_to_ast(INTEGER),*num,num); 
        
        }
        case L_PAREN:{

            #ifdef DEBUG
            RULE(L_PAREN,yytext);
            #endif

            match(L_PAREN);
            node = expression();

            #ifdef DEBUG
            RULE(R_PAREN,yytext);
            #endif

            match(R_PAREN);
            return node;
        }
        case PLUS:
            match(PLUS);
            node = expression();
            return mk_unary_node(U_PLUS,node);

        case MINUS:
            match(MINUS);
            node = expression();
            return mk_unary_node(U_MIN,node);
        case ID:
            match(ID);   //need to make num node;
            if(cur_token == EQ) return assignment();
            s_tab = sym_fetch(sym_tab,prev_token);
            node = mk_num_node(NO,s_tab->entry->value,&s_tab->entry->value);
            return node;
        case TRUE_EXP:
        case FALSE_EXP:
            #define bool_value(type) (type == TRUE_EXP)? true : false
            type = cur_token;
            match(type);
            return mk_bool_node(token_to_ast(type),bool_value(type));   
            
        case STRING:
            match(STRING);            
            return mk_string_node(STR,strndup(prev_token+1,strlen(prev_token)-2),strlen(prev_token)-2);
        default:
            printf("Syntax error at line %d\n near %s\n",line,prev_token);
            exit(ERROR);

    }
    return NULL;
}



