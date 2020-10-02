#ifndef SUPERBLOCK_H_
#define SUPERBLOCK_H_

#include <stdint.h>

typedef struct SuperBlock {
	uint32_t blocksCount;
	uint32_t inodeBlocksCount;
	uint32_t inodesCount;
	uint32_t freeBlocksCount;
	uint32_t freeInodesCount;
} SuperBlock;

#endif
