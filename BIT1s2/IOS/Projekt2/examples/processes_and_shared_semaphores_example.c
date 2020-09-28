#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>



#define SEM_KEY 1931

#define SEM_INC 1
#define SEM_DEC -1
#define SEM_WAIT 0

int SemaphoreCreate(key_t key)
{
	int SEMid = semget(key, 1, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);// key, number of semaphores in array, flags - IPC_CREAT = create new semaphore if doesn't exist; IPC_EXCL = fail if semaphore already exists; S_IRUSR | S_IWUSR = ask for permissions
	if(SEMid >= 0){// semaphore succesfully created
		// Init semaphore
		union semun {
			int val; // Value for SETVAL
			struct semid_ds *buf; // Buffer for IPC_STAT, IPC_SET 
			unsigned short *array; // Array for GETALL, SETALL
		};
		union semun initValue;
		initValue.val = 0; // set value 0
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

int SemaphoreRemove(int semaphoreId)
{
	return semctl(semaphoreId, 0, IPC_RMID, 0);// semaphore id, semaphore number in array [from 0], command, [ignored]
}

int SemaphoreGetId(key_t key)
{
	return semget(key, 1, S_IRUSR | S_IWUSR);// key, number of semaphores in array, permissions
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



int ProcessCreate(int (processFunction)(int), bool waitForChild)// true - returns child exit code or number < 0 for error; false - returns new process id or number < 0 for error
{
	int child_pid;
	child_pid = fork();// create new process
	if(child_pid > 0)// new process created succesfully - [child_pid = new process id]
	{
		//printf("main - new process created succesfully! [%d]\n", child_pid);
		if(waitForChild){
			int status;
			while(wait(&status) != child_pid);// wait for current new process
			//printf("main - child exit code: %d [%d]\n", WEXITSTATUS(status), child_pid);
			return WEXITSTATUS(status);
		}
		return child_pid;
	}
	else if(child_pid == 0)// new process is running - [child_pid = 0]
	{
		exit((processFunction)(getpid()));
	}
	//printf("main - unable to create a new process! [%d]\n", child_pid);
	return child_pid;// failure - unable to create a new process
}

int function1(int processID)
{
	printf("FUNCTION 1 [%d]\n", processID);

	int SEMid = SemaphoreGetId(SEM_KEY);
	if(SEMid < 0){printf("ERROR1: get semaphore\n"); return 1;}
	SEMid = SemaphorePushOperation(SEMid, 10);
	if(SEMid < 0){printf("ERROR1: semaphore push operation\n"); return 1;}
	
	sleep(4);
	
	SEMid = SemaphoreGetId(SEM_KEY);
	if(SEMid < 0){printf("ERROR1: get semaphore\n"); return 1;}
	SEMid = SemaphorePushOperation(SEMid, -10);
	if(SEMid < 0){printf("ERROR1: semaphore push operation\n"); return 1;}
	
	printf("FUNCTION 1 - END [%d]\n", processID);
	return 1;
}

int function2(int processID)
{
	printf("FUNCTION 2 [%d]\n", processID);

	int SEMid = SemaphoreGetId(SEM_KEY);
	if(SEMid < 0){printf("ERROR2: get semaphore\n"); return 1;}

	printf("wait [%d]\n", SemaphoreGetValue(SEMid));
	SEMid = SemaphorePushOperation(SEMid, SEM_WAIT);
	if(SEMid < 0){printf("ERROR2: semaphore push operation\n"); return 1;}
	printf("after wait\n");
	
	printf("FUNCTION 2 - END [%d]\n", processID);
	return 0;
}

int main(int argc, char *argv[])
{
	printf("main - STARTED\n");
	
	int SEMid = SemaphoreCreate(SEM_KEY);// TODO generate key using ftok();
	if(SEMid < 0){printf("ERRORmain: create semaphore\n");}//TODO if exists continue
	
	ProcessCreate(function1, false);
	ProcessCreate(function2, false);
	int child_finished_pid, status;
	while((child_finished_pid = wait(&status)) > 0);// wait for all child processes
	
	SEMid = SemaphoreGetId(SEM_KEY);
	if(SEMid < 0){printf("ERRORmain: get semaphore\n");}
	SEMid = SemaphoreRemove(SEMid);
	if(SEMid < 0){printf("ERRORmain: semaphore remove\n");}
	
	printf("Oh, something went wrong: %s\n", strerror(errno)); 
	printf("main - FINISHED\n");
	return 0;
}



//https://www.fit.vutbr.cz/study/courses/IOS/private/Lab/projekty/projekt2/projekt2.pdf
//
//https://stackoverflow.com/questions/1671336/how-does-one-keep-an-int-and-an-array-in-shared-memory-in-c
//https://www.tutorialspoint.com/how-to-use-posix-semaphores-in-c-language



//processes
//https://www.thegeekstuff.com/2012/03/c-process-control-functions/
//https://stackoverflow.com/questions/23709888/how-to-use-wait-in-c
//https://stackoverflow.com/questions/12304949/how-to-create-a-process-in-c-under-linux


//https://linux.die.net/man/3/execl
//https://stackoverflow.com/questions/5769734/what-are-the-different-versions-of-exec-used-for-in-c-and-c


// ipcrm -s 65536        - kill semaphore
// ipcs -s               - list semaphores



