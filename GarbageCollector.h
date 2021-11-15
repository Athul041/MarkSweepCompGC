#ifndef GARBAGECOLLECTOR_H
#define GARBAGECOLLECTOR_H
#include <stdlib.h>
#include <inttypes.h>
#include "classPool.h"
#include "RootSet.h"

typedef struct {
    int objId;
    uint64_t oldLoc;
    uint64_t newLoc;
} ReferenceTrace ;

typedef struct {
    ReferenceTrace *refTrace;
    int size;
    int used;
} ReferenceTraceTable;

extern uint64_t getReferenceTraceIndex(ReferenceTraceTable *RT, uint64_t oldRef);
extern void computeLocations(unsigned char *heap, uint64_t end, ReferenceTraceTable *RT);
extern int updateReferences(unsigned char *heap, size_t end, ClassPool *CP, ReferenceTraceTable *RT, RootSet *RS);
extern uint64_t relocate(unsigned char *heap, size_t end, ReferenceTraceTable *RT);
extern void mark(uint64_t root, unsigned char *heap, uint64_t heapHead, ClassPool *CP, int *liveObjects, ReferenceTraceTable *RT);
extern void addToReferenceTable(ReferenceTraceTable *RT, int objId, uint64_t oldLoc);
extern void initReferenceTable(ReferenceTraceTable *RT, int size);
extern void runGarbageCollector(unsigned char *heap, uint64_t *heapHead, size_t heapSize, ClassPool *CP, RootSet *RS, int objectsAllocated, int garbageCollected);

#endif