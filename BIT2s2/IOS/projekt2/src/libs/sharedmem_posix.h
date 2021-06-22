/**
 * @file sharedmem_posix.h
 * 
 * @brief Functions for working with shared memory - POSIX
 * @date 2021-04-26
 * @author F3lda
 * @update 2021-04-26
 */
#ifndef SHAREDMEM_POSIX_H_
#define SHAREDMEM_POSIX_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <errno.h>
#include <semaphore.h>
#include <sys/fcntl.h> /* For O_* constants */
#include <sys/stat.h>  /* For mode constants */
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>


int sharedmem_create(void **sharedmemDATA, char *name, int size);

int sharedmem_get(void **sharedmemDATA, char *name, int size);

int sharedmem_close(void **sharedmemDATA, int size);

int sharedmem_destroy(char *name);

#endif
