#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#define S_VIEW

extern int line;


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

void sym_entry(S_TABLE *table,char *id,long double value){
   

    size_t c = hash(id,SYM_TABLE_S);
    if(table->sym_table[c] == NULL){
        table->sym_table[c] = malloc(sizeof(SYM_TABLE));
        table->sym_table[c]->id_name = strdup(id);
        table->sym_table[c]->value = value;
        table->counter++;
    }else if(table->sym_table[c]->next == NULL){
        if(!sym_check(table,id)){
            SYM_TABLE *t = malloc(sizeof(SYM_TABLE));
            table->sym_table[c]->next = t;
            t->id_name = id;
            t->value = value;
            t->next = NULL;
        }else{
            table->sym_table[c]->value = value;
        }
    }else{
        SYM_TABLE *t = table->sym_table[c];
        if(!sym_check(table,id)){
            while(t->next != NULL){
                t = t->next;
            }
            t->next = malloc(sizeof(SYM_TABLE));
            t->next->id_name = id;
            t->next->value = value;
            t->next->next = NULL;
        }else{
            while(t->next->id_name != id){
                t = t->next;
            }
            t->value = value;
        }

    }
}


int sym_check(S_TABLE *s,char *id){
    
    int index = hash(id,SYM_TABLE_S);
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

SYM_TABLE *sym_fetch(S_TABLE *s,char *id){
        
        int index = hash(id,SYM_TABLE_S);
        SYM_TABLE *entry = s->sym_table[index];

        if(entry == NULL){
            printf("line %d : Id '%s' is not defined\n",line,id);
            exit(0);
        }else if(!strcmp(entry->id_name,id)){
            return entry;
        }else{
            SYM_TABLE *head = entry;
            while(head->next != NULL){
                if(head->id_name == id) 
                     return head;
                head = head->next;             
            }
            printf("line %d : Id '%s' is not defined\n",line,id);
            exit(0);
        }
}

#ifdef S_VIEW
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

#endif
