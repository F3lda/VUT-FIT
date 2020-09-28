/**************************************************
 * Ukazkove kody do predmetu IOS na FIT VUT.
 * Prace se semafory a sdilenou pameti.
 * Kody jsou poskytnuty tak jak jsou.
 * (C) Radek Koci, 2016
***************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>

#define shmSIZE sizeof(int)
#define shmKEY "/ios-proj2"

key_t KEY;

void proc1() {
    int shmID;
    int *shm;

    // kontrola uspesnosti !!!
    shmID = shmget(KEY, shmSIZE, IPC_CREAT | 0666);
    shm = (int*)shmat(shmID, NULL, 0);

    for (int i=0; i<10; i++) {
        *shm = *shm + 1;
        fprintf(stdout, "first: %d\n", *shm);
        usleep(500);
    }

    shmdt(shm);
}

void proc2() {
    int shmID;
    int *shm;

    // kontrola uspesnosti !!!
    shmID = shmget(KEY, shmSIZE, IPC_CREAT | 0666);
    shm = (int*)shmat(shmID, NULL, 0);

    for (int i=0; i<10; i++) {
        *shm = *shm + 1;
        fprintf(stdout, "second: %d\n", *shm);
        usleep(500);
    }

    shmdt(shm);
}

//int main(int argc, char *argv[])
int main(void)
{
    int pid;
    int *shm;
    int shmID;
    pid_t consPID;
    pid_t prodPID;
    setbuf(stdout,NULL);
    setbuf(stderr,NULL);

    // generovani klice
    char* path = getenv("HOME");
    KEY = ftok(path, getpid());

    // vytvoreni sdilene pameti - opet overit uspesnost ...
    // misto KEY je mozno pouzit IPC_PRIVATE (pozor na omezeni!)
    if ((shmID = shmget(KEY, shmSIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    // -- inicializace sdilene pameti
    shm = (int*)shmat(shmID, NULL, 0);
    *shm = 20;
    shmdt(shm);
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
    shmctl(shmID, IPC_RMID, NULL);

    return 0;
}
