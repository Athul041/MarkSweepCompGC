#include <stdio.h>
#include <inttypes.h>
#include "MemFunctions.h"

void pushIntToMem(unsigned char *destination, signed int num)
{
    // printf("\nPushing %d to memory", num);
    // // printf("\nInt as hex %02x %02x %02x %02x", num, (num >> 8), (num >> 16), (num >> 24));
    destination[0] = (num >> 24);
    destination[1] = (num >> 16);
    destination[2] = (num >> 8);
    destination[3] = num;
    // // printf("\nInt after push %02x %02x %02x %02x", destination[0], destination[1], destination[2], destination[3]);
}

void pushRefToMem(unsigned char *destination, uint64_t num)
{
    // printf("\nPushing %u to memory", num);
    // printf("\nInt before push %u | %02x %02x %02x %02x", num, num, (num >> 8), (num >> 16), (num >> 24));
    destination[0] = (num >> 56);
    destination[1] = (num >> 48);
    destination[2] = (num >> 40);
    destination[3] = (num >> 32);
    destination[4] = (num >> 24);
    destination[5] = (num >> 16);
    destination[6] = (num >> 8);
    destination[7] = num;
    // printf("\nInt after push %u | %02x %02x %02x %02x", getRefFromMem(destination), destination[0], destination[1], destination[2], destination[3]);
}

signed int getIntFromMem(unsigned char *memPtr)
{
    signed int num = (signed int)memPtr[3] | (memPtr[2] << 8) | (memPtr[1] << 16) | (memPtr[0] << 24);
    return num;
}

uint64_t getRefFromMem(unsigned char *memPtr)
{
    uint64_t num = (uint64_t)memPtr[7] | ((uint64_t)memPtr[6] << 8) | ((uint64_t)memPtr[5] << 16) | ((uint64_t)memPtr[4] << 24)
                    | ((uint64_t)memPtr[3] << 32) | ((uint64_t)memPtr[2] << 40) | ((uint64_t)memPtr[1] << 48) | ((uint64_t)memPtr[0] << 56);
    return num;
}

// void addIntValueToMem(unsigned char* source, signed int num)
// {
//     pushIntToMem(source, getIntFromMem(source)+num);
// }