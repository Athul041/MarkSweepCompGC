#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

typedef struct {
	unsigned char staticRefs[128];
} binClass;

typedef struct {
	binClass *classPool;
	size_t used;
	size_t size;
} ClassPool;

extern void initClassPool(ClassPool *a, size_t initialSize);
extern void createClassPoolEntry(ClassPool *pool, int classId);
extern void freeClassPool(ClassPool *a);
extern void writeStaticObjectRefToClass(ClassPool *pool, int classId, int slot, uint64_t objRef);
extern void readRefFromClass();
extern void printClass(ClassPool *pool, int classId);