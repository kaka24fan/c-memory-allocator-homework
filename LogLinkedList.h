#ifndef SWCLL_H
#define SWCLL_H

#include "SortedSegmentLinkedList.h"

typedef struct segmentWithCount
{
	uintptr_t leftInclusive;
	uintptr_t rightExclusive;
	uint count;
} SegmentWithCount;

bool SWC_equal(SegmentWithCount s1, SegmentWithCount s2)
{
	return s1.leftInclusive == s2.leftInclusive && s1.rightExclusive == s2.rightExclusive;
}

bool SWC_contains(SegmentWithCount s, uintptr_t address)
{
	return s.leftInclusive <= address && address < s.rightExclusive;
}

typedef struct SWCLinkedList SWCLL;
struct SWCLinkedList
{
	SegmentWithCount head;
	SWCLL* tail;
};

SWCLL* createEmptySWCLL()
{
	return NULL;
}

SWCLL* SWCLL_insert(SWCLL* l, SegmentWithCount s)
{
	SWCLL* newCell = (SWCLL*)malloc(sizeof(SWCLL));
	newCell->head = s;
	newCell->tail = l;
	return newCell;
}

/*
Can only be called if the delete is guaranteed to succeed.
*/
SWCLL* SWCLL_remove(SWCLL* l, SegmentWithCount s)
{
	assert(l != NULL);
	// l is not empty

	if (SWC_equal(l->head, s))
	{
		SWCLL* result = l->tail;
		free(l);
		return result;
	}

	SWCLL* result = l;
	SWCLL* prev = NULL;
	while (l->tail != NULL && !SWC_equal(l->head, s))
	{
		prev = l;
		l = l->tail;
	}

	assert(SWC_equal(l->head, s));
	{
		prev->tail = l->tail;
		free(l);
		return result;
	}
}

SWCLL* registerBlockAllocation(SWCLL* l, void* allocated, uint nBlocks)
{
	SegmentWithCount s = { (uintptr_t)allocated, (uintptr_t)allocated + nBlocks * MEM_BLOCK_SIZE, 1 };
	return SWCLL_insert(l, s);
}

SWCLL* registerMinorAllocation(SWCLL* l, void* _allocated)
{
	assert(l != NULL);
	// l is not empty

	uintptr_t allocated = (uintptr_t) _allocated;

	SWCLL* result = l;
	while (l->tail != NULL && !SWC_contains(l->head, allocated))
	{
		l = l->tail;
	}

	assert(SWC_contains(l->head, allocated));
	{
		l->head.count += 1;
		return result;
	}
}

SWCLL* registerDeallocation(SWCLL* l, void* _deallocated)
{
	assert(l != NULL);
	// l is not empty

	uintptr_t deallocated = (uintptr_t) _deallocated;

	SWCLL* result = l;
	while (l->tail != NULL && !SWC_contains(l->head, deallocated))
	{
		l = l->tail;
	}

	assert(SWC_contains(l->head, deallocated));
	{
		l->head.count -= 1;

		if (l->head.count == 0)
		{
			mem_block_free((void*) l->head.leftInclusive);
			return SWCLL_remove(result, l->head);
		}
		else
		{
			return result;
		}
	}
}

#endif // SWCLL_H