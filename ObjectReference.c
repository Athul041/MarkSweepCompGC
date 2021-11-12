#include <stdio.h>
#include "MemFunctions.h"

uint64_t getObjRefFromHeap(unsigned char *heap, uint64_t heapHead, int objId)
{
    int scanner = 0;
    while(scanner < heapHead)
    {
        if(getIntFromMem(&heap[scanner + 4]) == objId)
        {
            return scanner;
        }
        scanner += getIntFromMem(&heap[scanner]) + 16;
    }
}

void writeObjectRefToObject(unsigned char *heap, int objId, int slot, int refId, uint64_t heapHead)
{
    pushRefToMem(&heap[getObjRefFromHeap(heap, heapHead, objId) + 16 + 8*slot], getObjRefFromHeap(heap, heapHead, refId));
}

void allocateObject(unsigned char *heap, int objId, int size, int refSlots, uint64_t *heapHead, int classId)
{
    pushIntToMem(&heap[*heapHead], size);
    pushIntToMem(&heap[*heapHead + 4], objId);
    pushIntToMem(&heap[*heapHead + 8], classId);
    pushIntToMem(&heap[*heapHead + 12], 0);
    heapHead += 16 + refSlots*8;
}

void readRefFromObj(unsigned char *heap, int objId, int slot, uint64_t heapHead)
{

}

void printObject(unsigned char *heap, int objId, uint64_t heapHead)
{
    uint64_t objRef = getObjRefFromHeap(heap, heapHead, objId);
    printf("\nObject %d",objId);
    int size = getIntFromMem(&heap[objRef]);
    printf("\n\tSize %d", size);
    printf("\n\tClassId %d",getIntFromMem(&heap[objRef + 8]));
    printf("\nReferences : ");
    for(int i=0; i<size; i++)
    {
        printf("%" PRIu64 "\t", getIntFromMem(&heap[objRef + 8]));
    }
}