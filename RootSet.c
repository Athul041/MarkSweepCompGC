#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include "RootSet.h"

void addObjectRefToRoot(RootSet *RS, int objId)
{
    if(RS->size == RS->used)
    {
        RS->size += 1;
        RS->rootSet = malloc(RS->size * sizeof(uint64_t));
    }
    RS->rootSet[RS->used++] = objId;
}

void deleteObjectRefFromRoot(RootSet *RS, int objId)
{
    for(int i=0; i < RS->size; i++)
    {
        if(RS->rootSet[i] == objId)
        {
            RS->rootSet[i] == -1;
            return;
        }
    }
}

void initRootSet(RootSet *RT, int size)
{
    RT->size = size;
    RT->rootSet = malloc(size * sizeof(uint64_t));
}

void printRootSet(RootSet *RS)
{
    printf("\nRootSet : ");
    for(int i=0; i < RS->size; i++)
    {
        printf("%d\t", RS->rootSet[i]);
    }
}