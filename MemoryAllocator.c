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

/*
Returns a pointer to contiguous memory of size at least 'n' bytes. Returns NULL
if no memory is available or 'n' is zero.
*/
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
	
	void* result = kernel_mem_block_alloc(blocksNeeded);
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

/*
Releases memory allocated by mem_alloc(). Does nothing if 'ptr' is NULL.
*/
void mem_free(void* ptr)
{
	if (ptr != NULL)
	{
		gSegmentCounts = registerDeallocation(gSegmentCounts, ptr);
	}
}

void* test_alloc_small(void)
{
	uint nBytes = 600;
	void* ptr = mem_alloc(nBytes);
#if MY_DEBUG
	char buff[256];   // Use an array which is large enough 
	snprintf(buff, sizeof(buff), "\n[USER] Allocated %d bytes at", nBytes);
	printPadded(buff, MY_PADDING_WIDTH);
	printf("%d - %d", (uintptr_t)ptr, (uintptr_t)ptr + nBytes);
#endif

	return ptr;
}

void* test_alloc_big(void)
{
	int div = 5, rem = 500;
	uint nBytes = MEM_BLOCK_SIZE * div + rem;
	void* ptr = mem_alloc(nBytes);

#if MY_DEBUG
	char buff[256];   // Use an array which is large enough 
	snprintf(buff, sizeof(buff), "\n[USER] Allocated %d = %d * MEM_BLOCK_SIZE + %d bytes at", nBytes, div, rem);
	printPadded(buff, MY_PADDING_WIDTH);
	printf("%d - %d", (uintptr_t)ptr, (uintptr_t)ptr + nBytes);
#endif

	return ptr;
}

void test1(void)
{
	const int n1 = 10;
	const int n2 = 5;
	const int n3 = 28;

	void* arr[10 + 5 + 28];

	printf("\n\n===>  %d SMALL ALLOCATIONS\n", n1);
	for (int i = 0; i < n1; i++)
	{
		arr[i] = test_alloc_small();
	}
	printf("\n\n===>  %d LARGE ALLOCATIONS\n", n2);
	for (int i = 0; i < n2; i++)
	{
		arr[n1 + i] = test_alloc_big();
	}
	printf("\n\n===>  %d SMALL ALLOCATIONS\n", n3);
	for (int i = 0; i < n3; i++)
	{
		arr[n1 + n2 + i] = test_alloc_small();
	}

	printf("\n\n===>  FIRST %d DEALLOCATIONS\n", n1);
	for (int i = 0; i < n1; i++)
	{
#if MY_DEBUG
		char buff[256];
		snprintf(buff, sizeof(buff), "\n[USER] About to free memory at");
		printPadded(buff, MY_PADDING_WIDTH);
		printf("%d", (uintptr_t)(arr[i]));
#endif
		mem_free(arr[i]);
	}

	printf("\n\n===>  NEXT %d DEALLOCATIONS\n", n2);
	for (int i = 0; i < n2; i++)
	{
#if MY_DEBUG
		char buff[256];
		snprintf(buff, sizeof(buff), "\n[USER] About to free memory at");
		printPadded(buff, MY_PADDING_WIDTH);
		printf("%d", (uintptr_t)(arr[n1 + i]));
#endif
		mem_free(arr[n1 + i]);
	}

	printf("\n\n===>  LAST %d DEALLOCATIONS\n", n3);
	for (int i = 0; i < n3; i++)
	{
#if MY_DEBUG
		char buff[256];
		snprintf(buff, sizeof(buff), "\n[USER] About to free memory at");
		printPadded(buff, MY_PADDING_WIDTH);
		printf("%d", (uintptr_t)(arr[n1 + n2 + i]));
#endif
		mem_free(arr[n1 + n2 + i]);
	}

	printf("\n\nAs you can see, there were 8 kernel allocations and 8 kernel deallocations."
		"\n6 small allocations fit within a block, and 5 small allocations fit in a block after 500 bytes get taken from it."
		"\nThus only the last (38th) small allocation triggers a new kernel allocation; 38 = 5*5 + 2*6 + 1");
}



int main(void)
{
	test1();
	getchar();
	return 0;
}