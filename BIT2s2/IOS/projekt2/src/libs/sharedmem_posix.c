/**
 * @file sharedmem_posix.c
 * 
 * @brief Functions for working with shared memory - POSIX
 * @date 2021-04-26
 * @author F3lda
 * @update 2021-04-26
 */
#include "sharedmem_posix.h"


int sharedmem_create(void **sharedmemDATA, char *name, int size)
{
	errno = 0;
    int sharedmemID;
	if ((sharedmemID = shm_open(name, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) != -1) {
		ftruncate(sharedmemID, size);
        if (sharedmemDATA != NULL) {
            (*sharedmemDATA) = (void*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, sharedmemID, 0);
        } else {
            munmap(mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, sharedmemID, 0), size);
        }
        close(sharedmemID);
        return 0;
	} else if(errno == EEXIST) {
		return -1;
	} else {
		return -2;
	}
}


int sharedmem_get(void **sharedmemDATA, char *name, int size)
{
	errno = 0;
    int sharedmemID;
	if ((sharedmemID = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) != -1) {
		(*sharedmemDATA) = (void*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, sharedmemID, 0);
        close(sharedmemID);
        return 0;
	} else {
		return -1;
	}
}


int sharedmem_close(void **sharedmemDATA, int size)
{
	return munmap(*sharedmemDATA, size);
}


int sharedmem_destroy(char *name)
{
	return shm_unlink(name);
}
