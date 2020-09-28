/**
 * @file semaphores_systemv.c
 * 
 * @brief Functions for working with shared semaphores - System V Semaphores (wait for zero)
 * @date 2020-05-06
 * @author F3lda
 */
#include "semaphores_systemv.h"


int SemaphoreCreate(key_t key, int value)
{
	errno = 0;
	int SEMid = semget(key, 1, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);// key, number of semaphores in array, flags - IPC_CREAT = create new semaphore if doesn't exist; IPC_EXCL = fail if semaphore already exists; S_IRUSR | S_IWUSR = ask for permissions
	if(SEMid >= 0){// semaphore succesfully created
		// Init semaphore
		union semun {
			int val; // Value for SETVAL
			struct semid_ds *buf; // Buffer for IPC_STAT, IPC_SET 
			unsigned short *array; // Array for GETALL, SETALL
		};
		union semun initValue;
		initValue.val = value; // set value
		if(semctl(SEMid, 0, SETVAL, initValue) < 0){// semaphore id, semaphore number in array [from 0], command, semun union
			return -3;
		}
	} else if(errno == EEXIST){// semaphore already exists
		return -1;
	} else {// other error 
		return -2;
	}
	return SEMid;
}


int SemaphoreGetId(key_t key)
{
	return semget(key, 1, S_IRUSR | S_IWUSR);// key, number of semaphores in array, permissions
}


int SemaphoreRemove(int semaphoreId)
{
	return semctl(semaphoreId, 0, IPC_RMID, 0);// semaphore id, semaphore number in array [from 0], command, [ignored]
}


int SemaphoreGetValue(int semaphoreId)// returns -1 on error
{
	return semctl(semaphoreId, 0, GETVAL, 0);// semaphore id, semaphore number in array [from 0], cmd, [ignored]
}


int SemaphorePushOperation(int semaphoreId, int operation)
{
	struct sembuf sem_buf;
	sem_buf.sem_num = 0;// number of semaphore in the semaphore array [from 0]
	sem_buf.sem_op = operation;// operation - 1: add 1 to semaphore value; -1: subtract 1 from semaphore value; 0: wait until semaphore value is zero 
	sem_buf.sem_flg = 0;// flag - 0: wait until operation is done; IPC_NOWAIT: don't wait; SEM_UNDO - automatically undone when the process terminates
    return semop(semaphoreId, &sem_buf, 1);// semaphore id, semaphore buffer, length of sem_buf array - RETURN: if successful returns 0, otherwise returns -1
}
