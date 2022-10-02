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
        int pixel_r;
        int pixel_g;
        int pixel_b;
        int altura; 
        int anchura;
        char name[50];
        char date[40];
        int index;
        int busy;
        int last;

}item;


int main(){    

    char filename[40];
    printf("Ingrese el nombre de la imagen: \n");
    scanf("%s", filename);

    int sizeOfChunk;
    printf("Ingrese la cantidad de pixeles por chunk: \n");
    scanf("%d", &sizeOfChunk);

    int mode;
    printf("Digite 1 si desea que la ejecución sea automatica, 0 para manual: \n");
    scanf("%d", &mode);

    /*int key;
    printf("Introduzca la clave: \n");
    scanf("%d", &key);*/
    
    int fd = shm_open(MEM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR); //Intento crear la memoria compartida. Si ya hay una creada retorna error (-1)

    long int memSpaces;
    
    if(fd == -1){
        fd = shm_open(MEM_NAME, O_RDWR, S_IRUSR | S_IWUSR);  //Como ya existe la memoria compartida, solo hace falta abrirla en modo lectura/escrituara
        struct stat mem_obj;
        fstat(fd, &mem_obj);
        memSpaces = ((mem_obj.st_size)-(sizeof(int)*2))/sizeof(item);
    }else{
        ftruncate(fd, (sizeof(item) * sizeOfChunk) + (sizeof(int)*2));
        memSpaces = sizeOfChunk;
    }

    typedef struct 
    {
        item items[memSpaces];
        int lastWrote;
        int lastRead;
    }items;

    items *sharedMemory = mmap(0, sizeof(items), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    
    sem_t *semDeco, *semEnco;
    semEnco = sem_open(SEM_ENCO_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 10);
    semDeco = sem_open(SEM_DECO_NAME, O_CREAT| O_EXCL, S_IRUSR | S_IWUSR, 0); 

    if (SEM_FAILED == semEnco){
        semEnco = sem_open(SEM_ENCO_NAME, 0);
    }

    if (SEM_FAILED == semDeco){
        semDeco = sem_open(SEM_DECO_NAME, 0);
    }
      

    FILE *fp = fopen(filename, "r");
    char charRead;
    char ch;

    if (fp == NULL)
    {
        printf("Error al abrir el archivo %s\n", filename);
        exit(1);
    }

    getchar();  //Limpia la consola xD

    int foundNext;
    int index;

    while ((charRead = fgetc(fp)) != EOF)
    {
        if(mode == 0){
            printf("Press enter to continue\n");
            getchar();
        }

        sem_wait(semEnco);
        printf("Entre...\n");
        foundNext = 0;
        index = sharedMemory->lastWrote;    
        
        printf("buscando\n");
        while(foundNext == 0){
           
            if(index == memSpaces) index = 0;
            if(sharedMemory->items[index].busy == 0){
                foundNext = 1;
            } 
            else{
                index++;
            }
        }
        printf("Voy a escribir en el %d, el valor %d\n", index, charRead);
        sharedMemory->items[index].pixel = charRead;
        sharedMemory->items[index].busy = 1;
        sharedMemory->items[index].index = index;
        sharedMemory->lastWrote = index;

        sem_post(semDeco);      
    }

    return 0;
}



//gcc -o out encodificador.c -lrt -pthread


//En este directorio se guarda la memoria compartida
// cd /dev/shm -> rm Memory_Shared


