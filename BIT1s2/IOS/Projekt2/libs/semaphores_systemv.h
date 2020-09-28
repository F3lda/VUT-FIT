/**
 * @file semaphores_systemv.h
 * 
 * @brief Functions for working with shared semaphores - System V Semaphores (wait for zero)
 * @date 2020-05-06
 * @author F3lda
 */
#ifndef SEMAPHORES_SYSTEMV_H_
#define SEMAPHORES_SYSTEMV_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <errno.h>
#include <sys/sem.h>
#include <sys/stat.h>

#define SEM_INC 1
#define SEM_DEC -1
#define SEM_WAIT 0
#define SEM_LOCK 1
#define SEM_UNLOCK -1

int SemaphoreCreate(key_t key, int value);

int SemaphoreGetId(key_t key);

int SemaphoreRemove(int semaphoreId);

int SemaphoreGetValue(int semaphoreId);

int SemaphorePushOperation(int semaphoreId, int operation);

#endif