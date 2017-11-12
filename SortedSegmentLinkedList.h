#ifndef SSLL_H
#define SSLL_H

#include "Math.h"

typedef struct segment Segment;
struct segment
{
	uintptr_t leftInclusive;
	uintptr_t rightExclusive;
};

uintptr_t length(Segment s)
{
	return s.rightExclusive - s.leftInclusive;
}

bool longerThan(Segment s1, Segment s2)
{
	return length(s1) > length(s2);
}

bool equal(Segment s1, Segment s2)
{
	return s1.leftInclusive == s2.leftInclusive && s1.rightExclusive == s2.rightExclusive;
}

typedef struct sortedSegmentLinkedList SSLL;
struct sortedSegmentLinkedList
{
	Segment head;
	SSLL* tail;
};

SSLL* createEmptySSLL()
{
	return NULL;
}

SSLL* SSLL_insert(SSLL* l, Segment s)
{
	SSLL* newCell = (SSLL*)malloc(sizeof(SSLL));
	newCell->head = s;

	// l is empty list
	if (l == NULL)
	{
		newCell->tail = NULL;
		return newCell;
	}
	// l is not empty
	
	// if s is longer than anything in l:
	if (longerThan(s, l->head))
	{
		newCell->tail = l;
		return newCell;
	}

	// we will eventually return 'result'
	SSLL* result = l;

	// walk through the lsit until the end or we find sth shorter than s
	SSLL* prev = NULL;
	while (l->tail != NULL && !longerThan(s, l->head))
	{
		prev = l;
		l = l->tail;
	}

	// if l is the first (longest) element shorter than s
	if (longerThan(s, l->head))
	{
		prev->tail = newCell;
		newCell->tail = l;
		return result;
	}

	// if s is shorter than anything on the list
	else
	{
		assert(l->tail == NULL);

		l->tail = newCell;
		newCell->tail = NULL;
		return result;
	}
}

/*
	Can only be called if the delete is guaranteed to succeed.
*/
SSLL* SSLL_remove(SSLL* l, Segment s)
{
	assert(l != NULL);
	// l is not empty

	if (equal(l->head, s))
	{
		SSLL* result = l->tail;
		free(l);
		return result;
	}

	SSLL* result = l;
	SSLL* prev = NULL;
	while (l->tail != NULL && !equal(l->head, s))
	{
		prev = l;
		l = l->tail;
	}

	assert(equal(l->head, s));
	{
		prev->tail = l->tail;
		free(l);
		return result;
	}
}

SSLL* takeBytesFromASegment(SSLL* l, uintptr_t nBytes, bool* success, void** allocated)
{
	if (l == NULL || nBytes > length(l->head))
	{
		*success = false;
		return l;
	}

	SSLL* result = l;

	SSLL* prev = NULL;
	while (l->tail != NULL && length(l->head) > nBytes)
	{
		prev = l;
		l = l->tail;
	}

	Segment s;
	if (length(l->head) > nBytes)
	{
		s = l->head;
	}
	else
	{
		s = prev->head;
	}

	Segment remains = { s.leftInclusive + nBytes, s.rightExclusive };

	result = SSLL_remove(result, s);

	if (length(remains) > 0)
	{
		result = SSLL_insert(result, remains);
	}

	*success = true;
	*allocated = (void*) s.leftInclusive;
	return result;
}

#endif // SSLL_H