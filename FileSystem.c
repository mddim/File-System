#include <stdio.h>
#include <fcntl.h>
#include <stdbool.h>
#include <err.h>
#include <string.h>
#include "Disk.h"
#include "Inode.h"
#include "SuperBlock.h"
#include "Block.h"
#include "FileSystem.h"

#define INODE_SIZE sizeof(Inode)

FileSystem makeFileSystem(Disk *disk) {
	SuperBlock superBlock;
	superBlock.blocksCount = (*disk).blocksCount;
	superBlock.inodeBlocksCount = (*disk).blocksCount / 5;
	superBlock.inodesCount = superBlock.inodeBlocksCount * (BLOCK_SIZE / INODE_SIZE);
	superBlock.freeBlocksCount = superBlock.blocksCount;
	superBlock.freeInodesCount = superBlock.inodesCount;
	FileSystem fileSystem;
	fileSystem.blocksStatus = malloc(superBlock.blocksCount * sizeof(bool));
	unsigned int i = 0;
	for (; i < superBlock.blocksCount; i++) {
		fileSystem.blocksStatus[i] = false;
	}
	fileSystem.superBlock = malloc(sizeof(SuperBlock));
	*fileSystem.superBlock = superBlock;

	Block block;
	block.superBlock = superBlock;
	writeBlockToDisk(disk, 0, block.data);
	fileSystem.blocksStatus[0] = true;

	return fileSystem;
}

int allocateFreeInodeBlock(FileSystem* fs, Disk* disk) {
	unsigned int i = 1;
	for (; i <= fs->superBlock->inodeBlocksCount; i++) {
		if (fs->blocksStatus[i] == false) {
			Inode inodes[INODES_PER_BLOCK];
			unsigned int j = 0;
			for (; j < INODES_PER_BLOCK; j++) {
				inodes[j].free = 1;
			}
			Block block;
			unsigned int z = 0;
			for (; z < INODES_PER_BLOCK; z++) {
				block.inodes[z] = inodes[z];
			}
			writeBlockToDisk(disk, i, block.data);
			fs->blocksStatus[i] = true;
			return i;
		}
		else {
			Block block;
			readBlockFromDisk(disk, i, block.data);
			unsigned int j = 0;
			for (; j < INODES_PER_BLOCK; j++) {
				if (block.inodes[j].free == true) {
					return i;
				}
			}
		}
	}
		errx(9, "No space for a new inode!");	
}

int addInode(FileSystem* fs, Disk* disk, Inode* inode) {
	int blockNumber = allocateFreeInodeBlock(fs, disk);
	Block block;
	readBlockFromDisk(disk, blockNumber, block.data);
	unsigned int i = 0;
	for (; i < INODES_PER_BLOCK; i++) {
		if (block.inodes[i].free == true) {
			inode->free = false;
			inode->blockNumber = blockNumber;
			inode->inodeID = blockNumber*10 + i;
			block.inodes[i] = *inode;
			writeBlockToDisk(disk, blockNumber, block.data);
			break;
		}
	}
	return blockNumber;
}

int allocateFreeDataBlock(FileSystem* fs) {
	unsigned int i = fs->superBlock->inodeBlocksCount + 1;
	for (; i < fs->superBlock->blocksCount; i++) {
		if (fs->blocksStatus[i] == false) {
			return i;
		}
	}
	errx(10, "No free blocks!");
}

int addDataBlock(FileSystem* fs, Disk* disk, Block block) {
	int blockNumber = allocateFreeDataBlock(fs);
	writeBlockToDisk(disk, blockNumber, block.data);
	fs->blocksStatus[blockNumber] = true;
	return blockNumber;
}

void createNameTablesForInode(FileSystem* fs, Disk* disk, Inode* inode) {
	const int tablesCount = 4;
	NameTable nameTables[tablesCount];
	Block blocks[tablesCount];
	int i = 0;
	for (; i < tablesCount; i++) {
		nameTables[i].currentObject = 0;
		blocks[i].nameTable = nameTables[i];
		int blockNumber = addDataBlock(fs, disk, blocks[i]);
		inode->direct[i] = blockNumber;
	}
}

bool addObjectToNameTable(FileSystem* fs, Disk* disk, char* name, Inode* directoryInode, char fileOrDir) {
	Block block;
	int i = 0;
	for (; i < INODE_DIRECT_POINTERS; i++) {
		readBlockFromDisk(disk, directoryInode->direct[i], block.data);
		NameTable nameTable = block.nameTable;
		if (nameTable.currentObject < MAX_OBJECTS) {
			if (strlen(name) > MAX_NAME_LENGTH) {
				errx(13, "Object name should be less than %d symbols!", MAX_NAME_LENGTH);
			}
			INIT_INODE_DIR(objectInode);
			if (fileOrDir == 'f') {
				objectInode.type = 'f';
			}
			if (fileOrDir == 'd') {
				createNameTablesForInode(fs, disk, &objectInode);
			}
			int objectInodeBlockNumber = addInode(fs, disk, &objectInode);
			nameTable.directoryObjects[nameTable.currentObject].inodeID = objectInode.inodeID;
			nameTable.directoryObjects[nameTable.currentObject].blockNumber = objectInodeBlockNumber;
			memcpy(nameTable.directoryObjects[nameTable.currentObject].name, name, MAX_NAME_LENGTH);
			nameTable.currentObject++;
			block.nameTable = nameTable;
			writeBlockToDisk(disk, directoryInode->direct[i], block.data);
			return true;
		}
	}
	errx(15, "No space for a new object in directory!");
}


Inode returnInodeByGivenBlockNumAndID(Disk* disk, int blockNumber, int inodeID) {
	Block block;
	readBlockFromDisk(disk, blockNumber, block.data);
	unsigned int i = 0;
	for (; i < INODES_PER_BLOCK; i++) {
		if (block.inodes[i].inodeID == inodeID) {
			return block.inodes[i];
		}
	}	
	errx(11, "Given object inode is not in this inode block!");
}

Inode findObjectsInodeInNameTable(Disk* disk, Inode dirInode, char* objectName) {
	Inode objectInode;
	uint16_t inodeID;
	uint32_t blockNumber;
	int i = 0;
	for (; i < INODE_DIRECT_POINTERS; i++) {
		Block block;
		if (dirInode.direct[i] == 0) {
			errx(14, "Directory is empty!");
		}
		readBlockFromDisk(disk, dirInode.direct[i], block.data);
		NameTable nameTable = block.nameTable;
		unsigned int j = 0;
		for (; j < MAX_OBJECTS; j++) {
			if (strcmp(objectName, nameTable.directoryObjects[j].name) == 0) {
				inodeID = nameTable.directoryObjects[j].inodeID;
				blockNumber = nameTable.directoryObjects[j].blockNumber;
				objectInode = returnInodeByGivenBlockNumAndID(disk, blockNumber, inodeID);
				return objectInode;
			}
		}
	}	
	errx(12, "Object %s doesn't exist in this directory", objectName);
}

Inode followPathToObject(Disk* disk, char* path, Inode root) {
	char str[strlen(path)];
	memcpy(str, path, strlen(path));
	char* object = strtok(str, "/");
	Inode currInode = root;
	while (object != NULL) {
		if (strcmp(object, "+") == 0) {
			object = strtok(NULL, "/");
		}
		Inode nextInode = findObjectsInodeInNameTable(disk, currInode, "abc");
		currInode = nextInode;
		object = strtok(NULL, "/");
	}
	return currInode;
}

/*
int main() {

	Disk disk = createDisk("/home/student/61000/file.bin");
	FileSystem fs = makeFileSystem(&disk);
	Block block;
	SuperBlock sb;
	block.superBlock = sb;
	readBlockFromDisk(&disk, 0, block.data);

	
	int a = allocateFreeInodeBlock(&fs, &disk);
	int i = 0;
	for(; i < 10; i++) {
		printf("%d\n", fs.blocksStatus[i]);
	}
	Inode is[14];
	i = 0;
	for (; i < 14; i++) {
		is[i].free = false;
		addInode(&fs, &disk, is[i]);
	}
	int b = allocateFreeInodeBlock(&fs, &disk);
	printf("%d\n", b);
	
	i = 0;
	for (; i < 10; i++) {
		printf("%d\n", fs.blocksStatus[i]);
	}
	Block block2;
	memcpy(block2.data, "az ne moga da se spra da tancuwam cqla nosht", 512);
	Block block2;

	Block block2;
	Inode is;
	addDataBlock(&fs, &disk, block2);
	addInode(&fs, &disk, &is);
	createNameTablesForInode(&fs, &disk, &is);
	addObjectToNameTable(&fs, &disk, "abc", &is, 'd');
	Inode inode2 = findObjectsInodeInNameTable(&disk, is, "abc");

	Inode inode3 = followPathToObject(&disk, "/+/abc", is);
	printf("object id:%d, %d\n", inode3.inodeID, inode3.blockNumber);

}
*/
