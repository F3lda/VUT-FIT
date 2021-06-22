/**
 * @file semaphores_posix.c
 * 
 * @brief Functions for working with shared semaphores - POSIX Semaphores (wait if zero)
 * @date 2020-05-06
 * @author F3lda
 * @update 2021-04-26
 */
#include "semaphores_posix.h"


int semaphore_create(sem_t **semaphore, char *name, int value)
{
	errno = 0;
	if ((*semaphore = sem_open(name, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, value)) != SEM_FAILED) {
		return 0;
	} else if(errno == EEXIST) {
		return -1;
	} else {
		return -2;
	}
}


int semaphore_get(sem_t **semaphore, char *name)
{
	errno = 0;
	if ((*semaphore = sem_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, 1)) != SEM_FAILED) {
		return 0;
	} else {
		return -1;
	}
}


int semaphore_close(sem_t *semaphore)
{
	return sem_close(semaphore);
}


int semaphore_destroy(char *name)
{
	return sem_unlink(name);
}


int semaphore_init(sem_t **semaphore, bool global, int value)
{
    if (global) {
        *semaphore = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if (*semaphore != NULL) {
            return sem_init(*semaphore, 1, value);
        } else {
            return -2;
        }
    } else {
        return sem_init(*semaphore, 0, value);
    }
}


int semaphore_delete(sem_t *semaphore, bool global)
{
    int result = sem_destroy(semaphore);
    if (global) {
        result = munmap(semaphore, sizeof(sem_t))*2;
    }
    return result;
}


int semaphore_get_value(sem_t *semaphore, int *value)
{
	return sem_getvalue(semaphore, value);
}


int semaphore_lock_or_wait(sem_t *semaphore)
{
	return sem_wait(semaphore);
}


int semaphore_unlock(sem_t *semaphore)
{
	return sem_post(semaphore);
}
