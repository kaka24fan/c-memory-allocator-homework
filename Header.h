#ifndef HEADER_H
#define HEADER_H

#include "assert.h"
#include "memory.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdlib.h"

#include "Printing.h"

#define MY_DEBUG 1
#define MY_PADDING_WIDTH 68

// Size of a block, as used by kernel_mem_block_alloc().
#define MEM_BLOCK_SIZE 4096

typedef size_t uint;

// Set this to true in a test to simulate out of memory.
bool gPretendMemoryIsExhausted = false;

// Returns a pointer to contiguous memory of size at least 'n' blocks.
// 'n' must be greater than 0. Returns NULL if no memory is available.
void* kernel_mem_block_alloc(uint n)
{
	assert(n > 0);
	if (gPretendMemoryIsExhausted)
	{
		return NULL;
	}
	void* result = malloc(n * MEM_BLOCK_SIZE);

#if MY_DEBUG
	char buff[256];   // Use an array which is large enough 
	snprintf(buff, sizeof(buff), "\n[KERNEL] Allocated %d blocks at", n);
	printPadded(buff, MY_PADDING_WIDTH);
	printf("%d - %d", (uintptr_t)result, (uintptr_t)result + n * MEM_BLOCK_SIZE);
#endif

	return result;
}

// Releases memory allocated by kernel_mem_block_alloc(). 'ptr' MUST NOT be NULL.
void kernel_mem_block_free(void* ptr) 
{
	assert(ptr != NULL);

#if MY_DEBUG
	char buff[256];   // Use an array which is large enough 
	snprintf(buff, sizeof(buff), "\n[KERNEL] Deallocated blocks at");
	printPadded(buff, MY_PADDING_WIDTH);
	printf("%d", (uintptr_t)ptr);
#endif

	free(ptr);
}

#endif // HEADER_H