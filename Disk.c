#include <stdio.h>
#include <fcntl.h>
#include <err.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "Disk.h"

#define BLOCK_SIZE 512

Disk createDisk(char* filePath) {
	Disk disk;
	disk.fileDescriptor = open(filePath, O_RDWR, 0600);
	if (disk.fileDescriptor < 0) {
		err(1, "Unable to open file: %s", filePath);
	}
	struct stat st;
	stat(filePath, &st);
	size_t fileSize = st.st_size;
	disk.blocksCount = fileSize / BLOCK_SIZE;
	disk.readsCount = 0;
	disk.writesCount = 0;
	return disk;	
}

void validateBlockNumber(Disk disk, int blockNumber) {
	if (blockNumber < 0) {
		errx(2, "Block number is less than 0!");
	}

	if (blockNumber >= (int)disk.blocksCount) {
		errx(3, "Block number is greater than blocks count!");
	}	
}

void validateData(char* data) {
	if (data == NULL) {
		err(4, "Null data pointer!");
	}
}

void readBlockFromDisk(Disk* disk, int blockNumber, char* data) {
	validateBlockNumber((*disk), blockNumber);
	validateData(data);
	if (lseek((*disk).fileDescriptor, blockNumber*BLOCK_SIZE, SEEK_SET) < 0) {
		errx(5, "Unable to lseek %d!", blockNumber);
	}
	if (read((*disk).fileDescriptor, data, BLOCK_SIZE) != BLOCK_SIZE) {
		errx(6, "Unable to read block number: %d!", blockNumber);
	}
	(*disk).readsCount++;
}

void writeBlockToDisk(Disk* disk, int blockNumber, char* data) {
	validateBlockNumber((*disk), blockNumber);
	validateData(data);
	if (lseek((*disk).fileDescriptor, blockNumber*BLOCK_SIZE, SEEK_SET) < 0) {
		errx(5, "Unable to lseek %d!", blockNumber);
	}
	if (write((*disk).fileDescriptor, data, BLOCK_SIZE) != BLOCK_SIZE) {
		errx(7, "Unable to write to block number: %d!", blockNumber);
	}
	(*disk).writesCount++;
}

/*
int main() {
	Disk disk = createDisk("/home/student/61000/file.bin");
	printf("%d\n", disk.blocksCount);
	//validateBlockNumber(disk, 10000000);
	write(disk.fileDescriptor, "aaabbbcccddd", 13);
	printf("%d\n", disk.readsCount);
	char* str = malloc(512);
	writeBlockToDisk(&disk, 2, "anastasia");
	readBlockFromDisk(&disk, 2, str);
	printf("%s\n", str);
	printf("%d\n", disk.readsCount);
	writeBlockToDisk(&disk, 1, "kokoboko");
}
*/
