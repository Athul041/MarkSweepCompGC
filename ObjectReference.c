#include <stdio.h>
#include <string.h>
#include "MemFunctions.h"
#include "classPool.h"

uint64_t getObjRefFromHeap(unsigned char *heap, uint64_t heapHead, int objId)
{
    uint64_t scanner = 0;
    while(scanner < heapHead)
    {
        if(getIntFromMem(&heap[scanner + 4]) == objId)
        {
            return scanner;
        }
        scanner += getIntFromMem(&heap[scanner]) + 16;
    }
    if(scanner >= heapHead)
    {
        printf("\n Object %d does not exist on Heap", objId);
        exit(0);
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
    memset(&heap[*heapHead + 16], 0, refSlots*8);
    *heapHead += 16 + size;
}

void readRefFromObj(unsigned char *heap, int objId, int slot, uint64_t heapHead)
{
    uint64_t objRef = getObjRefFromHeap(heap, heapHead, objId);

}

void printObject(unsigned char *heap, int objId, uint64_t heapHead, ClassPool *CP)
{
    uint64_t objRef = getObjRefFromHeap(heap, heapHead, objId);
    printf("\n objRef %" PRIu64, objRef);
    printf("\nObject %d",objId);
    printf("\nMarked %d",getIntFromMem(&heap[objRef + 12]));
    int size = getIntFromMem(&heap[objRef]);
    printf("\n\tSize %d", size);
    printf("\n\tClassId %d",getIntFromMem(&heap[objRef + 8]));
    printf("\n\tReferences %d :", CP->classPool[getIntFromMem(&heap[objRef + 8])].referenceSlots);
    int i;
    for(i=0; i<CP->classPool[getIntFromMem(&heap[objRef + 8])].referenceSlots; i++)
    {
        printf("%" PRIu64 "\t", getRefFromMem(&heap[objRef + 16 + 8*i]));
    }
}