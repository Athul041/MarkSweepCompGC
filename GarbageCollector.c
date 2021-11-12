#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "RootSet.h"
#include "ObjectReference.h"
#include "classPool.h"
#include "MemFunctions.h"

void mark(uint64_t root, unsigned char *heap, uint64_t heapHead, ClassPool *CP)
{
    pushIntToMem(&heap[getObjRefFromHeap(heap, heapHead, root) + 12], 1);
    int classId = getIntFromMem(heap[heapHead + 8]);
    int refSlots = CP->classPool[classId].referenceSlots;
}

void runGarbageCollector();