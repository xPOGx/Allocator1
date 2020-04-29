#include <stdio.h>
#include "allocator.h"
#include "dump.h"
int main() {
	{
		printf("TEST 1: memory initialization\n");
		mem_init(0);
		mem_dump();
		mem_drop();
	}

	{
		printf("TEST 2: memory allocation\n");
		mem_init(1024);
		mem_alloc(512);
		mem_dump();
		mem_drop();
	}

	{
		printf("TEST 3: memory allocation (return NULL)\n");
		mem_init(1024);
		mem_alloc(512);
		mem_alloc(400);
		mem_dump();
		mem_drop();
	}

	{
		printf("test 4: memory allocation (align size)\n");
		mem_init(1024);
		mem_alloc(1);
		mem_dump();
		mem_drop();
	}

	{
		printf("test 5: memory freeing (no merge)\n");
		mem_init(1024);

		unsigned char *p = mem_alloc(256);
		mem_alloc(256);

		printf("before freeing\n");
		mem_dump();
		mem_free(p);

		printf("after freeing\n");
		mem_dump();
		mem_drop();
	}

	{
		printf("test 6: memory freeing (merge with previous free)\n");
		mem_init(1024);

		unsigned char *p1 = mem_alloc(512);
		unsigned char *p2 = mem_alloc(256);
		mem_alloc(256);
		mem_free(p1);

		printf("before freeing\n");
		mem_dump();

		mem_free(p2);

		printf("after freeing\n");
		mem_dump();
		mem_drop();
	}

	{
		printf("test 7: memory freeing (merge with next free)\n");
		mem_init(1024);

		mem_alloc(512);
		unsigned char *p = mem_alloc(256);

		printf("before freeing\n");
		mem_dump();

		mem_free(p);

		printf("after freeing\n");
		mem_dump();
		mem_drop();
	}

	{
		printf(
			"test 8: memory freeing (merge with both next and previous free)\n"
		);
		mem_init(1024);

		unsigned char *p1 = mem_alloc(512);
		unsigned char *p2 = mem_alloc(256);
		mem_free(p1);

		printf("before freeing\n");
		mem_dump();

		mem_free(p2);

		printf("after freeing\n");
		mem_dump();
		mem_drop();

	}

	{
		printf("test 9: memory reallocation (increase size)\n");
		mem_init(1024);

		unsigned char *p = mem_alloc(256);
		mem_alloc(256);

		printf("before reallocation\n");
		mem_dump();

		mem_realloc(p, 400);

		printf("after reallocation\n");
		mem_dump();
		mem_drop();

	}

	{
		printf("test 10: memory reallocation (decrease size)\n");
		mem_init(1024);

		unsigned char *p = mem_alloc(256);
		mem_alloc(256);

		printf("before reallocation\n");
		mem_dump();

		mem_realloc(p, 16);

		printf("after reallocation\n");
		mem_dump();
		mem_drop();

	}

	{
		printf("test 11: memory reallocation (borrow from next block)\n");
		mem_init(1024);

		unsigned char *p = mem_alloc(256);

		printf("before reallocation\n");
		mem_dump();

		mem_realloc(p, 300);

		printf("after reallocation\n");
		mem_dump();
		mem_drop();
	}

	{
		printf("test 12: memory reallocation (borrow from previous block)\n");
		mem_init(1024);

		unsigned char *p1 = mem_alloc(256);
		unsigned char *p2 = mem_alloc(256);
		mem_alloc(512);

		mem_free(p1);

		printf("before reallocation\n");
		mem_dump();

		mem_realloc(p2, 300);

		printf("after reallocation\n");
		mem_dump();

		mem_drop();

	}

	getchar();
	return 0;

}
