#ifndef NAME_TABLE_H_
#define NAME_TABLE_H_

#include <stdint.h>

#define BLOCK_SIZE 512
#define MAX_NAME_LENGTH 20

typedef struct ObjectName {
	char name[MAX_NAME_LENGTH];
	uint16_t inodeID;
	uint32_t blockNumber;
} ObjectName;

#define MAX_OBJECTS (BLOCK_SIZE / sizeof(ObjectName))

typedef struct NameTable {
	uint32_t blockNumber;
	uint16_t currentObject;
	ObjectName directoryObjects[MAX_OBJECTS];	
} NameTable;

#define INIT_NAMETABLE(X) NameTable X = {.currentObject = 0}

#endif
