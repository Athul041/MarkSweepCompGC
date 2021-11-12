#include <inttypes.h>

typedef struct {
    uint64_t *rootSet;
    int size;
    int used;
} RootSet;

extern void addObjectRefToRoot(RootSet *RS, int objId);
extern void deleteObjectRefFromRoot(RootSet *RS, int objId);
extern void initRootSet(RootSet *RT, int size);
extern void printRootSet(RootSet *RS);