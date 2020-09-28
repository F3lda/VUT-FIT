/**
 * @file proj2.c
 * 
 * @brief IOS project 2 - Faneuil Hall Problem
 * @date 2020-05-06
 * @author Karel Jirgl
 */
// Custom libs
#include "libs/semaphores_systemv.h"
#include "libs/semaphores_posix.h"
#include "libs/instrpar.h"
#include "libs/processes.h"

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

// ipcrm -s 65536        - kill semaphore
// ipcs -s               - list semaphores



// Program Arguments
#define ARG_IMM_COUNT 0
#define ARG_IMM_TIME_GENERATING 1
#define ARG_IMM_TIME_CERTIFICATE 3
#define ARG_JUDGE_TIME_OUTSIDE 2
#define ARG_JUDGE_TIME_DECISION 4
// output file name
#define OUTPUT_FILE "proj2.out"

// SystemV semaphores
#define SEM_HASH_FILE "proj2.c"
// counters
#define SEM_KEY_ERROR 192 // if error occure > 1
#define SEM_KEY_ACTION 193 // action number counter
#define SEM_KEY_IMM_IN_NE 194 // immigrant inside counter
#define SEM_KEY_IMM_IN_REG_NC 195 // immigrant inside registered counter
#define SEM_KEY_IMM_IN_COUNT_NB 196 // total immigrants in counter
#define SEM_KEY_IMM_REG_IN_PROGRESS 1951 // number of immigrants in register process
// semaphores
#define SEM_KEY_JUDGE_IN 197


// POSIX semaphores
#define SEMAPHORE_KEY_PRINT "/randomPRINT"
#define SEMAPHORE_KEY_IMM_ENTERS "/randomIMMenters"
#define SEMAPHORE_KEY_IMM_REGS "/randomIMMregs"





// RANDOM NUMBER GENERATOR
void randomize()//at the begining of main
{
    time_t t;
    srand((unsigned) time(&t));
}

int randomBetweenWith(int from, int to)//includes from and to
{
    return from + (rand() % (to-from+1));
}


// SEMAPHORES SystemV custom functions
void CreateSemaphoreSYSTEMV(int key, char *key_name, int value)
{
	int SEMid = SemaphoreCreate(ftok(SEM_HASH_FILE, key), value);
	if(SEMid == -1){
		SEMid = SemaphoreRemove(SEMid);
		if(SEMid < 0){fprintf(stderr, "ERRORCreateSemaphoreSYSTEMV: semaphore remove - %s\n", key_name); errno = 1;}
		SEMid = SemaphoreCreate(ftok(SEM_HASH_FILE, key), value);
		if(SEMid < -1){fprintf(stderr, "ERRORCreateSemaphoreSYSTEMV: create semaphore 2 - %s\n", key_name); errno = 1;}
	} else if(SEMid < -1){fprintf(stderr, "ERRORCreateSemaphoreSYSTEMV: create semaphore - %s\n", key_name); errno = 1;}
}

void DeleteSemaphoreSYSTEMV(int key, char *key_name)
{
	int SEMid = SemaphoreGetId(ftok(SEM_HASH_FILE, key));
	if(SEMid < 0){fprintf(stderr, "ERRORDeleteSemaphoreSYSTEMV: semaphore get id - %s\n", key_name); errno = 1;}
	SEMid = SemaphoreRemove(SEMid);
	if(SEMid < 0){fprintf(stderr, "ERRORDeleteSemaphoreSYSTEMV: semaphore remove - %s\n", key_name); errno = 1;}
}

int GetValueSemaphoreSYSTEMV(int key, char *key_name)
{
	int SEMid = SemaphoreGetId(ftok(SEM_HASH_FILE, key));
	if(SEMid < 0){fprintf(stderr, "ERRORGetValueSemaphoreSYSTEMV: semaphore get id - %s\n", key_name); errno = 1;}
	SEMid = SemaphoreGetValue(SEMid);
	if(SEMid < 0){fprintf(stderr, "ERRORGetValueSemaphoreSYSTEMV: semaphore get value - %s\n", key_name); errno = 1;}
	return SEMid;
}

void PushOperationSemaphoreSYSTEMV(int key, char *key_name, int operation)
{
	int SEMid = SemaphoreGetId(ftok(SEM_HASH_FILE, key));
	if(SEMid < 0){fprintf(stderr, "ERRORPushOperationSemaphoreSYSTEMV: semaphore get id - %s\n", key_name); errno = 1;}
	SEMid = SemaphorePushOperation(SEMid, operation);
	if(SEMid < 0){fprintf(stderr, "ERRORPushOperationSemaphoreSYSTEMV: semaphore push operation - %s\n", key_name); errno = 1;}
}


// SEMAPHORES POSIX custom functions
void CreateSemaphorePOSIX(char *key, char *key_name)
{
	sem_t *semaphore;
	if(semaphore_get(&semaphore, key) == 0){semaphore_delete(semaphore, key);}
	if(semaphore_create(&semaphore, key, 1) < -1){fprintf(stderr, "ERRORCreateSemaphorePOSIX: semaphore create - %s\n", key_name); errno = 1;}
}

void DeleteSemaphorePOSIX(char *key, char *key_name)
{
	sem_t *semaphore;
	if(semaphore_get(&semaphore, key) == 0){semaphore_delete(semaphore, key);}
}


// PRINT MESSAGE
int SEMAPHOREgetNewActionNumber()// use only in SEMAPHOREprint
{
	int value = GetValueSemaphoreSYSTEMV(SEM_KEY_ACTION, "SEM_KEY_ACTION");
	PushOperationSemaphoreSYSTEMV(SEM_KEY_ACTION, "SEM_KEY_ACTION", SEM_INC);
	return value+1;
}

void SEMAPHOREprint(char person, int IMMIGRANT_ID, char *message)
{
	sem_t *semaphore;
	if(semaphore_get(&semaphore, SEMAPHORE_KEY_PRINT) != 0){fprintf(stderr, "ERRORSEMAPHOREprint: semaphore get - SEMAPHORE_KEY_PRINT\n"); return;}
	semaphore_lock_or_wait(semaphore);
	
		FILE *outpu_file = fopen(OUTPUT_FILE, "a");
		if(outpu_file == NULL){
			fprintf(stderr, "ERRORSEMAPHOREprint: cannot open output file!\n"); return;
		} else {
			int newActionNumber = SEMAPHOREgetNewActionNumber();
			
			//if(newActionNumber != 1){fprintf(outpu_file, "\n");}// don't print last empty new line
			
			if(person == 'j'){
				if(strcmp(message, "wants to enter") == 0 || strcmp(message, "finishes") == 0){
					fprintf(outpu_file, "%d	: JUDGE		: %s", newActionNumber, message);
				} else {
					fprintf(outpu_file, "%d	: JUDGE		: %s		: %d	: %d	: %d", newActionNumber, message, GetValueSemaphoreSYSTEMV(SEM_KEY_IMM_IN_NE,"SEM_KEY_IMM_IN_NE"), GetValueSemaphoreSYSTEMV(SEM_KEY_IMM_IN_REG_NC, "SEM_KEY_IMM_IN_REG_NC"), GetValueSemaphoreSYSTEMV(SEM_KEY_IMM_IN_COUNT_NB, "SEM_KEY_IMM_IN_COUNT_NB"));
				}
			} else if(person == 'i'){
				if(strcmp(message, "starts") == 0){
					fprintf(outpu_file, "%d	: IMM %d		: starts", newActionNumber, IMMIGRANT_ID);
				} else {
					fprintf(outpu_file, "%d	: IMM %d		: %s		: %d	: %d	: %d", newActionNumber, IMMIGRANT_ID, message, GetValueSemaphoreSYSTEMV(SEM_KEY_IMM_IN_NE,"SEM_KEY_IMM_IN_NE"), GetValueSemaphoreSYSTEMV(SEM_KEY_IMM_IN_REG_NC, "SEM_KEY_IMM_IN_REG_NC"), GetValueSemaphoreSYSTEMV(SEM_KEY_IMM_IN_COUNT_NB, "SEM_KEY_IMM_IN_COUNT_NB"));
				}
			} else {
				fprintf(stderr, "ERRORSEMAPHOREprint: wrong person type\n"); return;
			}
			
			fprintf(outpu_file, "\n");
			
			fclose(outpu_file);
		}
	
	semaphore_unlock(semaphore);
}


//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

int ARGS[5];


int JUDGE(pid_t processID, void *userData)
{
	errno = 0;
	int IMMIGRANTS_DONE = 0;
	
	do{
		//usleep - the judge is coming to work
		usleep(randomBetweenWith(0, ARGS[ARG_JUDGE_TIME_OUTSIDE])*1000);//milliseconds


		// print wants to enter
		SEMAPHOREprint('j', 0, "wants to enter");
		if(errno != 0){PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC); return 1;}
		

		// lock the door - judge is in
		PushOperationSemaphoreSYSTEMV(SEM_KEY_JUDGE_IN, "SEM_KEY_JUDGE_IN", SEM_INC);
		if(errno != 0){PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC); return 1;}
		
		
		// print enters
		SEMAPHOREprint('j', 0, "enters");
		if(errno != 0){PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC); return 1;}


		//NE != NC - not all immigrants are registered -> wait for them
		if(GetValueSemaphoreSYSTEMV(SEM_KEY_IMM_REG_IN_PROGRESS, "SEM_KEY_IMM_REG_IN_PROGRESS") > 0){
			SEMAPHOREprint('j', 0, "waits for imm");
			PushOperationSemaphoreSYSTEMV(SEM_KEY_IMM_REG_IN_PROGRESS, "SEM_KEY_IMM_REG_IN_PROGRESS", SEM_WAIT);// registration process - wait
		}


		// NE == NC
		// print starts confirmation
		SEMAPHOREprint('j', 0, "starts confirmation");
		if(errno != 0){PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC); return 1;}
		//usleep - the judge decides
		usleep(randomBetweenWith(0, ARGS[ARG_JUDGE_TIME_DECISION])*1000);//milliseconds


		// print ends confirmation
		int currentImmIn = -1*GetValueSemaphoreSYSTEMV(SEM_KEY_IMM_IN_REG_NC, "SEM_KEY_IMM_IN_REG_NC");
		IMMIGRANTS_DONE -= currentImmIn; // add
		PushOperationSemaphoreSYSTEMV(SEM_KEY_IMM_IN_NE, "SEM_KEY_IMM_IN_NE", currentImmIn);
		PushOperationSemaphoreSYSTEMV(SEM_KEY_IMM_IN_REG_NC, "SEM_KEY_IMM_IN_REG_NC", currentImmIn);
		SEMAPHOREprint('j', 0, "ends confirmation");
		if(errno != 0){PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC); return 1;}
		
		
		// print leaves
		//usleep - the judge prepares for the way home
		usleep(randomBetweenWith(0, ARGS[ARG_JUDGE_TIME_DECISION])*1000);//milliseconds
		SEMAPHOREprint('j', 0, "leaves");
		if(errno != 0){PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC); return 1;}
		
		
		// unlock the door
		PushOperationSemaphoreSYSTEMV(SEM_KEY_JUDGE_IN, "SEM_KEY_JUDGE_IN", SEM_DEC);
		if(errno != 0){PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC); return 1;}
		
	}while(IMMIGRANTS_DONE != ARGS[ARG_IMM_COUNT]);

	//print finishes
	SEMAPHOREprint('j', 0, "finishes");
	if(errno != 0){PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC); return 1;}
	
	return 0;
}


int IMMIGRANT(pid_t processID, void *userData)
{
	errno = 0;
	sem_t *semaphore;
	int IMMIGRANT_ID = *((int *)userData);


	// print starts
	SEMAPHOREprint('i', IMMIGRANT_ID, "starts");
	if(errno != 0){PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC); return 1;}
	
	
	if(semaphore_get(&semaphore, SEMAPHORE_KEY_IMM_ENTERS) != 0){fprintf(stderr, "ERRORIMMIGRANT: semaphore get - SEMAPHORE_KEY_IMM_ENTERS\n"); PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC); return 1;}
	semaphore_lock_or_wait(semaphore);
		
		// wait for the judge leaves
		PushOperationSemaphoreSYSTEMV(SEM_KEY_JUDGE_IN, "SEM_KEY_JUDGE_IN", SEM_WAIT);
		PushOperationSemaphoreSYSTEMV(SEM_KEY_IMM_REG_IN_PROGRESS, "SEM_KEY_IMM_REG_IN_PROGRESS", SEM_INC);// registration process - start
		if(errno != 0){PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC); return 1;}
		
		// enter building
		PushOperationSemaphoreSYSTEMV(SEM_KEY_IMM_IN_COUNT_NB, "SEM_KEY_IMM_IN_COUNT_NB", SEM_INC);
		PushOperationSemaphoreSYSTEMV(SEM_KEY_IMM_IN_NE, "SEM_KEY_IMM_IN_NE", SEM_INC);
		SEMAPHOREprint('i', IMMIGRANT_ID, "enters");
		if(errno != 0){PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC); return 1;}
	
	semaphore_unlock(semaphore);
	
	
	// registration
	if(semaphore_get(&semaphore, SEMAPHORE_KEY_IMM_REGS) != 0){fprintf(stderr, "ERRORIMMIGRANT: semaphore get - SEMAPHORE_KEY_IMM_REGS\n"); PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC); return 1;}
	semaphore_lock_or_wait(semaphore);
	
		PushOperationSemaphoreSYSTEMV(SEM_KEY_IMM_IN_REG_NC, "SEM_KEY_IMM_IN_REG_NC", SEM_INC);
		SEMAPHOREprint('i', IMMIGRANT_ID, "checks");
		PushOperationSemaphoreSYSTEMV(SEM_KEY_IMM_REG_IN_PROGRESS, "SEM_KEY_IMM_REG_IN_PROGRESS", SEM_DEC);// registration process - end
		if(errno != 0){PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC); return 1;}
	
	semaphore_unlock(semaphore);
	
	
	// waiting for the judge's decision
	PushOperationSemaphoreSYSTEMV(SEM_KEY_IMM_IN_REG_NC, "SEM_KEY_IMM_IN_REG_NC", SEM_WAIT);//wait for NC == 0
	if(errno != 0){PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC); return 1;}
	
	
	// print wants certificate
	SEMAPHOREprint('i', IMMIGRANT_ID, "wants certificate");
	if(errno != 0){PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC); return 1;}
	//usleep - getting certificate
	usleep(randomBetweenWith(0, ARGS[ARG_IMM_TIME_CERTIFICATE])*1000);//milliseconds
	
	
	// print got certificate
	SEMAPHOREprint('i', IMMIGRANT_ID, "got certificate");
	if(errno != 0){PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC); return 1;}


	// wait for the judge leaves
	PushOperationSemaphoreSYSTEMV(SEM_KEY_JUDGE_IN, "SEM_KEY_JUDGE_IN", SEM_WAIT);
	if(errno != 0){PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC); return 1;}


	// print leaves
	PushOperationSemaphoreSYSTEMV(SEM_KEY_IMM_IN_COUNT_NB, "SEM_KEY_IMM_IN_COUNT_NB", SEM_DEC);
	SEMAPHOREprint('i', IMMIGRANT_ID, "leaves");
	if(errno != 0){PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC); return 1;}

	return 0;
}


int IMMIGRANTS(pid_t processID, void *userData)
{
	int IMMIGRANTS_CREATED = 0;
	int IMMIGRANTS_EXIST = 0;
	int IMMIGRANTS_DONE = 0;
	pid_t pid;
	pid_t all_child[ARGS[ARG_IMM_COUNT]];
	errno = 0;
	do{
		//create immigrant process if not enough
		if(IMMIGRANTS_CREATED < ARGS[ARG_IMM_COUNT]){
			usleep(randomBetweenWith(0, ARGS[ARG_IMM_TIME_GENERATING])*1000);//milliseconds
			int immCout = IMMIGRANTS_CREATED+1;
			if((pid = ProcessCreate(IMMIGRANT, &immCout, false)) <= 0){
				fprintf(stderr, "ERRORpristehovalci: cannot create a new child\n");
				errno = 1;
			} else {
				all_child[IMMIGRANTS_CREATED] = pid;
				IMMIGRANTS_CREATED++;
				IMMIGRANTS_EXIST++;
			}
		}
		
		//check SEM_KEY_ERROR for error
		int errorv = GetValueSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR");
		if(errorv > 0){errno = 1;}
		
		//check for finished immigrant processes
		if(errno == 0){
			pid = ProcessCheckIfAnyChildProcessFinished();
			if(pid != 0){
				IMMIGRANTS_EXIST--;
				IMMIGRANTS_DONE++;
			}
		}
	}while(pid != -1 && errno == 0);
	if(errno != ECHILD){
		//save error
		PushOperationSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", SEM_INC);
		
		//kill all on error
		int i;
		for(i = 0; i < IMMIGRANTS_CREATED; i++){
			kill(all_child[i], SIGTERM);//SIGKILL, SIGTERM
		}
		ProcessWaitForAllChildProcesses();
		errno = 0;
	}

	return 0;
}



int main(int argc, char *argv[])
{
	randomize();
	errno = 0;
	if(argc == 6){
		// Get params
		int i = 0;
		while(i < 5){
			if(sscani(argv[i+1],&ARGS[i]) != 0 || ARGS[i] < 0 || 2000 < ARGS[i]){
				fprintf(stderr, "ERRORmain: wrong argument number [%d] on input!\n", i+1);
				return 1;
			}
			i++;
		}
		
		// Clear or create output file
		fclose(fopen(OUTPUT_FILE, "w"));
		
		//--------------------------------------------------------------------------------------------------------------------
		
		// Create semaphores
		
		//POSIX SEMAPHORES
		CreateSemaphorePOSIX(SEMAPHORE_KEY_PRINT, "SEMAPHORE_KEY_PRINT");
		CreateSemaphorePOSIX(SEMAPHORE_KEY_IMM_ENTERS, "SEMAPHORE_KEY_IMM_ENTERS");
		CreateSemaphorePOSIX(SEMAPHORE_KEY_IMM_REGS, "SEMAPHORE_KEY_IMM_REGS");

		
		//SystemV SEMAPHORES
		CreateSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR", 0); // error counter
		CreateSemaphoreSYSTEMV(SEM_KEY_ACTION, "SEM_KEY_ACTION", 0); // action counter
		CreateSemaphoreSYSTEMV(SEM_KEY_IMM_IN_NE, "SEM_KEY_IMM_IN_NE", 0);
		CreateSemaphoreSYSTEMV(SEM_KEY_IMM_IN_REG_NC, "SEM_KEY_IMM_IN_REG_NC", 0);
		CreateSemaphoreSYSTEMV(SEM_KEY_IMM_IN_COUNT_NB, "SEM_KEY_IMM_IN_COUNT_NB", 0);
		CreateSemaphoreSYSTEMV(SEM_KEY_JUDGE_IN, "SEM_KEY_JUDGE_IN", 0);
		CreateSemaphoreSYSTEMV(SEM_KEY_IMM_REG_IN_PROGRESS, "SEM_KEY_IMM_REG_IN_PROGRESS", 0);
		
		//--------------------------------------------------------------------------------------------------------------------
		
		if(errno == 0){
			//Run processes
			ProcessCreate(IMMIGRANTS, NULL, false);
			ProcessCreate(JUDGE, NULL, false);
			ProcessWaitForAllChildProcesses();
			
			//Handle error
			int errorv = GetValueSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR");
			if(errorv > 0){errno = 1;}
		}
		
		//--------------------------------------------------------------------------------------------------------------------
		
		// Remove semaphores
		
		//POSIX SEMAPHORES
		DeleteSemaphorePOSIX(SEMAPHORE_KEY_PRINT, "SEMAPHORE_KEY_PRINT");
		DeleteSemaphorePOSIX(SEMAPHORE_KEY_IMM_ENTERS, "SEMAPHORE_KEY_IMM_ENTERS");
		DeleteSemaphorePOSIX(SEMAPHORE_KEY_IMM_REGS, "SEMAPHORE_KEY_IMM_REGS");


		//SystemV SEMAPHORES
		DeleteSemaphoreSYSTEMV(SEM_KEY_ERROR, "SEM_KEY_ERROR");
		DeleteSemaphoreSYSTEMV(SEM_KEY_ACTION, "SEM_KEY_ACTION");
		DeleteSemaphoreSYSTEMV(SEM_KEY_IMM_IN_NE, "SEM_KEY_IMM_IN_NE");
		DeleteSemaphoreSYSTEMV(SEM_KEY_IMM_IN_REG_NC, "SEM_KEY_IMM_IN_REG_NC");
		DeleteSemaphoreSYSTEMV(SEM_KEY_IMM_IN_COUNT_NB, "SEM_KEY_IMM_IN_COUNT_NB");
		DeleteSemaphoreSYSTEMV(SEM_KEY_JUDGE_IN, "SEM_KEY_JUDGE_IN");
		DeleteSemaphoreSYSTEMV(SEM_KEY_IMM_REG_IN_PROGRESS, "SEM_KEY_IMM_REG_IN_PROGRESS");
		
		//--------------------------------------------------------------------------------------------------------------------
		
		if(errno == ECHILD){errno = 0;}
		//printf("Something is wrong, I can feel it: %s [%d]\n", strerror(errno), errno);
		//printf("Done.\n");
	} else {
		fprintf(stderr, "ERRORmain: wrong count of arguments!\n");
		return 1;
	}
	return errno;
}