#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#define S_VIEW

extern SCOPE_STACK *s_ptr;


SYM_TABLE *sym_create(){
    SYM_TABLE *table;
    table = malloc(sizeof(SYM_TABLE));
    table->sym_table = calloc(SYM_TABLE_S,sizeof(SYM_ENTRY*));
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

SYM_ENTRY *sym_update(SYM_TABLE *table,char *id,long double value){ 
    SYM_DATA *record = sym_fetch(table,id);
    if(record != NULL){
        record->entry->value = value;
        return record->entry;
    }else{
        fprintf(stderr,"Id '%s' is not defined\n",id);
        return NULL;
    }
}

SYM_ENTRY *sym_entry(char *id,long double value){
    #define sym_tab(tab) (tab->sym_table) 
    #define add_entry(rec,id,val,n_ptr)\
                      rec = malloc(sizeof(SYM_ENTRY));\
                      rec->id_name = id;\
                      rec->value = value;\
                      rec->next = n_ptr;
        
    SYM_TABLE *table = get_cur_scope(s_ptr);
    size_t c = hash(id,SYM_TABLE_S);
    
    if(sym_tab(table)[c] == NULL){ /* add the entry if the current entry is empty */
        add_entry(sym_tab(table)[c],id,value,NULL);
        table->counter++;
        return sym_tab(table)[c];

    }else {  /* chains the entry if the hash is same */
        
        if(!sym_check(table,id,c)){ /* for entry,if no entry exists abt the id/variable */
            SYM_ENTRY *current_entry = sym_tab(table)[c];
            while(current_entry->next != NULL){
                current_entry = current_entry->next;
            }
            add_entry(current_entry->next,id,value,NULL);
            return sym_tab(table)[c];
        }
    }

    return NULL;
}


int sym_check(SYM_TABLE *s,char *id,int index){
    
    SYM_ENTRY *entry = s->sym_table[index];
    if(entry == NULL){
        return 0;
    }else if(!strcmp(entry->id_name,id)){
            return 1;
    }else{
        SYM_ENTRY *head = entry;
        while(head->next != NULL){
            if(head->id_name == id) 
                    return 1;
            head = head->next;             
        }
    }
    return 0;
}


SYM_DATA *mk_sym_data(SYM_TABLE *sym_table,SYM_ENTRY *entry){
        SYM_DATA *n = malloc(sizeof(SYM_DATA));
        n->entry = entry;
        n->sym_table = sym_table;
        return n;
}


SYM_DATA *sym_fetch(SYM_TABLE *s,char *id){
        #define lookup_parent(parent,entry_no) parent->sym_table[entry_no]
        
        int index = hash(id,SYM_TABLE_S);     
        SYM_TABLE *table = s;
        SYM_ENTRY *entry = s->sym_table[index];

        while(entry == NULL){
            if(table->parent != NULL){
                entry = lookup_parent(table->parent,index);
                table = table->parent;
            }else{
                printf("line %d : Id '%s' is not defined\n",line,id);
                return NULL;
            }
        }
        return mk_sym_data(table,entry);            
}


void sym_view(SCOPE_STACK *s){
    
    for(int i = 0;i< SYM_TABLE_S;i++){
        if(i < s->top ){

            SYM_ENTRY *entry = s->stack[i]->sym_table[i];
            if(entry == NULL) 
                continue;

            printf("[%i] %s : %Lf \n",i,entry->id_name,entry->value);
            if(entry->next != NULL){
                SYM_ENTRY *head = entry->next;
                int count = 1;
                while(head != NULL){
                    printf("[%d][%d] %s : %Lf \n",i,count,head->id_name,head->value);
                    head = head->next;
                    count++;
                }     
            }
        }
    }
}



