#include "Math.h"
#include "LogLinkedList.h"

int gBlocksAllocated;
SSLL* gFreeSegments;
SWCLL* gSegmentCounts;

void init(void)
{
	gBlocksAllocated = 0;
	gFreeSegments = createEmptySSLL();
	gSegmentCounts = createEmptySWCLL();
}

// Returns a pointer to contiguous memory of size at least 'n' bytes. Returns NULL
// if no memory is available or 'n' is zero.
void* mem_alloc(uint n)
{
	if (n == 0) return NULL;

	uint blocksNeeded = ceilingOfQuotient(n, MEM_BLOCK_SIZE);
	
	if (blocksNeeded == 1)
	{
		// Only one block or part of a block necessary
		bool success = 0;
		void* allocated = 0;
		gFreeSegments = takeBytesFromASegment(gFreeSegments, n, &success, &allocated);

		if (success)
		{
			gSegmentCounts = registerMinorAllocation(gSegmentCounts, allocated);
			return allocated;
		}
	}

	// We arrive here if we either need more than 1 block,
	// or we need 1 block but need to allocate a new one.
	
	void* result = mem_block_alloc(blocksNeeded);
	if (result == NULL)
	{
		// Block allocator failed.
		return NULL;
	}

	// If n is not an exact multiple of MEM_BLOCK_SIZE,
	// we need to add a new free segment to the list.
	if (MEM_BLOCK_SIZE * blocksNeeded != n)
	{
		uintptr_t firstByteOfFreeSegment = ((uintptr_t)result) + n;
		uintptr_t firstByteAfterFreeSegment = ((uintptr_t)result) + MEM_BLOCK_SIZE * blocksNeeded;
		Segment freeSeg = { firstByteOfFreeSegment, firstByteAfterFreeSegment };
		gFreeSegments = SSLL_insert(gFreeSegments, freeSeg);

		gSegmentCounts = registerBlockAllocation(gSegmentCounts, result, n);
	}
	return result;
}

// Releases memory allocated by mem_alloc(). Does nothing if 'ptr' is NULL.
void mem_free(void* ptr)
{
	gSegmentCounts = registerDeallocation(gSegmentCounts, ptr);
}


int main() {
	// FIXME: Add test calls here.

	return 0;
}
