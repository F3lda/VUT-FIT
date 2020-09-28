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
#include <sys/mman.h>

#define shmSIZE sizeof(int)
#define shmKEY "/ios-proj2"


void proc1() {
    int shmID;
    int *shm;

    // kontrola uspesnosti !!!
    shmID = shm_open(shmKEY, O_RDWR, S_IRUSR | S_IWUSR);
    shm = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
    close(shmID);

    for (int i=0; i<10; i++) {
        *shm = *shm + 1;
        fprintf(stdout, "first: %d\n", *shm);
        usleep(500);
    }

    munmap(shm, shmSIZE);
}

void proc2() {
    int shmID;
    int *shm;

    // kontrola uspesnosti !!!
    shmID = shm_open(shmKEY, O_RDWR, S_IRUSR | S_IWUSR);
    shm = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
    close(shmID);

    for (int i=0; i<10; i++) {
        *shm = *shm + 1;
        fprintf(stdout, "second: %d\n", *shm);
        usleep(500);
    }

    munmap(shm, shmSIZE);
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

    shmID = shm_open(shmKEY, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(shmID, shmSIZE);
    shm = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
    close(shmID);
    *shm = 20;
    munmap(shm, shmSIZE);

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
    shm_unlink(shmKEY);

    return 0;
}
