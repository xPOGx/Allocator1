#include <stdio.h>
#include "allocator.h"
#include "dump.h"

void mem_dump() {
	printf("========= Memory Dump =========\n");
	printf("Heap size: \t");
	printf("%d\n", mem_heap.size);
	printf("Blocks count: \t");
	printf("%d\n", mem_heap.blocks_count);
	printf("Header Size: \t");
	printf("17\n");
	printf("Blocks info:\n");
	mem_block_header_t *block = mem_blocks.start;
	


	for (int i = 1; i <= mem_heap.blocks_count; ++i) {
		printf("    %d) Free: ", i);
		printf("%s", block->is_free ? "true" : "false");
		printf(", Size: ");
		printf("%d", block->size);
		printf(", Address: ");
		printf("%d", block->addr - mem_heap.bytes);
		printf("\n");
		block = block->next;
	}

	printf("======= Memory Dump End =======\n");
	printf("\n\n");

};
