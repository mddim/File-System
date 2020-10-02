#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <err.h>
#include <time.h>
#include "Disk.h"
#include "FileSystem.h"

void printObjectInfo(Disk* disk, char* path, Inode root); 
//void printAccessMode(int num);
	
int main() {

	char* fileName = getenv("BDSM_FS");
	Disk disk;
	FileSystem fileSystem;
	INIT_INODE_DIR(root);

	while (true) {
		char line[20], cmd[20], arg[20];

		if (fgets(line, 20, stdin) == NULL) {
			break;
		}

		sscanf(line, "%s %s", cmd, arg);

		if (strcmp(cmd, "mkfs") == 0) {
			disk = createDisk(fileName);
			fileSystem = makeFileSystem(&disk);
			time_t currentTime;
			time(&currentTime);
			localtime(&currentTime);
			root.modificationTime = currentTime;
			createNameTablesForInode(&fileSystem, &disk, &root);
			addInode(&fileSystem, &disk, &root); 
			printf("%s\n", "File system has been created successfully!");
		}
		else if (strcmp(cmd, "debug") == 0) {
			Block block;
			if (sizeof(block) > BLOCK_SIZE) {
				errx(16, "Size of block is bigger than 512");
			}
			struct stat st;
			stat(fileName, &st);
			size_t fileSize = st.st_size;
			if (fileSystem.superBlock->blocksCount != fileSize / BLOCK_SIZE) {
				errx(17, "Incorrect count of blocks!");
			}
			bool allBlocksTaken = true;
			unsigned int i = 0;
			for (; i < fileSystem.superBlock->blocksCount; i++) {
				if (fileSystem.blocksStatus[i] == false) {
					allBlocksTaken = false;
					break;
				}	
			}
			if (allBlocksTaken) {
				errx(18, "No free memory in file system!");
			}
			if (fileSystem.blocksStatus[0] == false) {
				errx(19, "Superblock is not added to file system!");
			}
			printf("%s\n", "No errors!");
		}
		else if (strcmp(cmd, "lsobj") == 0) {
			printObjectInfo(&disk, arg, root);
		}

		else if (strcmp(cmd, "exit") == 0) {
			break;
		}
	}
	
}



void printObjectInfo(Disk* disk, char* path, Inode root) {
	Inode inode = followPathToObject(disk, path, root);
	printf("%d ", inode.accessPermissions);
	printf("%d ", inode.ownerID);
	printf("%d ", inode.groupID);
	printf("%d ", inode.size);
	time_t tmp = inode.modificationTime;
	time(&tmp);
	printf("%s \n", ctime(&tmp));
}

/* 
void printAccessMode(int num) {
	char buf[3][3];
	while (num > 0) {
		int i = 0;
		int a = num%10;
		char b[3];
		switch(a) {
			case'4': memcpy(buf[i][3], "r--", 3);
			break;
			case'2': memcpy(buf[i][3], "-w-", 3);
			break;
			case'1': memcpy(buf[i][3], "--x", 3);
			break;
			case'6': memcpy(buf[i][3], "rw-", 3);
			break;
			case'3': memcpy(buf[i][3], "-wx", 3);
			break;
			case'7': memcpy(buf[i][3], "rwx", 3);
			break;
			case'5': memcpy(buf[i][3], "r-x", 3);
			break;
		}
		i++;
		num = num / 10;
	}
	int i = 0;
	for (; i < 3; i++) {
		int j = 0;
		for (; j < 3; j++) {
			printf("%s", buff[i][3]);
		}
	}
}
*/
