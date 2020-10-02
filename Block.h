#ifndef BLOCK_H_
#define BLOCK_H_

#include <stdint.h>
#include "SuperBlock.h"	
#include "Inode.h"
#include "NameTable.h"

#define BLOCK_SIZE 512
#define INODES_PER_BLOCK (512/sizeof(Inode))
#define POINTERS_FOR_AN_INODE 128

typedef union Block {
	SuperBlock superBlock;
	Inode inodes[INODES_PER_BLOCK];
	uint32_t indirectPointers[POINTERS_FOR_AN_INODE];
	NameTable nameTable;
	char data[BLOCK_SIZE];
} Block; 

#endif
