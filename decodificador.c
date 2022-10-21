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


typedef struct item
{
        int pixel;
        char date[40];
        int index;
        int busy;

}item;



int main(){
    int fd;
    long int memSpaces;
    
    fd = shm_open(MEM_NAME, O_RDWR, S_IRUSR | S_IWUSR);  //Como ya existe la memoria compartida, solo hace falta abrirla en modo lectura/escrituara
    struct stat mem_obj;
    fstat(fd, &mem_obj);
    memSpaces = ((mem_obj.st_size)-(sizeof(int)*2))/sizeof(item);

    typedef struct 
    {
        item items[memSpaces];
        int lastWrote;
        int lastRead;
    }items;

    items *sharedMemory = mmap(0, sizeof(items), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    sem_t *semDeco, *semEnco;
    semEnco = sem_open(SEM_ENCO_NAME, 0);
    semDeco = sem_open(SEM_DECO_NAME, 0);
    int index;

    while(1){
        printf("Press enter to continue\n");
        getchar();
        sem_wait(semDeco); //Decrease the available resources 'cause one of them will be taken.

        index = sharedMemory->lastRead; 
        printf("Index = %d\n", index);

        if(index == memSpaces) index = 0;        
        
        printf("Lei en la posicion %d el caracter %d\n",index, sharedMemory->items[index].pixel);
        sharedMemory->items[index].busy = 0; 
        sharedMemory->lastRead = index + 1; 
        sem_post(semEnco); //Increase the available resources 'cause the used in this process was release'
    }
}
