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
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>

#define semNAME "/ios-proj2"


void proc1() {
    sem_t *sem;

    sem = sem_open(semNAME, O_RDWR);

    sem_wait(sem);
    for (int i=0; i<10; i++) {
        fprintf(stdout, "first: %d\n", i);
        usleep(500);
    }
    fprintf(stdout, "first: end\n");

    sem_close(sem);
}

void proc2() {
    sem_t *sem;

    sem = sem_open(semNAME, O_RDWR);

    for (int i=0; i<10; i++) {
        fprintf(stdout, "second: %d\n", i);
        // po splneni podminky dovolime dalsimu procesu pokracovat
        if (i==5) sem_post(sem);
        usleep(500);
    }
    fprintf(stdout, "second: end\n");

    sem_close(sem);
}

//int main(int argc, char *argv[])
int main(void)
{
    int pid;
    sem_t *sem;
    pid_t consPID;
    pid_t prodPID;
    setbuf(stdout,NULL);
    setbuf(stderr,NULL);

    // vytvorime a inicializujeme semafory
    sem = sem_open(semNAME, O_CREAT, 0666, 0);
    sem_close(sem);

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
    sem_unlink(semNAME);

    return 0;
}
