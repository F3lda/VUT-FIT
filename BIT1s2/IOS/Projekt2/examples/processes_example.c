#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int ProcessCreate(int (processFunction)(int), bool waitForChild)// true - returns child exit code or number < 0 for error; false - returns new process id or number < 0 for error
{
	int child_pid;
	child_pid = fork();// create new process
	if(child_pid > 0)// new process created succesfully - [child_pid = new process id]
	{
		printf("main - new process created succesfully! [%d]\n", child_pid);
		if(waitForChild){
			int status;
			while(wait(&status) != child_pid);// wait for current new process
			printf("main - child exit code: %d [%d]\n", WEXITSTATUS(status), child_pid);
			return WEXITSTATUS(status);
		}
		return child_pid;
	}
	else if(child_pid == 0)// new process is running - [child_pid = 0]
	{
		exit((processFunction)(getpid()));
	}
	printf("main - unable to create a new process! [%d]\n", child_pid);
	return child_pid;// failure - unable to create a new process
}

int function1(int processID)
{
	printf("FUNCTION 1 [%d]\n", processID);
	sleep(3);
	return 1;
}

int function2(int processID)
{
	printf("FUNCTION 2 [%d]\n", processID);
	return 0;
}

int main()
{
	printf("main - STARTED\n");
	ProcessCreate(function1, true);
	ProcessCreate(function2, false);
	int child_finished_pid, status;
	while((child_finished_pid = wait(&status)) > 0);// wait for all child processes
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




