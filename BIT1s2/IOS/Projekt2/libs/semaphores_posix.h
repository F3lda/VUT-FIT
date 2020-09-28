/**
 * @file semaphores_posix.h
 * 
 * @brief Functions for working with shared semaphores - POSIX Semaphores (wait if zero)
 * @date 2020-05-06
 * @author F3lda
 */
#ifndef SEMAPHORES_POSIX_H_
#define SEMAPHORES_POSIX_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <errno.h>
#include <semaphore.h>
#include <sys/fcntl.h> /* For O_* constants */
#include <sys/stat.h>  /* For mode constants */


int semaphore_create(sem_t **semaphore, char *name, int value);

int semaphore_get(sem_t **semaphore, char *name);

int semaphore_delete(sem_t *semaphore, char *name);

int semaphore_get_value(sem_t *semaphore, int *value);

int semaphore_lock_or_wait(sem_t *semaphore);

int semaphore_unlock(sem_t *semaphore);

#endif