#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void createClassPoolEntry(ClassPool *pool, int classId, int refSlots)
{
	if (pool->size <= pool->used)
	{
		pool->size = pool->size + 1;
		pool->classPool = realloc(pool->classPool, pool->size*sizeof(binClass));
	}
	if (pool->size <= classId)
	{
		pool->size = classId + 1;
		pool->classPool = realloc(pool->classPool, pool->size*sizeof(binClass));
	}
	if(pool->classPool[classId].init == 1)
	{
		return;
	}
	pool->classPool[classId].referenceSlots = refSlots;
	memset(&(pool->classPool[classId].staticRefs), 0, 128);
	pool->classPool[classId].init = 1;
	pool->used += 1; 
}

void writeStaticObjectRefToClass(ClassPool *pool, int classId, int slot, uint64_t objRef)
{
	pushRefToMem(&pool->classPool[classId].staticRefs[8*slot], objRef);
	// printf("\nvalue at slot %d : %" PRIu64 "\n", slot, getRefFromMem(&pool->classPool[classId].staticRefs[slot]));
}

void readRefFromClass(ClassPool *pool, int classId, int slot)
{

}

void printClass(ClassPool *pool, int classId)
{
	printf("\nClassId %d",classId);
	printf("\nStaticRefs : ");
	int i;
	for (i = 0; i < 16; i++)
	{
		printf("%" PRIu64 "\t", getRefFromMem(&(pool->classPool[classId].staticRefs[8*i])));
	}
	
}