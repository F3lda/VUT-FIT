/**
 * @file proj2.c
 * 
 * @brief IOS project 2 - Santa Claus problem
 * @date 2021-04-27
 * @author F3lda
 * @update 2021-04-29
 */

// Libs
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

// Custom libs
#include "libs/semaphores_systemv.h"
#include "libs/semaphores_posix.h"
#include "libs/sharedmem_posix.h"
#include "libs/instrpar.h"
#include "libs/processes.h"


// ipcrm -s 65536        - kill semaphore
// ipcs -s               - list semaphores
// ps -u xlogin00        - list all processes
// pkill -f proj2        - kill all processes proj2



// output file name
#define OUTPUT_FILE "proj2.out"

// program args
#define ARG_COUNT 4
int ARGS[ARG_COUNT] = {-1};
#define ARG_ELF_COUNT 0
#define ARG_DEER_COUNT 1
#define ARG_MAX_TIME_ELF_WORKS 2
#define ARG_MAX_TIME_DERR_VAC 3



void exitFunction();

void randomize();// at the begining of main

int randomBetweenWith(int from, int to);// includes from and to

int procFunc(pid_t processID, void *userData);


int SANTA(pid_t processID, void *userData);
int ELF(pid_t processID, void *userData);
int REINDEER(pid_t processID, void *userData);

void checkError(int error);

void printMessage(char * entity, int entity_id, char * message);



#define WAKEUP_ENTITY_ELF 1
#define WAKEUP_ENTITY_REINDEER 2

// generators
#define GEN_HASH_FILE "proj2"
#define GEN_KEY(key) (ftok(GEN_HASH_FILE, key))

// shared mems
#define SHAMEM_ERRORS "/MEM-IOS-2-rand-errors"
#define SHAMEM_MESSAGE_NUMBER "/MEM-IOS-2-rand-message-number"
#define SHAMEM_SANTA_WORKS_FOR "/MEM-IOS-2-santa-works-for"
#define SHAMEM_REINDEERS_HOME "/MEM-IOS-2-reindeers-home"
#define SHAMEM_HOLIDAYS_NOW "/MEM-IOS-2-holidays-now"

// semaphores system v
#define SEMSYS_ELVES_NEED_HELP 126346
#define SEMSYS_SANTA_HELP_DONE 654321

// semaphores posix
#define SEMPOS_PRINT_MESSAGE "/POS-IOS-2-print-message"
#define SEMPOS_ELF_NEW_IN_QUEUE "/POS-IOS-2-elf-new-in-queue"
#define SEMPOS_ELVES_HELP_IN_PROGRESS "/POS-IOS-2-elves-help-in-progress"
#define SEMPOS_SANTA_IS_BUSY "/POS-IOS-2-santa-is-busy"
#define SEMPOS_SANTA_IS_SLEEPING "/POS-IOS-2-santa-is-sleeping"
#define SEMPOS_SANTA_STARTS_HELPING "/POS-IOS-2-santa-starts-helping"
#define SEMPOS_CHECK_HOLIDAYS "/POS-IOS-2-check-holidays"
#define SEMPOS_REINDEER_RETURNED_ADD "/POS-IOS-2-reindeer-returned-add"
#define SEMPOS_SANTA_HITCH_REINDEERS "/POS-IOS-2-santa-hitch-reindeers"
#define SEMPOS_REINDEERS_ALL_HITCHED "/POS-IOS-2-reindeers-all-hitched"



int main(int argc, char *argv[])
{
    // register the termination function
    if (atexit(exitFunction) != 0) {
        perror("EXIT FUNCTION");
        exit(EXIT_FAILURE);
    }
    // randomize RNG
    randomize();
    // clear errno
    errno = 0;
    
    
    
    // PARSE ARGS
    // -------------------------
	if (argc != ARG_COUNT+1) {
        fprintf(stderr, "ERROR main(): wrong count of arguments!\nusage: %s <NE> <NR> <TE> <TR>\n0<NE<1000\n0<NR<20\n0<=TE<=1000\n0<=RE<=1000\nE.g.: %s 5 4 100 100\n", argv[0], argv[0]);
		exit(EXIT_FAILURE);
    }
    // get args
    int i = -1;
    while (++i < ARG_COUNT)
    {
        if (sscani(argv[i+1], &ARGS[i]) != 0 || 
            (i == ARG_ELF_COUNT && !(0 < ARGS[i] && ARGS[i] < 1000)) || 
            (i == ARG_DEER_COUNT && !(0 < ARGS[i] && ARGS[i] < 20)) || 
            (i == ARG_MAX_TIME_ELF_WORKS && !(0 <= ARGS[i] && ARGS[i] <= 1000)) || 
            (i == ARG_MAX_TIME_DERR_VAC && !(0 <= ARGS[i] && ARGS[i] <= 1000))
            ) {
            
            fprintf(stderr, "ERROR main(): invalid argument [%d] on input!\n", i+1);
            exit(EXIT_FAILURE);
        }
    }
    
    
    
    // CREATE SHARED MEMs and SEMAPHORES
    // -------------------------
    long int *temp_long_int = NULL;
    int *temp_int = NULL;
    sem_t *semaphore;
    
    // CREATE SHARED MEMs
    sharedmem_create((void**)&temp_long_int, SHAMEM_ERRORS, sizeof(long int));
    (*temp_long_int) = 0;
    sharedmem_close((void**)&temp_long_int, sizeof(long int));
    sharedmem_create((void**)&temp_long_int, SHAMEM_MESSAGE_NUMBER, sizeof(long int));
    (*temp_long_int) = 0;
    sharedmem_close((void**)&temp_long_int, sizeof(long int));
    sharedmem_create((void**)&temp_int, SHAMEM_SANTA_WORKS_FOR, sizeof(int));
    (*temp_int) = 0;
    sharedmem_close((void**)&temp_int, sizeof(int));
    sharedmem_create((void**)&temp_int, SHAMEM_REINDEERS_HOME, sizeof(int));
    (*temp_int) = ARGS[ARG_DEER_COUNT];
    sharedmem_close((void**)&temp_int, sizeof(int));
    sharedmem_create((void**)&temp_int, SHAMEM_HOLIDAYS_NOW, sizeof(int));
    (*temp_int) = 0;
    sharedmem_close((void**)&temp_int, sizeof(int));
    
    // CREATE SEMAPHORES - SYSTEM V
    SemaphoreCreate(GEN_KEY(SEMSYS_ELVES_NEED_HELP), 3);
    SemaphoreCreate(GEN_KEY(SEMSYS_SANTA_HELP_DONE), 3);
    
    // CREATE SEMAPHORES - POSIX
    semaphore_create(&semaphore, SEMPOS_PRINT_MESSAGE, 1); semaphore_close(semaphore);
    semaphore_create(&semaphore, SEMPOS_ELF_NEW_IN_QUEUE, 1); semaphore_close(semaphore);
    semaphore_create(&semaphore, SEMPOS_ELVES_HELP_IN_PROGRESS, 3); semaphore_close(semaphore);
    semaphore_create(&semaphore, SEMPOS_SANTA_IS_BUSY, 1); semaphore_close(semaphore);
    semaphore_create(&semaphore, SEMPOS_SANTA_IS_SLEEPING, 0); semaphore_close(semaphore);
    semaphore_create(&semaphore, SEMPOS_SANTA_STARTS_HELPING, 0); semaphore_close(semaphore);
    semaphore_create(&semaphore, SEMPOS_SANTA_HITCH_REINDEERS, 0); semaphore_close(semaphore);
    semaphore_create(&semaphore, SEMPOS_REINDEER_RETURNED_ADD, 1); semaphore_close(semaphore);
    semaphore_create(&semaphore, SEMPOS_REINDEERS_ALL_HITCHED, 0); semaphore_close(semaphore);
    semaphore_create(&semaphore, SEMPOS_CHECK_HOLIDAYS, 1); semaphore_close(semaphore);
    
    
    
    // RUN PROCESSES
    // -------------------------
    if (errno == 0) {
        
        // create santa
        ProcessCreate(SANTA, NULL, false);
        
        // create elves
        i = 0;
        while(i++ < ARGS[ARG_ELF_COUNT])
        {
            ProcessCreate(ELF, &i, false);
        }
        
        // create reindeers
        i = 0;
        while(i++ < ARGS[ARG_DEER_COUNT])
        {
            ProcessCreate(REINDEER, &i, false);
        }
        ProcessWaitForAllChildProcesses();
        errno = 0;
        
        
        
        // check errors
        long int *error;
        sharedmem_get((void**)&error, SHAMEM_ERRORS, sizeof(long int));
        if (*error != 0) {
            errno = EXIT_FAILURE;
            fprintf(stderr, "ERROR main(): error occurred in processes!\n");
        }
        sharedmem_close((void**)&error, sizeof(long int));
        
    }
    
    
    
    // FREE SHARED MEMs and SEMAPHORES
    // -------------------------
    
    // FREE SHARED MEMs
    sharedmem_destroy(SHAMEM_ERRORS);
    sharedmem_destroy(SHAMEM_MESSAGE_NUMBER);
    sharedmem_destroy(SHAMEM_SANTA_WORKS_FOR);
    sharedmem_destroy(SHAMEM_REINDEERS_HOME);
    sharedmem_destroy(SHAMEM_HOLIDAYS_NOW);
    
    // DESTROY SEMAPHORES - SYSTEM V
    SemaphoreDestroy(SemaphoreGetId(GEN_KEY(SEMSYS_ELVES_NEED_HELP)));
    SemaphoreDestroy(SemaphoreGetId(GEN_KEY(SEMSYS_SANTA_HELP_DONE)));
    
    // DESTROY SEMAPHORES - POSIX
    semaphore_destroy(SEMPOS_PRINT_MESSAGE);
    semaphore_destroy(SEMPOS_ELF_NEW_IN_QUEUE);
    semaphore_destroy(SEMPOS_ELVES_HELP_IN_PROGRESS);
    semaphore_destroy(SEMPOS_SANTA_IS_BUSY);
    semaphore_destroy(SEMPOS_SANTA_IS_SLEEPING);
    semaphore_destroy(SEMPOS_SANTA_STARTS_HELPING);
    semaphore_destroy(SEMPOS_SANTA_HITCH_REINDEERS);
    semaphore_destroy(SEMPOS_REINDEER_RETURNED_ADD);
    semaphore_destroy(SEMPOS_REINDEERS_ALL_HITCHED);
    semaphore_destroy(SEMPOS_CHECK_HOLIDAYS);
    
    
    return errno;
}


int SANTA(pid_t processID, void *userData)
{
    sem_t *semaphore;
    int SEMid;
    
    
    // randomize RNG
    randomize();
    
    
    while(1)
    {
        // SANTA is going to sleep
        printMessage("Santa", 0, "going to sleep");
        
        
        // SANTA is sleeping
        semaphore_get(&semaphore, SEMPOS_SANTA_IS_SLEEPING);
        semaphore_lock_or_wait(semaphore);
        semaphore_close(semaphore);
        
        
        // SANTA is going to work for
        int *santaWorksFor, SantaWorksFor = 0;
        sharedmem_get((void**)&santaWorksFor, SHAMEM_SANTA_WORKS_FOR, sizeof(int));
        SantaWorksFor = (*santaWorksFor); // get value
        sharedmem_close((void**)&santaWorksFor, sizeof(int));
        
        
        if (SantaWorksFor == WAKEUP_ENTITY_ELF) { // ELVES
            
            // SANTA is helping elves
            printMessage("Santa", 0, "helping elves");
            
            
            // SANTA is helping (3 ELVES)
            semaphore_get(&semaphore, SEMPOS_SANTA_STARTS_HELPING);
            semaphore_unlock(semaphore);
            semaphore_unlock(semaphore);
            semaphore_unlock(semaphore);
            semaphore_close(semaphore);
            
            
            // wait until help is done
            errno = 0;
            SEMid = SemaphoreGetId(GEN_KEY(SEMSYS_SANTA_HELP_DONE));
            checkError(0);
            SemaphorePushOperation(SEMid, SEM_WAIT);
            checkError(0);
            
            
            // clear help done
            SEMid = SemaphoreGetId(GEN_KEY(SEMSYS_SANTA_HELP_DONE));
            checkError(0);
            SemaphorePushOperation(SEMid, 3);
            checkError(0);
            
            
            // unlock help for the other ELVES (3)
            semaphore_get(&semaphore, SEMPOS_ELVES_HELP_IN_PROGRESS);
            semaphore_unlock(semaphore);
            semaphore_unlock(semaphore);
            semaphore_unlock(semaphore);
            semaphore_close(semaphore);
            
            
            // SANTA is not busy
            semaphore_get(&semaphore, SEMPOS_SANTA_IS_BUSY);
            semaphore_unlock(semaphore);
            semaphore_close(semaphore);
            
        } else { // REINDEERS
            break;
        }
        
    }
    
    
    printMessage("Santa", 0, "closing workshop");
    
    
    
    
    
    // set holidays now
    semaphore_get(&semaphore, SEMPOS_CHECK_HOLIDAYS);
    semaphore_lock_or_wait(semaphore);
        
        // set holidays now
        int *holidaysNow;
        sharedmem_get((void**)&holidaysNow, SHAMEM_HOLIDAYS_NOW, sizeof(int));
        (*holidaysNow) = 1; // set value
        sharedmem_close((void**)&holidaysNow, sizeof(int));
        
    semaphore_unlock(semaphore);
    semaphore_close(semaphore);
    
    
    // unlock 3 ELVES in QUEUE
    semaphore_get(&semaphore, SEMPOS_SANTA_IS_BUSY);
    semaphore_unlock(semaphore);
    semaphore_close(semaphore);
    
    semaphore_get(&semaphore, SEMPOS_SANTA_STARTS_HELPING);
    semaphore_unlock(semaphore);
    semaphore_unlock(semaphore);
    semaphore_unlock(semaphore);
    semaphore_close(semaphore);
    
    // unlock all other ELVES in QUEUE
    semaphore_get(&semaphore, SEMPOS_ELVES_HELP_IN_PROGRESS);
    semaphore_unlock(semaphore);
    semaphore_close(semaphore);
    
    
    
    
    
    // start hitching RDs
    semaphore_get(&semaphore, SEMPOS_SANTA_HITCH_REINDEERS);
    semaphore_unlock(semaphore);
    semaphore_close(semaphore);
    
    
    // waint until all RDs are hitched
    semaphore_get(&semaphore, SEMPOS_REINDEERS_ALL_HITCHED);
    semaphore_lock_or_wait(semaphore);
    semaphore_close(semaphore);
    
    
    printMessage("Santa", 0, "Christmas started");
    
    
    return 0;
}


int ELF(pid_t processID, void *userData)
{
    int ELF_ID = *((int *)userData);
    sem_t *semaphore;
    int SEMid;
    int *holidaysNow, HolidaysNow = 0;
    
    
    // randomize RNG
    randomize();
    
    
    // ELF started
    printMessage("Elf", ELF_ID, "started");
    
    
    while(1)
    {
        // ELF works alone
        usleep(randomBetweenWith(0, ARGS[ARG_MAX_TIME_ELF_WORKS])*1000);
        
        
        // ELF need help
        printMessage("Elf", ELF_ID, "need help");
        
        
        // new ELF in QUEUE
        semaphore_get(&semaphore, SEMPOS_ELF_NEW_IN_QUEUE);
        semaphore_lock_or_wait(semaphore);
        semaphore_close(semaphore);
        
            // help in progress for other ELVES
            semaphore_get(&semaphore, SEMPOS_ELVES_HELP_IN_PROGRESS);
            semaphore_lock_or_wait(semaphore);
            semaphore_close(semaphore);
            
            
            
            // check HOLIDAYS now
            semaphore_get(&semaphore, SEMPOS_CHECK_HOLIDAYS);
            semaphore_lock_or_wait(semaphore);
                
                // check holidays now
                sharedmem_get((void**)&holidaysNow, SHAMEM_HOLIDAYS_NOW, sizeof(int));
                HolidaysNow = (*holidaysNow);
                sharedmem_close((void**)&holidaysNow, sizeof(int));
                
                
                if (HolidaysNow == 1) {
                    
                    // holidays
                    semaphore_unlock(semaphore);
                    semaphore_close(semaphore);
                    
                    // UNLOCK NEXT ELF
                    
                    // unlock SEMPOS_ELVES_HELP_IN_PROGRESS
                    semaphore_get(&semaphore, SEMPOS_ELVES_HELP_IN_PROGRESS);
                    semaphore_unlock(semaphore);
                    semaphore_close(semaphore);
                    
                    // unlock SEMPOS_ELF_NEW_IN_QUEUE
                    semaphore_get(&semaphore, SEMPOS_ELF_NEW_IN_QUEUE);
                    semaphore_unlock(semaphore);
                    semaphore_close(semaphore);
                    
                    break;
                }
                
            semaphore_unlock(semaphore);
            semaphore_close(semaphore);
            
            
            
            // check if 3rd elf in queue
            int semValue = 0;
            semaphore_get(&semaphore, SEMPOS_ELVES_HELP_IN_PROGRESS);
            semaphore_get_value(semaphore, &semValue);
            if (semValue == 0) { // yes
                semaphore_close(semaphore);
                
                
                // check if SANTA is busy
                semaphore_get(&semaphore, SEMPOS_SANTA_IS_BUSY);
                semaphore_lock_or_wait(semaphore);
                semaphore_close(semaphore);
                
                
                // set SANTA works for
                int *santaWorksFor;
                sharedmem_get((void**)&santaWorksFor, SHAMEM_SANTA_WORKS_FOR, sizeof(int));
                (*santaWorksFor) = WAKEUP_ENTITY_ELF; // FOR ELVES
                sharedmem_close((void**)&santaWorksFor, sizeof(int));
                
                
                // wake up SANTA
                semaphore_get(&semaphore, SEMPOS_SANTA_IS_SLEEPING);
                semaphore_unlock(semaphore);
                semaphore_close(semaphore);
                
            } else { // no
                semaphore_close(semaphore);
            }
        
        // open queue for another ELF
        semaphore_get(&semaphore, SEMPOS_ELF_NEW_IN_QUEUE);
        semaphore_unlock(semaphore);
        semaphore_close(semaphore);
        
        
        // wait for SANTA's help
        semaphore_get(&semaphore, SEMPOS_SANTA_STARTS_HELPING);
        semaphore_lock_or_wait(semaphore);
        semaphore_close(semaphore);
        
        
        
        // check HOLIDAYS now
        semaphore_get(&semaphore, SEMPOS_CHECK_HOLIDAYS);
        semaphore_lock_or_wait(semaphore);
            
            // check holidays now
            sharedmem_get((void**)&holidaysNow, SHAMEM_HOLIDAYS_NOW, sizeof(int));
            HolidaysNow = (*holidaysNow);
            sharedmem_close((void**)&holidaysNow, sizeof(int));
            
            if (HolidaysNow == 1) {
                
                // holidays
                semaphore_unlock(semaphore);
                semaphore_close(semaphore);
                
                break;
            }
            
        semaphore_unlock(semaphore);
        semaphore_close(semaphore);
        
        
        
        // ELF gets help
        printMessage("Elf", ELF_ID, "get help");
        
        
        // ELF gets help from SANTA
        errno = 0;
        SEMid = SemaphoreGetId(GEN_KEY(SEMSYS_SANTA_HELP_DONE));
        checkError(0);
        SemaphorePushOperation(SEMid, SEM_DEC);
        checkError(0);
    }
    
    
    
    // ELF taking holidays
    printMessage("Elf", ELF_ID, "taking holidays");
    
    
    return 0;
}


int REINDEER(pid_t processID, void *userData)
{
    int REINDEER_ID = *((int *)userData);
    sem_t *semaphore;
    
    // randomize RNG
    randomize();
    
    
    // REINDEER started
    printMessage("RD", REINDEER_ID, "rstarted");
    
    
    // REINDEER on vac
    usleep(randomBetweenWith(ARGS[ARG_MAX_TIME_DERR_VAC]/2, ARGS[ARG_MAX_TIME_DERR_VAC])*1000);
    
    
    // REINDEER home
    printMessage("RD", REINDEER_ID, "return home");
    
    
    // new RD returned home
    semaphore_get(&semaphore, SEMPOS_REINDEER_RETURNED_ADD);
    semaphore_lock_or_wait(semaphore);
    semaphore_close(semaphore);
        
        
        // read number of RDs home
        int *deerCount, deersHome = -1;
        sharedmem_get((void**)&deerCount, SHAMEM_REINDEERS_HOME, sizeof(int));
        (*deerCount)--; // new RD home
        deersHome = (*deerCount); // get current value
        sharedmem_close((void**)&deerCount, sizeof(int));
        
        
        // check if last RD is home
        if (deersHome == 0) {
            
            // check if SANTA is busy
            semaphore_get(&semaphore, SEMPOS_SANTA_IS_BUSY);
            semaphore_lock_or_wait(semaphore);
            semaphore_close(semaphore);
            
            
            // set SANTA works for
            int *santaWorksFor;
            sharedmem_get((void**)&santaWorksFor, SHAMEM_SANTA_WORKS_FOR, sizeof(int));
            (*santaWorksFor) = WAKEUP_ENTITY_REINDEER; // FOR REINDEERS
            sharedmem_close((void**)&santaWorksFor, sizeof(int));
            
            
            // wake up SANTA
            semaphore_get(&semaphore, SEMPOS_SANTA_IS_SLEEPING);
            semaphore_unlock(semaphore);
            semaphore_close(semaphore);
        }
        
        
    // open queue for new RD
    semaphore_get(&semaphore, SEMPOS_REINDEER_RETURNED_ADD);
    semaphore_unlock(semaphore);
    semaphore_close(semaphore);
    
    
    // wait for SANTA to close workshop and start hitching REINDEERS
    semaphore_get(&semaphore, SEMPOS_SANTA_HITCH_REINDEERS);
    semaphore_lock_or_wait(semaphore);
    semaphore_close(semaphore);
        
        
        // RD get hitched
        printMessage("RD", REINDEER_ID, "get hitched");
        
        
        // set/get number of hitched RDs
        int *numberOfHitchedRDs;
        sharedmem_get((void**)&numberOfHitchedRDs, SHAMEM_REINDEERS_HOME, sizeof(int));
        (*numberOfHitchedRDs)++; // add hitched RD
        
        
        // if last RD is hitched
        if ((*numberOfHitchedRDs) == ARGS[ARG_DEER_COUNT]) {
            
            // start Christmas
            semaphore_get(&semaphore, SEMPOS_REINDEERS_ALL_HITCHED);
            semaphore_unlock(semaphore);
            semaphore_close(semaphore);
        }
        
        
        sharedmem_close((void**)&numberOfHitchedRDs, sizeof(int));
        
        
    // hitch next RD
    semaphore_get(&semaphore, SEMPOS_SANTA_HITCH_REINDEERS);
    semaphore_unlock(semaphore);
    semaphore_close(semaphore);
    
    
    return 0;
}


void printMessage(char * entity, int entity_id, char * message)
{
    sem_t *semaphore;
    semaphore_get(&semaphore, SEMPOS_PRINT_MESSAGE);
    semaphore_lock_or_wait(semaphore);
        
        
        // get new message number
        long int *messageNumberMEM, messageNumber = 0;
        sharedmem_get((void**)&messageNumberMEM, SHAMEM_MESSAGE_NUMBER, sizeof(long int));
        (*messageNumberMEM)++;
        messageNumber = (*messageNumberMEM);
        sharedmem_close((void**)&messageNumberMEM, sizeof(long int));
        
        
        // open file to write
        FILE *outpu_file;
        if (messageNumber == 1) {
            outpu_file = fopen(OUTPUT_FILE, "w");
        }else {
            outpu_file = fopen(OUTPUT_FILE, "a");
        }
        
        
		if(outpu_file == NULL){
			fprintf(stderr, "printMessage: cannot open output file!\n");
            checkError(1);
		} else {
			
			//if(messageNumber != 1){fprintf(outpu_file, "\n");}// don't print last empty new line
			
            if (entity[0] == 'S') {
                fprintf(outpu_file, "%ld: %s: %s", messageNumber, entity, message);
                // TODO hide
                //printf("%ld: %s: %s\n", messageNumber, entity, message);
            } else {
                fprintf(outpu_file, "%ld: %s %d: %s", messageNumber, entity, entity_id, message);
                // TODO hide
                //printf("%ld: %s %d: %s\n", messageNumber, entity, entity_id, message);
            }
            
			fprintf(outpu_file, "\n");
			
			fclose(outpu_file);
		}
    
    semaphore_unlock(semaphore);
    semaphore_close(semaphore);
}


void checkError(int error)
{
    // check error flag (and exit)
    long int *errorMem;
    sharedmem_get((void**)&errorMem, SHAMEM_ERRORS, sizeof(long int));
    if ((*errorMem) != 0) {
        sharedmem_close((void**)&errorMem, sizeof(long int));
        exit(EXIT_FAILURE);
    }
    sharedmem_close((void**)&errorMem, sizeof(long int));
    // set error flag and exit
    if (error != 0 || errno != 0) {
        sharedmem_get((void**)&errorMem, SHAMEM_ERRORS, sizeof(long int));
        (*errorMem) = 1;
        sharedmem_close((void**)&errorMem, sizeof(long int));
        exit(EXIT_FAILURE);
    }
}


void exitFunction()
{
    //printf("PROCESS DONE.\n");
}

// RANDOM NUMBER GENERATOR
void randomize()// at the begining of main
{
    //srand(time(NULL));
    srand(getpid());
}

int randomBetweenWith(int from, int to)// includes from and to
{
    return from + (rand() % (to-from+1));
}
