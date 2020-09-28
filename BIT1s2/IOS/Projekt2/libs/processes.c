/**
 * @file processes.c
 * 
 * @brief Functions for working with processes
 * @date 2020-05-06
 * @author F3lda
 */
#include "processes.h"


int ProcessCreate(int (processFunction)(pid_t, void*), void *userData, bool waitForChild)// true - returns child exit code or number < 0 for error; false - returns new process id or number < 0 for error
{
	pid_t child_pid;
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
		exit((processFunction)(getpid(), userData));
	}
	//printf("main - unable to create a new process! [%d]\n", child_pid);
	return child_pid;// failure - unable to create a new process
}


void ProcessWaitForAllChildProcesses()
{
	int child_finished_pid, status;
	while((child_finished_pid = wait(&status)) > 0);// wait for all child processes
}


int ProcessWaitForChildProcess(int processID)
{
	int status;
	return waitpid(processID, &status, 0);
}


int ProcessCheckIfAnyChildProcessFinished()
{
	int status;
	return waitpid(-1, &status, WNOHANG);// >0 - finished child pid; 0 - any state not changed; -1 - no child exists
}