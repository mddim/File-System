#ifndef DISK_H_
#define DISK_H_

#include <stdlib.h>

typedef struct {
	int fileDescriptor;
	size_t blocksCount;
	size_t readsCount;
	size_t writesCount;
} Disk;

Disk createDisk(char* filePath);
void validateBlockNumber(Disk disk, int blockNumber);
void validateData(char* data);
void readBlockFromDisk(Disk* disk, int blockNumber, char* data);
void writeBlockToDisk(Disk* disk, int blockNumber, char* data);

#endif
