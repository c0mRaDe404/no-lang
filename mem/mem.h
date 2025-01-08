#ifndef MEMORY_H
#define MEMORY_H
#include <stdbool.h>
#include <stdlib.h>

#define BLOCK_OVERFLOW(data_size,block_size) ((data_size > block_size)? 1:0)
#define POOL_SIZE 1024
#define BLOCK_SIZE 16




/* should be dynamic */
/* expandable and shrinkable */
/* generic for all data types */


typedef struct{
    void *pool;
    size_t pool_size;
    size_t pool_used;
    struct MEMORY_BLOCK **block;
    struct MEMORY_BLOCK *free_block;
    struct FREE_LIST *free_block_list;
    size_t free_block_pos;
    size_t total_blocks;
    size_t block_size;
}MEMORY_POOL;

typedef struct FREE_LIST{
    struct MEMORY_BLOCK *block;
    bool has_entry;
    struct FREE_LIST *next_block;
}FREE_LIST;

typedef struct MEMORY_BLOCK{
    size_t block_id;
    void *block;
    bool is_empty;
    size_t block_size;
    size_t block_used;
    struct MEMORY_BLOCK *next_block;
}MEMORY_BLOCK;


void *init_pool(size_t,size_t);
void *mem_alloc(MEMORY_POOL*,size_t);
void *re_alloc(MEMORY_POOL*);
void reset_pool(MEMORY_POOL*);
void *free_block(MEMORY_POOL*,void *);
void *alloc_from_list(MEMORY_POOL*,size_t);
void block_view(MEMORY_POOL*);
size_t get_block_pos(MEMORY_POOL*,void *);
size_t align_size(size_t, size_t);


#endif
