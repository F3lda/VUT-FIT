/**
 * @file processes.h
 * 
 * @brief Functions for working with processes
 * @date 2020-05-06
 * @author F3lda
 */
#ifndef PROCESSES_FORK_H_
#define PROCESSES_FORK_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/wait.h>


int ProcessCreate(int (processFunction)(pid_t, void*), void *userData, bool waitForChild);

void ProcessWaitForAllChildProcesses();

int ProcessWaitForChildProcess(int processID);

int ProcessCheckIfAnyChildProcessFinished();

#endif