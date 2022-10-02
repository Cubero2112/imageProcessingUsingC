#include <sys/mman.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#define MEM_NAME "/Memory_Shared"
#define SEM_ENCO_NAME "/SEM_ENCO"
#define SEM_DECO_NAME "/SEM_DECO"


int main(){

    sem_t *semDeco, *semEnco;

    //semEnco = sem_open(SEM_ENCO_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    semEnco = sem_open(SEM_ENCO_NAME, 0);
    sem_close(semEnco);
    sem_unlink(SEM_ENCO_NAME);

    semDeco = sem_open(SEM_DECO_NAME, 0);
    sem_close(semDeco);
    sem_unlink(SEM_DECO_NAME);

    shm_unlink(MEM_NAME);
}

//gcc -o clean clean.c -pthread


