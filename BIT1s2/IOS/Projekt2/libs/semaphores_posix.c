/**
 * @file semaphores_posix.c
 * 
 * @brief Functions for working with shared semaphores - POSIX Semaphores (wait if zero)
 * @date 2020-05-06
 * @author F3lda
 */
#include "semaphores_posix.h"


int semaphore_create(sem_t **semaphore, char *name, int value)
{
	errno = 0;
	if((*semaphore = sem_open(name, O_CREAT | O_EXCL, DEFFILEMODE, value)) != SEM_FAILED){
		return 0;
	} else if(errno == EEXIST){
		return -1;
	} else {
		return -2;
	}
}


int semaphore_get(sem_t **semaphore, char *name)
{
	errno = 0;
	if((*semaphore = sem_open(name, 0)) != SEM_FAILED){
		return 0;
	} else {
		return -1;
	}
}


int semaphore_delete(sem_t *semaphore, char *name)
{
	sem_close(semaphore);
	return sem_unlink(name);
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