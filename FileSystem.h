#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include "Disk.h"
#include "SuperBlock.h"
#include "Inode.h"
#include "Block.h"

typedef struct FileSystem { 
	bool* blocksStatus;
	struct SuperBlock *superBlock;
	struct Inode *inode;
	union Block *block;
} FileSystem;

FileSystem makeFileSystem(Disk* disk);
int allocateFreeInodeBlock(FileSystem* fs,  Disk* disk);
int addInode(FileSystem* fs, Disk* disk, Inode* inode);
int allocateFreeDataBlock(FileSystem* fs);
int addDataBlock(FileSystem* fs, Disk* disk, Block block);
void createNameTablesForInode(FileSystem* fs, Disk* disk, Inode* inode);
bool addObjectToNameTable(FileSystem* fs, Disk* disk, char* name, Inode* directoryInode, char fileOrDir);
Inode returnInodeByGivenBlockNumAndID(Disk* disk, int blockNumber, int inodeID);
Inode findObjectsInodeInNameTable(Disk* disk, Inode dirInode, char* objectName);
Inode followPathToObject(Disk* disk, char* path, Inode root);

#endif
