#ifndef INODE_H_
#define INODE_H_

#include <stdint.h>
#include <time.h>
#include <stdbool.h>

#define INODE_DIRECT_POINTERS 4

typedef struct Inode {
	uint16_t inodeID;
	uint32_t blockNumber;
	char type;
	bool free;
	uint32_t size;
	uint32_t direct[INODE_DIRECT_POINTERS];
	uint32_t indirect;
	uint16_t ownerID;
	uint16_t groupID;
	uint32_t accessPermissions;
	uint16_t future;
	time_t modificationTime;
} Inode;

#define INIT_INODE_DIR(X) Inode X = {.inodeID = 0, .blockNumber = 0, .type = 'd', .free = true, .size = BLOCK_SIZE, .direct = {0, 0, 0, 0}, .indirect = 0, .ownerID = 1, .groupID = 1, .accessPermissions = 775, .future = 0}

#define INIT_INODE_FILE(X) Inode X = {.inodeID = 0, .blockNumber = 0, .type = 'f', .free = true, .size = 0, .direct = {0, 0, 0, 0}, .indirect = 0, .ownerID = 1, .groupID = 1, .accessPermissions = 664, .future = 0}

#endif
