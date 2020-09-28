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
#include <sys/stat.h>
#include <sys/mman.h>

#define shmSIZE sizeof(int)

pid_t consPID;
pid_t prodPID;


//int main(int argc, char *argv[])
int main(void)
{
    int pid;
    int *shm;
    setbuf(stdout,NULL);
    setbuf(stderr,NULL);

    shm = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
//    int mem = 0; shm = &mem;
    *shm = 20;

    // systemove volani - vzdy je vhodne overit uspesnost!
    if ((pid = fork()) < 0) {
        perror("fork"); 
        exit(2);
    }

    if (pid == 0) { // child
        for (int i=0; i<10; i++) {
            *shm = *shm + 1;
            fprintf(stdout, "first: %d\n", *shm);
            usleep(500);
        }
        exit(0);
    } else { // parent.
        consPID = pid;
        //--
        pid = fork();
        //--
        if (pid == 0) { // child
            for (int i=0; i<10; i++) {
                *shm = *shm + 1;
                fprintf(stdout, "second: %d\n", *shm);
                usleep(500);
            }
            exit(0);
        } else { // parent
            prodPID = pid;
        }
    }

    // pockame az vsichni skonci
    waitpid(consPID, NULL, 0);
    waitpid(prodPID, NULL, 0);

    // zrusime zdroje
    munmap(shm, shmSIZE);

    return 0;
}
