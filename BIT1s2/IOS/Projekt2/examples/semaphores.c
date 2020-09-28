#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* thread */
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

/* semaphore */
#include <errno.h>
#include <semaphore.h>
#include <sys/fcntl.h> /* For O_* constants */
#include <sys/stat.h>  /* For mode constants */

/* 

POSIX Semaphore - wait if zero

*/

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





sem_t *semaphore;

void threadfunc() {
    while (1) {
        sem_wait(semaphore);
		printf("Semaphore locked.\n");
        printf("Hello from da thread!\n");
        sem_post(semaphore);
		printf("Semaphore unlocked.\n");
        sleep(1);
    }
}

#define SEMAPHORE_KEY "/randomkey"

int main(void)
{
	if(semaphore_create(&semaphore, SEMAPHORE_KEY, 1) < -1){printf("ERROR - semaphore create\n"); return 1;}
	printf("SEMAPHORE - created.\n");
	
    
	pthread_t *mythread;
    mythread = (pthread_t *)malloc(sizeof(*mythread));
    pthread_create(mythread, NULL, (void*)threadfunc, NULL);
	
	
    getchar();
    semaphore_lock_or_wait(semaphore);
    printf("SEMAPHORE - locked.\n");


	
	int value;
	semaphore_get_value(semaphore,&value);
	printf("%d\n",value);
	// do stuff with whatever is shared between threads
	
	
	
    getchar();
    semaphore_unlock(semaphore);
    printf("SEMAPHORE - unlocked.\n");
    
	
	
    getchar();
	semaphore_delete(semaphore, SEMAPHORE_KEY);
	printf("SEMAPHORE - deleted.\n");
	
	

    printf("Something is wrong, I can feel it: %s [%d]\n", strerror(errno), errno);
    return 0;
}