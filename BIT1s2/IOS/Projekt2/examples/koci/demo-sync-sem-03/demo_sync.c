/**************************************************
 * Ukazkove kody do predmetu IOS na FIT VUT.
 * Prace se semafory a sdilenou pameti.
 * Kody jsou poskytnuty tak jak jsou.
 * (C) Radek Koci, 2016
***************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/sem.h>
#include <errno.h>

#define semNAME "/ios-proj2"
#define numSEMAPHORES 1

key_t KEY;

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO */
};

void proc1() {
    int semid;

    semid = semget(KEY, numSEMAPHORES, IPC_CREAT | 0666);

    //sem_wait(sem);
    struct sembuf operations[1];
    operations[0].sem_num = 0;
    operations[0].sem_op = -1;
    operations[0].sem_flg = SEM_UNDO;
    if (semop(semid, operations, 1) < 0) {
        perror("semop: wait");
        exit(2);
    }

    for (int i=0; i<10; i++) {
        fprintf(stdout, "first: %d\n", i);
        usleep(500);
    }
    fprintf(stdout, "first: end\n");
}

void proc2() {
    int semid;

    semid = semget(KEY, numSEMAPHORES, IPC_CREAT | 0666);

    for (int i=0; i<10; i++) {
        fprintf(stdout, "second: %d\n", i);
        // po splneni podminky dovolime dalsimu procesu pokracovat
        if (i==5) {
            // sem_post(sem);
            struct sembuf operations[1];
            operations[0].sem_num = 0;
            operations[0].sem_op = 1;
            operations[0].sem_flg = SEM_UNDO;
            semop(semid, operations, 1);
        }
        usleep(500);
    }
    fprintf(stdout, "second: end\n");
}

//int main(int argc, char *argv[])
int main(void)
{
    int pid;
    int semid;
    pid_t consPID;
    pid_t prodPID;
    setbuf(stdout,NULL);
    setbuf(stderr,NULL);


    // generovani klice
    char* path = getenv("HOME");
    KEY = ftok(path, getpid());

    // vytvorime a inicializujeme semafory
    semid = semget(KEY, numSEMAPHORES, IPC_CREAT | 0666);

    union semun semarg;
    unsigned short values[numSEMAPHORES];
    values[0] = 0;
    semarg.array = values;
    semctl(semid, 0, SETALL, semarg);
    // --

    // systemove volani - vzdy je vhodne overit uspesnost!
    if ((pid = fork()) < 0) {
        perror("fork"); 
        exit(2);
    }

    if (pid == 0) { // child
        proc1();
        exit(0);
    } else { // parent.
        consPID = pid;
        //--
        pid = fork();
        //--
        if (pid == 0) { // child
            proc2();
            exit(0);
        } else { // parent
            prodPID = pid;
        }
    }

    // pockame az vsichni skonci
    waitpid(consPID, NULL, 0);
    waitpid(prodPID, NULL, 0);

    // zrusime zdroje
    semctl(semid, numSEMAPHORES, IPC_RMID, NULL);

    return 0;
}
