#ifndef MEMFUNCTIONS_H
#define MEMFUNCTIONS_H
#include <inttypes.h>

extern void pushIntToMem(unsigned char *destination, signed int num);
extern void pushRefToMem(unsigned char *destination, uint64_t num);
extern signed int getIntFromMem(unsigned char *memPtr);
extern uint64_t getRefFromMem(unsigned char *memPtr);
extern signed int getIntFromLoadedMem(unsigned char *memPtr);
extern void relocateInt(unsigned char *source, unsigned char *destination);
extern void addIntValueToMem(unsigned char* source, signed int num);

#endif