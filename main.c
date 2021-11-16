#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include "ObjectReference.h"
#include "GarbageCollector.h"

int getArgPayload(char *str, char arg)
{
    char *ar = strchr(str, arg);
    int startIndex = (int)(ar - str) + 1;
    char *remainingStr = malloc((strlen(str)-startIndex)*sizeof(char));
    memset(remainingStr, 0, sizeof(remainingStr));
    memcpy(remainingStr, str+startIndex, strlen(str)-1-startIndex);
    char *space = strchr(remainingStr, ' ');
    int endIndex = space ? (int)(space - remainingStr) + startIndex : sizeof(remainingStr) + startIndex;
    char *payload = malloc((endIndex - startIndex)*sizeof(char));
    memset(payload, 0, sizeof(payload));
    memcpy(payload, remainingStr, endIndex-startIndex);
    return atoi(payload);
}

int main(int argc, char *argv[])
{
    size_t sz = 0;
    if(argc > 2)
    {
        sz = atoi(argv[2]);
        // printf("\nheapSize %d", sz);
    }
    if(sz == 0)
    {
        printf("Heap size not given\n");
        exit(0);
    }

    unsigned char *heap = malloc(sz);
    if(heap == NULL)
    {
        printf("Virtual Memory full\n");
        exit(0);
    }
    uint64_t heapHead = 0;

    char *fileName;
    if(argc > 1)
    {
        fileName = argv[1];
        // printf("\nfilename %s", argv[1]);
    }
    if(strlen(fileName) == 0)
    {
        printf("\nTrace is not given\n");
        exit(0);
    }

    FILE *f1 = fopen(fileName, "r");
    if (!f1)
    {
        printf("\nCould not read file\n");
        exit(0);
    }

    RootSet RS;
    initRootSet(&RS,0);
    ClassPool CP;
    initClassPool(&CP, 0);

    fseek(f1, 0, SEEK_END);
    long filesize = ftell(f1);
    fseek(f1, 0 ,SEEK_SET);

    char request[100];
    int objectsAllocated = 0;
    int garbageCollected = 0;
    
    while(fgets(request, 100, f1) != NULL)
    {
        // printf("\nrequest :%s", request);
        char command = request[0];
        int classId;
        int objId;
        int size;
        int refSlots;
        int slot;
        int refId;
        int objRef;
        switch(command)
        {
            case('a'):
                classId = getArgPayload(request, 'C');
                objId = getArgPayload(request, 'O');
                size = getArgPayload(request, 'S');
                refSlots = getArgPayload(request, 'N');
                // printf("\n heapHead %" PRId64, heapHead);
                if(heapHead + 16 + size >= sz)
                {
                    garbageCollected += 1;
                    // printf("\nG%d", garbageCollected);
                    // printf("\nrequest :%s", request);
                    runGarbageCollector(heap, &heapHead, sz, &CP, &RS, objectsAllocated, garbageCollected);
                }
                createClassPoolEntry(&CP, classId, refSlots);
                allocateObject(heap, objId, size, refSlots, &heapHead, classId);           
                objectsAllocated++;
                break;
            case('+'):
                objId = getArgPayload(request, 'O');
                addObjectRefToRoot(&RS, objId, heap, heapHead);
                // printRootSet(&RS);
                break;
            case('w'):
                objId = getArgPayload(request, 'P');
                slot = getArgPayload(request, '#');
                refId = getArgPayload(request, 'O');
                writeObjectRefToObject(heap, objId, slot, refId, heapHead);
                break;
            case('c'):
                classId = getArgPayload(request, 'C');
                slot = getArgPayload(request, '#');
                objId = getArgPayload(request, 'O');
                objRef = getObjRefFromHeap(heap, heapHead, objId);
                writeStaticObjectRefToClass(&CP, classId, slot, objRef);
                // printClass(&CP, classId);
                break;
            case('r'):
                if(request[2]=='C')
                {
                    classId = getArgPayload(request, 'C');
                    slot = getArgPayload(request, '#');
                    readRefFromClass(&CP, classId, slot);
                }
                else
                {
                    objId = getArgPayload(request, 'O'); 
                    slot = getArgPayload(request, '#');
                    readRefFromObj(heap, objId, slot, heapHead);
                }
                break;
            case('-'):
                objId = getArgPayload(request, 'O');
                char a;
                // scanf("%c", &a);
                objRef = getObjRefFromHeap(heap, heapHead, objId);
                deleteObjectRefFromRoot(&RS, objRef);
                // printRootSet(&RS);
                // scanf("%c", &a);
                break;
        }
    }
    garbageCollected += 1;
    // printf("\nFinal GC");
    runGarbageCollector(heap, &heapHead, sz, &CP, &RS, objectsAllocated, garbageCollected);
    printf("\nExecution Complete");
    fclose(f1);
}