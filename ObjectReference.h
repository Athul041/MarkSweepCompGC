#include <inttypes.h>

extern uint64_t getObjRefFromHeap(unsigned char *heap, uint64_t heapHead, int objId);
extern void writeObjectRefToObject(unsigned char *heap, int objId, int slot, int refId, uint64_t heapHead);
extern void allocateObject(unsigned char *heap, int objId, int size, int refSlots, uint64_t *heapHead, int classId);
extern void readRefFromObj(unsigned char *heap, int objId, int slot, uint64_t heapHead);
extern void printObject(unsigned char *heap, int objId, uint64_t heapHead);