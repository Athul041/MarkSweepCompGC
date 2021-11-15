#include <stdio.h>
#include <string.h>
#include "MemFunctions.h"

uint64_t getObjRefFromHeap(unsigned char *heap, uint64_t heapHead, int objId)
{
    uint64_t scanner = 0;
    while(scanner < heapHead)
    {
        int objAtScanner = getIntFromMem(&heap[scanner + 4]);
        if(objAtScanner == objId)
        {
            return scanner;
        }
        int size = getIntFromMem(&heap[scanner]);
        scanner += size + 16;
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

void printObject(unsigned char *heap, int objId, uint64_t heapHead)
{
    uint64_t objRef = getObjRefFromHeap(heap, heapHead, objId);
    printf("\nObject %d",objId);
    int size = getIntFromMem(&heap[objRef]);
    printf("\n\tSize %d", size);
    printf("\n\tClassId %d",getIntFromMem(&heap[objRef + 8]));
    printf("\n\tReferences :");
    for(int i=0; (i*8)<size; i++)
    {
        printf("%" PRIu64 "\t", getRefFromMem(&heap[objRef + 16 + 8*i]));
    }
}