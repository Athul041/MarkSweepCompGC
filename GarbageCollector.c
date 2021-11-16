#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GarbageCollector.h"
#include "ObjectReference.h"
#include "MemFunctions.h"

uint64_t getReferenceTraceIndex(ReferenceTraceTable *RT, uint64_t oldRef)
{
    for(int i=0;i<RT->size;i++)
    {
        if(RT->refTrace[i].oldLoc == oldRef)
        {
            return i;
        }
    }
    return -1;
}

void printRefTable(ReferenceTraceTable *RT)
{
    printf("\nRefTraceTable");
    for (int i = 0; i < RT->size; i++)
    {
        printf("\n Id: %d, old: %" PRIu64 ", new: %" PRIu64, RT->refTrace[i].objId, RT->refTrace[i].oldLoc, RT->refTrace[i].newLoc);
    }
    
}

void addToReferenceTable(ReferenceTraceTable *RT, int objId, uint64_t oldLoc)
{
    if(RT->size <= RT->used)
    {
        RT->size += 1;
        RT->refTrace = realloc(RT->refTrace, RT->size*sizeof(ReferenceTrace));
    }
    RT->refTrace[RT->used].objId = objId;
    RT->refTrace[RT->used].oldLoc = oldLoc;
    RT->refTrace[RT->used].newLoc = 0;
    RT->used += 1;
}

void computeLocations(unsigned char *heap, uint64_t end, ReferenceTraceTable *RT)
{
    uint64_t scan = 0;
    uint64_t free = 0;
    while(scan < end)
    {
        if(getIntFromMem(&heap[scan + 12]) == 1)
        {
            int rtId = getReferenceTraceIndex(RT, scan);
            if (rtId != -1)
            {
                RT->refTrace[rtId].newLoc = free;
                free += 16 + getIntFromMem(&heap[scan]);
            }
        }
        scan += 16 + getIntFromMem(&heap[scan]);
    }
}

int updateReferences(unsigned char *heap, size_t end, ClassPool *CP, ReferenceTraceTable *RT, RootSet *RS)
{
    uint64_t scan = 0;
    int count;
    while(scan < end)
    {        
        if (getIntFromMem(&heap[scan + 12]) == 1)
        {
            int classId = getIntFromMem(&heap[scan + 8]);
            int refSlots = CP->classPool[classId].referenceSlots;
            // printf("\n ObjId %d, refSlots %d", getIntFromMem(&heap[scan + 4]), refSlots);
            for(int i=0;i<refSlots;i++)
            {
                uint64_t ref = getRefFromMem(&heap[scan + 16 + 8*i]);
                // printf("\n ref %" PRIu64, ref);
                if(ref != 0)
                {
                    int rtId = getReferenceTraceIndex(RT, ref);
                    if (rtId != -1)
                    {
                        // printf("\n %d", i);
                        pushRefToMem(&heap[scan + 16 + i*8], RT->refTrace[rtId].newLoc);
                        count += 1;
                    }
                }
            }
            pushIntToMem(&heap[scan + 12], 0);
        }
        scan += 16 + getIntFromMem(&heap[scan]);
    }
    // printf("\n Heap refs updated");
    for(int i=0; i<CP->size; i++)
    {
        if(CP->classPool[i].init == 1)
        {
            for (int j=0; j < 16; j++)
            {
                uint64_t ref = getRefFromMem(&(CP->classPool[i].staticRefs[8*j]));
                if(ref != 0)
                {
                    int rtId = getReferenceTraceIndex(RT, ref);
                    if (rtId != -1)
                    {
                        pushRefToMem(&(CP->classPool[i].staticRefs[8*j]), RT->refTrace[rtId].newLoc);
                        count += 1;
                    }
                }
            }
        }
    }
    // printf("\n Static refs updated");

    for(int i=0; i<RS->size; i++)
    {
        uint64_t ref = RS->rootSet[i];
        if(ref != 0)
        {
            int rtId = getReferenceTraceIndex(RT, ref);
            if (rtId != -1)
            {
                RS->rootSet[i] = RT->refTrace[rtId].newLoc;
                count += 1;
            }
        }
    }
    // printf("\n RootSet updated");
    return count;
}

uint64_t relocate(unsigned char *heap, size_t end, ReferenceTraceTable *RT)
{
    uint64_t scan = 0;
    uint64_t free = 0;
    while(scan < end)
    {
        int rtId = getReferenceTraceIndex(RT, scan);
        int size = getIntFromMem(&heap[scan]);
        if (rtId != -1)
        {
            memcpy(&heap[RT->refTrace[rtId].newLoc], &heap[scan], size + 16);
            free = RT->refTrace[rtId].newLoc + size + 16;
        }
        scan += 16 + size;
    }
    return free;
}
    
void mark(uint64_t root, unsigned char *heap, uint64_t heapHead, ClassPool *CP, int *liveObjects, ReferenceTraceTable *RT)
{
    if(getIntFromMem(&heap[root + 12]) == 1)
    {
        return;
    }
    pushIntToMem(&heap[root + 12], 1);
    // printObject(heap, getIntFromMem(&heap[root + 4]), heapHead, CP);
    addToReferenceTable(RT, getIntFromMem(&heap[root + 4]), root);
    int classId = getIntFromMem(&heap[root + 8]);
    int refSlots = CP->classPool[classId].referenceSlots;
    for(int i=0;i<refSlots;i++)
    {
        uint64_t ref = getRefFromMem(&(heap[root + 16 + i*8]));
        if(ref != 0)
        {
            mark(ref, heap, heapHead, CP, liveObjects, RT);
        }
    }
    // printf("\n Obj Mark complete");
    for(int i=0;i<16;i++)
    {
        uint64_t ref = getRefFromMem(&(CP->classPool[classId].staticRefs[i*8]));
        if(ref != 0)
        {
            mark(ref, heap, heapHead, CP, liveObjects, RT);
        }
    }
    // printf("\n Static Mark complete");
    *liveObjects += 1;
}

void initReferenceTable(ReferenceTraceTable *RT, int size)
{
    RT->size = size;
    RT->used = 0;
    RT->refTrace = malloc(size * sizeof(ReferenceTrace));
}

void runGarbageCollector(unsigned char *heap, uint64_t *heapHead, size_t heapSize, ClassPool *CP, RootSet *RS, int objectsAllocated, int garbageCollected)
{
    // printf("\n Running GC..");
    int liveObjects = 0;
    int deadObjects = 0;
    float heapFree = 0.0;
    int updatedReferences = 0;

    ReferenceTraceTable RT;
    initReferenceTable(&RT, 0);
    // printRootSet(RS);
    for(int i = 0; i<RS->size; i++)
    {
        mark(RS->rootSet[i], heap, *heapHead, CP, &liveObjects, &RT);
    }
    // printf("\n Mark Complete");
    // printRefTable(&RT);
    if (RT.size == objectsAllocated)
    {
        printf("\nOut of Memory!");
        exit(0);
    }
    computeLocations(heap, *heapHead, &RT);
    // printf("\n Compute Locations Complete");
    // printRefTable(&RT);
    updatedReferences = updateReferences(heap, *heapHead, CP, &RT, RS);
    // printf("\n Update References Complete");
    *heapHead = relocate(heap, *heapHead, &RT);
    if (*heapHead >= heapSize)
    {
        printf("\nOut of Memory!");
        exit(0);
    }
    // printRefTable(&RT);
    // printf("\n New heapHead %" PRIu64 "\n", *heapHead);
    heapFree = (float)(heapSize - *heapHead)*100/heapSize;
    // printf("\n HeapFree %.2f", heapFree);
    deadObjects = objectsAllocated - liveObjects;

    printf("G%d LO%d DO%d HF%%%.2f UR%d\n", garbageCollected, liveObjects, deadObjects, heapFree, updatedReferences);
    // printObject(heap, 997, *heapHead, CP);
    // char a;
    // scanf("%c", &a);
}