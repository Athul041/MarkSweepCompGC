#ifndef ROOTSET_H
#define ROOTSET_H
#include <inttypes.h>

typedef struct {
    uint64_t *rootSet;
    int size;
    int used;
} RootSet;

extern void addObjectRefToRoot(RootSet *RS, int objId, unsigned char *heap, uint64_t heapHead);
extern void deleteObjectRefFromRoot(RootSet *RS, uint64_t objref);
extern void initRootSet(RootSet *RT, int size);
extern void printRootSet(RootSet *RS);

#endif