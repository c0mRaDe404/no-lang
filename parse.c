#include <stdio.h>
#include "defs.h"
#include <stdlib.h>
#include <string.h>

extern size_t loop_counter;

#define DEBUG
#undef DEBUG

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
        case TRUE_EXP:
            return TRUE;
        case FALSE_EXP:
            return FALSE;
        case BREAK:
            return BRK;
        case CONTINUE:
            return CONT;
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


void *block(){

    AST_NODE *node = NULL;
    if(cur_token == OPEN_CURLY){
        match(OPEN_CURLY);
        if(cur_token != CLOSE_CURLY) node = program();
        match(CLOSE_CURLY);
        return node;
    }else {
        return program();
    } 
    return NULL;
}

void *program(){
    AST_NODE *node1,*node2;
#define END(token) (END_OF_FILE(token) || END_OF_STMT(token))
    if(END(cur_token)) return NULL;
    node1 = statement();
    node2 = program();
    return mk_binary_node(STMT,node1,node2);
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
            if(cur_token != SEMI_COLON) 
                n1 = declaration();
            match(SEMI_COLON);
            if(cur_token != SEMI_COLON) 
                n2 = expression();
            match(SEMI_COLON);
            if(cur_token != R_PAREN)    
                n3 = declaration();
            match(R_PAREN);
            n4 = block();
            loop_counter--;
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
        default:
            n1 = expression();
            match(SEMI_COLON);
            return n1;
    }

    //node = expression();
    //match(SEMI_COLON);
    //return node;
}


void *declaration(){

    match(LET);
    AST_NODE *node;
    char *id = strndup(yytext,yyleng);
    sym_entry(sym_tab,id,0);
#ifdef DEBUG
    RULE(ASSIGN,"assign");
    RULE(ID,yytext);
#endif
    match(ID);
    match(EQ);
    return mk_assign_node(ASSIGN,id,expression());
}


void *assignment(){

    AST_NODE *node;
    char *id = strndup(yytext,yyleng);
    if(sym_check(sym_tab,id)){
        match(ID);
        match(EQ);
        return mk_assign_node(ASSIGN,id,expression());
    }else return NULL;
}


void *expression(){
#ifdef DEBUG
    RULE(EXPRESSION,"exp");
#endif

    AST_NODE *node1 = term();
    AST_NODE *node2 = exp_prime();
    TOKEN_TYPE type;

    if(node2 == NULL){      
        return node1;
    }else{
        node2->node.Binary.left = node1; 
        return node2;
    }
    return NULL;
}

void *exp_prime(){

#define check_op(op) (op==PLUS||op==MINUS||op==OR_OP)


    if(check_op(cur_token)) {

        TOKEN_TYPE type = cur_token;

#ifdef DEBUG
        RULE(SYMBOL,yytext);
#endif

        match(type);
        AST_NODE *node1 = term();
        AST_NODE *node2 = exp_prime();

        if(node2 == NULL){
            return mk_binary_node(token_to_ast(type),NULL,node1);
        }else{

            node2->node.Binary.left = node1;
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
            case OPEN_CURLY:
                return epsilon();
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
        node2->node.Binary.left = node1;
        return node2;
    }
    return NULL;
}

void *term_prime(){

    #define check_op(op) (op==DIVIDE||op==MULTIPLY||op==MODULO || op == AND_OP || RELATIONAL_OP(op))

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
            node2->node.Binary.left = node1;
            node2 = mk_binary_node(token_to_ast(type),node1,node2);
            return node2;
        }

    }else{

        //term prime follow set
        switch(cur_token){
            case PLUS :
            case MINUS:
            case OR_OP:
            case EOS:
            case SEMI_COLON:
            case R_PAREN:
            case OPEN_CURLY:
                return epsilon();
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
    long double num;
    AST_NODE *node;
    TOKEN_TYPE type;
    SYM_TABLE *table;
    switch(cur_token){
        case NUM:

        #ifdef DEBUG
        RULE(NUMBER,yytext);
        #endif
        num = atof(yytext);
        match(NUM);
        return mk_num_node(NO,num); 
        case L_PAREN:

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

        case PLUS:
        case MINUS:
            type = (cur_token == PLUS) ? PLUS:MINUS;
            match(type);
            node = factor();
            return mk_unary_node((type == PLUS)? U_PLUS:U_MIN,node);
        case ID:
            match(ID);   //need to make num node;
            table = sym_fetch(sym_tab,prev_token);
            node = mk_num_node(NO,table->value);
            node->node.Unary.ref = &table->value; 
            return node;
        case NOT_OP:
            type = cur_token;
            match(type);
            node = expression();
            return mk_unary_node(NOT,node);
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

void *epsilon(){
    return NULL;
}


