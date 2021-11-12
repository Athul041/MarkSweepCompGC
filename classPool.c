#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "classPool.h"
#include "MemFunctions.h"
#include "ObjectReference.h"

binClass * getClassPointer(binClass *pool, int classIndex)
{
	return &(pool[classIndex]);
}

void initClassPool(ClassPool *a, size_t initialSize)
{
	a->classPool = malloc(initialSize * sizeof(binClass));
	a->used = 0;
	a->size = initialSize;
}

void freeClassPool(ClassPool *a)
{
	free(a->classPool);
	a->classPool = NULL;
	a->used = a->size = 0;
}

void createClassPoolEntry(ClassPool *pool, int classId)
{
	if (pool->size > classId)
	{
		return;
	}
	else
	{
		pool->size = classId + 1;
		pool->classPool = realloc(pool->classPool, pool->size*sizeof(binClass));
	}
	binClass newClass;
	pool->classPool[classId] = newClass;
	pool->used += 1;
	binClass *cp = &(pool->classPool[classId]);
	printf("\n%" PRIu64 "\n", cp);
}

void writeStaticObjectRefToClass(ClassPool *pool, int classId, int slot, uint64_t objRef)
{
	pushRefToMem(&pool->classPool[classId].staticRefs[8*slot], objRef);
	printf("\nvalue at slot %d : %" PRIu64 "\n", slot, getRefFromMem(&pool->classPool[classId].staticRefs[slot]));
}

void readRefFromClass();

void printClass(ClassPool *pool, int classId)
{
	printf("\nClassId %d",classId);
	printf("\nStaticRefs : ");
	for (int i = 0; i < 16; i++)
	{
		printf("%d\t", pool->classPool[classId].staticRefs[i]);
	}
	
}