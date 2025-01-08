#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mem.h"

void *init_pool(size_t pool_size,size_t block_size){
    
    MEMORY_POOL *mp = malloc(sizeof(MEMORY_POOL));
    mp->pool = (void*) malloc(pool_size); /* raw bytes */
    mp->free_block_list = malloc(sizeof(FREE_LIST));
    mp->pool_size = pool_size;
    mp->pool_used = 0;
    mp->block_size = block_size;
    mp->total_blocks = pool_size / block_size;
    mp->free_block_pos = 0;
    mp->free_block_list->block = NULL;
    mp->free_block_list->next_block = NULL;
    mp->free_block_list->has_entry = false;
    mp->block = malloc(mp->total_blocks*sizeof(MEMORY_BLOCK*));
    for(int i = 0;i < mp->total_blocks;i++){
        mp->block[i] = malloc(sizeof(MEMORY_BLOCK));
        mp->block[i]->block = (mp->pool+(block_size*i));
        mp->block[i]->block_id = i+1;
        mp->block[i]->is_empty = true;
        mp->block[i]->block_size = block_size;
        mp->block[i]->block_used = 0;
        if((i > 0) && (i < mp->total_blocks-1))
           mp->block[i-1]->next_block = mp->block[i];
    }
     
    mp->free_block = mp->block[mp->free_block_pos];
    return mp;
}


void *mem_alloc(MEMORY_POOL *mp,size_t n){
    
    MEMORY_BLOCK **blocks = mp->block;
    MEMORY_BLOCK *new_block = mp->free_block;
    
 
    if(new_block == NULL) {
        mp = re_alloc(mp);
        blocks = mp->block;
        new_block = mp->free_block;
    }

    if(BLOCK_OVERFLOW(n,mp->block_size)){
        MEMORY_BLOCK *temp = new_block;
        size_t req_blocks = align_size(n,mp->block_size) / mp->block_size;

        for(int i = 0;i < req_blocks;i++){
            temp->block_used = n / req_blocks;
            mp->pool_used += temp->block_used;
            temp->is_empty = false;
            temp = temp->next_block;
        }
        mp->free_block_pos += req_blocks;
        mp->free_block = *(blocks+mp->free_block_pos);
        return new_block->block;
    }else{
        if(mp->free_block_list->has_entry){
            return alloc_from_list(mp,n);
        }else{
            new_block->block_used += n;
            new_block->is_empty = false;
            mp->free_block_pos += 1;
            mp->free_block = *(blocks+mp->free_block_pos);
            mp->pool_used += n;
            return new_block->block;
        }
    }
}

void *re_alloc(MEMORY_POOL *mp){
    size_t mp_size = mp->pool_size*2;
    size_t new_total_blocks = mp_size / mp->block_size;
    mp->block = realloc(mp->block,sizeof(MEMORY_BLOCK*)*new_total_blocks);
    mp->pool = realloc(mp->pool,mp_size);
    mp->pool_size = mp_size;

    for(int i = mp->total_blocks;i<new_total_blocks;i++){
        mp->block[i] = malloc(sizeof(MEMORY_BLOCK));
        mp->block[i]->block = (mp->pool+(mp->block_size*i));
        mp->block[i]->block_id = i+1;
        mp->block[i]->is_empty = true;
        mp->block[i]->block_size = mp->block_size;
        mp->block[i]->block_used = 0;
        if((i > 0) && (i < new_total_blocks-1))
           mp->block[i-1]->next_block = mp->block[i];
    }
    mp->total_blocks = new_total_blocks;
    mp->free_block = mp->block[mp->free_block_pos];
    return mp;
}
void reset_pool(MEMORY_POOL *mp){
    for(int i = 0;i < mp->total_blocks;i++){
        mp->block[i]->is_empty = true;
        mp->block[i]->block_used = 0;
    }
    mp->pool_used = 0;
    mp->free_block_pos = 0;
    mp->free_block = mp->block[mp->free_block_pos];
    return;
}

void *alloc_from_list(MEMORY_POOL *mp,size_t n){
    FREE_LIST *head = mp->free_block_list;
    while(head != NULL){
        if(head->block->is_empty){
            head->block->is_empty = false;
            head->block->block_used = n;
            return head->block->block;
        }
        else{
            head = head->next_block;
        }
    }
    return NULL;
}

void *free_block(MEMORY_POOL *mp,void *ptr){
    size_t block_pos;
    FREE_LIST *head;
    block_pos = get_block_pos(mp,ptr);
    mp->block[block_pos]->is_empty = true;
    mp->block[block_pos]->block_used = 0;
    mp->free_block_list->block = mp->block[block_pos];
    head = mp->free_block_list;
    
    while(head->next_block != NULL)
        head = head->next_block;
    
    if(head!= NULL){
        head->next_block = malloc(sizeof(FREE_LIST));
        head->has_entry = true;
    }
    return NULL;
}


void block_view(MEMORY_POOL *mp){
    size_t id;
    size_t used;
    bool empty;
    for(int i=0;i<mp->total_blocks;i++){
        id = mp->block[i]->block_id;
        used = mp->block[i]->block_used;
        empty = mp->block[i]->is_empty;
        fprintf(stdout,"block id: %zu | block usage : %zu | is empty :  %d\n",id,used,empty);
        fflush(stdout);
    }
    fprintf(stdout,"Total memory used from the pool : %zu bytes\n",mp->pool_used);
}




size_t align_size(size_t size, size_t alignment) {
    return (size + (size % alignment));
}

size_t get_block_pos(MEMORY_POOL *mp,void *ptr){
    #define base_ptr(mp) (mp->block[0]->block)
    return ((ptr - base_ptr(mp)) / mp->block_size);
}


