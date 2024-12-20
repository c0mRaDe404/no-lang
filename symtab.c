#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#define S_VIEW

extern SCOPE_STACK *s_ptr;


S_TABLE *sym_create(){
    S_TABLE *table;
    table = malloc(sizeof(S_TABLE));
    table->sym_table = calloc(SYM_TABLE_S,sizeof(SYM_TABLE*));
    table->counter = 0;
    return table;
}


size_t hash(char *string,size_t m_val){
    int counter = 0;
    char *head = string;
    char *base = string;
    while(*head != '\0'){
        counter += (int)(*head);
        counter = counter%m_val;
        head++;
    }
    counter += (head-base);
    counter = counter%m_val;
    
    return counter;
}

SYM_TABLE *sym_entry(S_TABLE *table,char *id,long double value){
   

    size_t c = hash(id,SYM_TABLE_S);
    if(table->sym_table[c] == NULL){
        table->sym_table[c] = malloc(sizeof(SYM_TABLE));
        table->sym_table[c]->id_name = strdup(id);
        table->sym_table[c]->value = value;
        table->counter++;
        return table->sym_table[c];
    }else if(table->sym_table[c]->next == NULL){
        if(!sym_check(table,id,c)){
            SYM_TABLE *t = malloc(sizeof(SYM_TABLE));
            table->sym_table[c]->next = t;
            t->id_name = id;
            t->value = value;
            t->next = NULL;
            return table->sym_table[c];
        }else{
            table->sym_table[c]->value = value;
            return table->sym_table[c];
        }
    }else{
        SYM_TABLE *t = table->sym_table[c];
        if(!sym_check(table,id,c)){
            while(t->next != NULL){
                t = t->next;
            }
            t->next = malloc(sizeof(SYM_TABLE));
            t->next->id_name = id;
            t->next->value = value;
            t->next->next = NULL;
            return t;
        }else{
            //t->id_name != id  
            while((strcmp(t->id_name,id) != 0)){
                t = t->next;
            }
            t->value = value;
            return t;
        }

    }
}


int sym_check(S_TABLE *s,char *id,int index){
    
    SYM_TABLE *entry = s->sym_table[index];
    if(entry == NULL){
        return 0;
    }else if(!strcmp(entry->id_name,id)){
            return 1;
    }else{
            SYM_TABLE *head = entry;
            while(head->next != NULL){
                if(head->id_name == id) 
                        return 1;
                head = head->next;             
        }
    }
    return 0;
}


SYM_DATA *mk_sym_data(S_TABLE *sym_table,SYM_TABLE *entry){
        SYM_DATA *n = malloc(sizeof(SYM_DATA));
        n->entry = entry;
        n->sym_table = sym_table;
        return n;
}


SYM_DATA *sym_fetch(S_TABLE *s,char *id){
        
        int index = hash(id,SYM_TABLE_S);
        
        SYM_TABLE *entry = s->sym_table[index];

        if(entry == NULL){
            if(s_ptr->top > 0){

                #define ENTRY(s,c,i) s->stack[c]->sym_table[i]
                int counter = s_ptr->top-1;

                while(counter >= 0){
                    if(entry == NULL){
                        entry = ENTRY(s_ptr,counter,index);
                        counter--;
                    }else {
                        return mk_sym_data(s_ptr->stack[counter+1],entry);
                    }
                }

                if(entry != NULL){
                    return mk_sym_data(s_ptr->stack[counter+1],entry);
                }else {
                    printf("line %d : Id '%s' is not defined\n",line,id);
                    exit(0);
                }
            }else{
                printf("line %d : Id '%s' is not defined\n",line,id);
                exit(0);
            }    
        }else if(!strcmp(entry->id_name,id)){
            return mk_sym_data(s,entry);
        }else{
            SYM_TABLE *cur_entry = entry;
            while(cur_entry != NULL){
                if(!strcmp(cur_entry->id_name,id)) 
                     return mk_sym_data(s,cur_entry);
                cur_entry = cur_entry->next;             
            }
            printf("line %d : Id '%s' is not defined\n",line,id);
            exit(0);

        }
}


void sym_view(S_TABLE *s){

    for(int i = 0;i< SYM_TABLE_S;i++){
        SYM_TABLE *entry = s->sym_table[i];
        if(entry == NULL) 
            continue;

        printf("[%i] %s : %Lf \n",i,entry->id_name,entry->value);
        if(entry->next != NULL){
            SYM_TABLE *head = entry->next;
            int count = 1;
            while(head != NULL){
                printf("[%d][%d] %s : %Lf \n",i,count,head->id_name,head->value);
                head = head->next;
                count++;
            }
        }
    }
}



