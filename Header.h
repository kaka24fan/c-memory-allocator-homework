#ifndef HEADER_H
#define HEADER_H

#include "assert.h"
#include "memory.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

// Size of a block, as used by mem_block_alloc().
#define MEM_BLOCK_SIZE 4096

typedef size_t uint;

// Set this to true in a test to simulate out of memory.
bool memory_exhausted = false;

// Returns a pointer to contiguous memory of size at least 'n' blocks.
// 'n' must be greater than 0. Returns NULL if no memory is available.
void* mem_block_alloc(uint n)
{
	assert(n > 0);
	if (memory_exhausted) 
	{
		return NULL;
	}
	return malloc(n * MEM_BLOCK_SIZE);
}

// Releases memory allocated by mem_block_alloc(). 'ptr' MUST NOT be NULL.
void mem_block_free(void* ptr) 
{
	assert(ptr != NULL);
	free(ptr);
}

#endif // HEADER_H