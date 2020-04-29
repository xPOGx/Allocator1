#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdlib.h>

#include <stdbool.h>
#define DEFAULT_MEM_SIZE 1024 // 1 MB

/*
 * Memory heap structure
 */
struct {
	unsigned char *bytes;
	size_t size;
	size_t blocks_count;
} mem_heap;

/*
 * Memory block header type
 */
typedef struct mem_block_header_t mem_block_header_t;
struct mem_block_header_t {
	bool is_free;
	size_t size;
	unsigned char *addr;
	mem_block_header_t *next;
	mem_block_header_t *prev;
};

/*
 * List of memory block headers
 */
struct {
	char order;
	mem_block_header_t *start;
	mem_block_header_t *end;
} mem_blocks;

/*
 * Initialize heap
 */
int mem_init(size_t);

/*
 * Free heap
 */
void mem_drop();

/*
 * Allocate memory
 */
unsigned char *mem_alloc(size_t);

/*
 * Reallocate memory
 */
unsigned char *mem_realloc(unsigned char *, size_t);

/*
 * Free memory
 */
void mem_free(unsigned char *);

bool isExist(unsigned char *block_addr);

/*
 * Align given size by 4
 */
size_t align(size_t);

/*
 * Find free block with corresponding size
 */
mem_block_header_t *find_free_block(size_t);

/*
 * Find block, responsible for with given address
 */
mem_block_header_t *find_block(unsigned char *);

/*
 * Get address for reallocation given address with given size
 */
unsigned char *get_realloc_addr(mem_block_header_t *, size_t);

/*
 * Split memory block in two parts, where one will have given size
 */
void split_mem_block(mem_block_header_t *, size_t);

/*
 * Merge memory blocks with given headers. Second one will be merged in first
 */
void merge_mem_blocks(mem_block_header_t *, mem_block_header_t *);

/*
 * Mark block header as free and merge with neighbours whether is able to
 */
void free_block(mem_block_header_t *);

#endif