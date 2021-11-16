#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include "RootSet.h"
#include "ObjectReference.h"

void addObjectRefToRoot(RootSet *RS, int objId, unsigned char *heap, uint64_t heapHead)
{
    if(RS->size == RS->used)
    {
        RS->size += 1;
        RS->rootSet = realloc(RS->rootSet, RS->size * sizeof(uint64_t));
    }
    RS->rootSet[RS->used] = getObjRefFromHeap(heap, heapHead, objId);
    RS->used += 1;
}

void deleteObjectRefFromRoot(RootSet *RS, uint64_t objref)
{
    // printRootSet(RS);
    // printf("\n objRef %" PRIu64, objref);
    int i;
    for(i=RS->size-1; i >= 0; i--)
    {
        if(RS->rootSet[i] == objref)
        {
            // printf("\n index %d", i);
            RS->rootSet[i] = 0;
            // printRootSet(RS);
            return;
        }
    }
}

void initRootSet(RootSet *RT, int size)
{
    RT->size = size;
    RT->used = 0;
    RT->rootSet = malloc(size * sizeof(uint64_t));
}

void printRootSet(RootSet *RS)
{
    printf("\nRootSet : ");
    int i;
    for(i=0; i < RS->size; i++)
    {
        if (RS->rootSet[i] != 0)
        {
            printf("%" PRIu64"\t", RS->rootSet[i]);
        }
    }
}