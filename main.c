#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include "classPool.h"
#include "ObjectReference.h"
#include "RootSet.h"

int getArgPayload(char *str, char arg)
{
    char *ar = strchr(str, arg);
    int startIndex = (int)(ar - str) + 1;
    printf("\nstartIndex %d", startIndex);
    char *remainingStr = malloc((strlen(str)-startIndex)*sizeof(char));
    memset(remainingStr, 0, sizeof remainingStr);
    printf("\nstrlen(str)-startIndex %d", strlen(str)-1-startIndex);
    memcpy(remainingStr, str+startIndex, strlen(str)-startIndex);
    printf("\nremainingStr %s", remainingStr);
    char *space = strchr(remainingStr, ' ');
    int endIndex = (int)(space - remainingStr) + startIndex;
    printf("\nendIndex %d", endIndex);
    char *payload = malloc((endIndex - startIndex)*sizeof(char));
    memcpy(payload, remainingStr, endIndex-startIndex);
    printf("\npayload %s", payload);
    return atoi(payload);
}

int main(int argc, char *argv[])
{
    size_t sz = 0;
    if(argc > 2)
    {
        sz = atoi(argv[2]);
        printf("\nheapSize %d", sz);
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
        printf("\nfilename %s", argv[1]);
    }
    if(strlen(fileName) == 0)
    {
        printf("Trace is not given\n");
        exit(0);
    }

    FILE *f1 = fopen(fileName, "rb");
    if (!f1)
    {
        printf("Could not read file\n");
        exit(0);
    }

    RootSet RS;
    initRootSet(&RS,0);
    ClassPool CP;
    initClassPool(&CP, 0);

    fseek(f1, 0, SEEK_END);
    long filesize = ftell(f1);
    fseek(f1, 0 ,SEEK_SET);

    char *request;
    fgets(request, filesize, f1);
    printf("\nrequest :%s", request);
    
    while(request != NULL)
    {
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
                printf("\nclassId %d", classId);
                objId = getArgPayload(request, 'O');
                printf("\nobjId %d", objId);
                size = getArgPayload(request, 'S');
                printf("\nsize %d", size);
                refSlots = getArgPayload(request, 'N');
                printf("\nrefSlots %d", refSlots);
                createClassPoolEntry(&CP, classId);
                allocateObject(heap, objId, size, refSlots, &heapHead, classId);
                break;
            case('+'):
                objId = getArgPayload(request, 'O');
                addObjectRefToRoot(&RS, objId);
                printRootSet(&RS);
                break;
            case('w'):
                refId = getArgPayload(request, 'P');
                slot = getArgPayload(request, '#');
                objId = getArgPayload(request, 'O');
                writeObjectRefToObject(heap, objId, slot, refId, heapHead);
                printObject(heap, objId, heapHead);
                break;
            case('c'):
                classId = getArgPayload(request, 'C');
                slot = getArgPayload(request, '#');
                objId = getArgPayload(request, 'O');
                objRef = getObjRefFromHeap(heap, heapHead, objId);
                writeStaticObjectRefToClass(&CP, classId, slot, objRef);
                printClass(&CP, classId);
                break;
            // case('r'):
            //     if(tolower(line[2])=='c')
            //     {
            //         readRefFromClass();
            //     }
            //     else
            //     {
            //         readRefFromObj();
            //     }
            //     break;
            case('-'):
                objId = getArgPayload(request, 'O');
                deleteObjectRefFromRoot(&RS, objId);
                printRootSet(&RS);
                break;
        }
        char a;
        scanf("%c", &a);
        // if(isHeapFull())
        // {
        //     runGarbageCollector();
        // }
        fgets(request, filesize, f1);
    }
    // runGarbageCollector();
    fclose(f1);
}