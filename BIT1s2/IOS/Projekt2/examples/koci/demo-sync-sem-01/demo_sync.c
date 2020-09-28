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
#include <sys/mman.h>
#include <semaphore.h>

#define shmSIZE sizeof(sem_t)

pid_t consPID;
pid_t prodPID;


//int main(int argc, char *argv[])
int main(void)
{
    int pid;
    setbuf(stdout,NULL);
    setbuf(stderr,NULL);

    sem_t *sem;
    sem = mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_init(sem, 1, 0);

    // systemove volani - vzdy je vhodne overit uspesnost!
    if ((pid = fork()) < 0) {
        perror("fork"); 
        exit(2);
    }

    if (pid == 0) { // child
        // cekame, az nam druhy proces dovoli pokracovat
        sem_wait(sem);
        for (int i=0; i<10; i++) {
            fprintf(stdout, "first: %d\n", i);
            usleep(500);
        }
        sem_post(sem);
        fprintf(stdout, "first: end\n");
        exit(0);
    } else { // parent.
        consPID = pid;
        //--
        pid = fork();
        //--
        if (pid == 0) { // child
            for (int i=0; i<10; i++) {
                fprintf(stdout, "second: %d\n", i);
                // po splneni podminky dovolime dalsimu procesu pokracovat
                if (i==5) sem_post(sem);
                usleep(500);
            }
            sem_wait(sem);
            fprintf(stdout, "second: end\n");
            exit(0);
        } else { // parent
            prodPID = pid;
        }
    }

    // pockame az vsichni skonci
    waitpid(consPID, NULL, 0);
    waitpid(prodPID, NULL, 0);

    // zrusime zdroje
    sem_destroy(sem);
    munmap(sem, shmSIZE);

    return 0;
}
