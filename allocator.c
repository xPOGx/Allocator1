#include <stdio.h>
#include <string.h>
#include "allocator.h"
#include <Windows.h>


#define FROM_START 1
DWORD   allocType = MEM_COMMIT;
DWORD   allocProtect = PAGE_READWRITE;

unsigned char * protectedAlloc(size_t size){
	LPVOID pMem = VirtualAlloc(NULL, size, allocType, allocProtect);
	if (pMem != NULL) {
		VirtualLock(pMem, size);
		return pMem;
	}
	else return NULL;
}
void freeAndUnProtected(unsigned char*addr, size_t size){
	SecureZeroMemory(addr, size);
	VirtualUnlock(addr, size);
	VirtualFree(addr, 0, MEM_RELEASE);
}

typedef unsigned char byte;

static byte *memory;
static size_t memory_size;


bool checkAddressRange(void *_object)
{
	uintptr_t object = (uintptr_t)_object;
	if (object >= (uintptr_t)memory && object < ((uintptr_t)memory + (uintptr_t)memory_size)) {
		return true;
	}
	else {
		fprintf(stderr, "checkAddressRange: It's Invalid Address, exit\n");
		return false;
	}
}

int mem_init(size_t size) {
	if (mem_heap.bytes != NULL) { 
		return 0;
	}

	if (size < sizeof(mem_block_header_t)) {
		size = DEFAULT_MEM_SIZE;
	}
	mem_heap.bytes = malloc(size);
	memory = mem_heap.bytes;
	memory_size = size;

	if (mem_heap.bytes == NULL) {
		return 0;
	}

	mem_heap.size = size;

	mem_block_header_t *block =
		(mem_block_header_t *)malloc(sizeof(mem_block_header_t));

	block->is_free = 1;
	block->addr = mem_heap.bytes;
	block->size = size;
	block->next = NULL;
	block->prev = NULL;
	mem_heap.blocks_count = 1;

	mem_blocks.order = FROM_START;
	mem_blocks.start = block;
	mem_blocks.end = block;

	return 1;

}
bool isExist(mem_block_header_t* block) {
	return (block != NULL) ? true : false;
}

void mem_drop() {
	if (mem_heap.bytes == NULL) {
		return;
	}


		mem_block_header_t *block = mem_blocks.start;
	do {
	if (mem_heap.blocks_count == 1) break;
		free_block(block);
		block = isExist(block)? block->next: mem_blocks.start;

	} while (block != NULL);

	
	
	(mem_heap.bytes);
	mem_heap.bytes = NULL;
};

unsigned char *mem_alloc(size_t size) {
	size = align(size);

	mem_block_header_t *block = find_free_block(size);
	if (block == NULL) {
		return NULL;
	}
	if (!checkAddressRange(block->addr)) {
		getchar();
		exit(EXIT_FAILURE);
	};

	split_mem_block(block, size);

	block->is_free = 0;
	return block->addr;
};

unsigned char *mem_realloc(unsigned char *addr, size_t size) {
	if (addr == NULL) {
		return mem_alloc(size);
	}

	if (size == 0) {
		mem_free(addr);
		return NULL;
	}

	mem_block_header_t *block = find_block(addr);
	if (!checkAddressRange(block->addr)) return NULL;

	return get_realloc_addr(block, size);
};

void mem_free(unsigned char *addr) {
	mem_block_header_t *block = find_block(addr);

	if (block == NULL) {
		return;
	}

	free_block(block);
};

size_t align(size_t size) {
	if (size % 4 == 0) {
		return size;
	}
	else {
		return size + 4 - size % 4;
	}
};

mem_block_header_t *find_free_block(size_t size) {
	mem_block_header_t *block;

	if (mem_blocks.order == FROM_START) {
		block = mem_blocks.start;
		while (!block->is_free || block->size < size) {
			if (block->next == NULL) {
				return NULL;
			}
			block = block->next;
		}
	}
	else {
		block = mem_blocks.end;
		while (!block->is_free || block->size < size) {
			if (block->prev == NULL) {
				return NULL;
			}
			block = block->prev;
		}
	}

	mem_blocks.order = !mem_blocks.order;

	return block;
};

mem_block_header_t *find_block(unsigned char *addr) {
	mem_block_header_t *block = mem_blocks.start;

	while (block->addr != addr) {
		if (block->next == NULL) {
			return NULL;
		}
		block = block->next;
	}

	return block;
};

unsigned char *get_realloc_addr(mem_block_header_t *block, size_t size) {
	size = align(size);

	unsigned char *old_addr = block->addr;
	size_t old_size = block->size;
	size_t delta = size - old_size;

	if (old_size == size) {
		return block->addr;
	}

	if (old_size > size) {
		split_mem_block(block, size);
		mem_free(block->next);
		return block->addr;
	}

	if (

		block->next &&
		block->next->is_free &&
		block->next->size >= delta
		) {
		split_mem_block(block->next, delta);
		merge_mem_blocks(block, block->next);
		return block->addr;
	}

	if (
		block->prev &&
		block->prev->is_free &&
		block->prev->size >= delta
		) {
		block->addr -= delta;
		block->size += delta;
		block->prev->size -= delta;
		return block->prev->addr;
	}

	mem_block_header_t *free_block = find_free_block(size);

	if (free_block == NULL) {
		return NULL;
	}

	free_block->is_free = 0;
	split_mem_block(free_block, size);
	memcpy(free_block->addr, old_addr, old_size);

	mem_free(old_addr);

	return free_block->addr;
};

void split_mem_block(mem_block_header_t *block, size_t size) {
	if ((block->size - (size + 4)) < sizeof(mem_block_header_t)) {
		return 1;
	}

	if (block->size >= size + 4) {
		mem_heap.blocks_count += 1;

		mem_block_header_t *next_block =
			(mem_block_header_t *)malloc(sizeof(mem_block_header_t));

		next_block->is_free = 1;
		next_block->size = align(block->size - size);
		next_block->addr = (unsigned char *)((size_t)block->addr + size);
		next_block->next = block->next;
		next_block->prev = block;

		block->size = size;
		block->next = next_block;

		if (next_block->next == NULL) {
			mem_blocks.end = next_block;
		}
		else if (next_block->next->is_free) {
			merge_mem_blocks(next_block, next_block->next);
		}
		else {
			next_block->next->prev = next_block;
		}
	}
};

void merge_mem_blocks(mem_block_header_t *block1, mem_block_header_t *block2) {
	block1->next = block2->next;
	block1->size += block2->size;

	if (block2->next != NULL) {
		block2->next->prev = block1;
	}
	else {
		mem_blocks.end = block1;
	}

	mem_heap.blocks_count -= 1;
	freeAndUnProtected(&block2, sizeof(mem_block_header_t));
	block2 = NULL;
};

void free_block(mem_block_header_t *block) {
		if (block->next && block->next->is_free) {
			merge_mem_blocks(block, block->next);
		}

		if (block->prev && block->prev->is_free) {
			merge_mem_blocks(block->prev, block);
		}
		else {
			block->is_free = 1;
		}
};
