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
        int canBeRead;
        int last;

}item;


int main(){    

    char filename[40];
    printf("Ingrese el nombre de la imagen: \n");
    scanf("%s", filename);

    // Esta variable representa cuantos caracteres/pixeles quieren ser tomados de la imagen y escritos en la memoria compartida por el proceso en cuestion.
    int sizeOfChunk;
    printf("Ingrese la cantidad de pixeles por chunk: \n");
    scanf("%d", &sizeOfChunk);

    int mode;
    printf("Digite 1 si desea que la ejecución sea automatica, 0 para manual: \n");
    scanf("%d", &mode);

    /*int key;
    printf("Introduzca la clave: \n");
    scanf("%d", &key);*/
    
    int fd = shm_open(MEM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR); //Intento crear la memoria compartida. Si ya hay una creada retorna error (-1). Este metodo crea la memoria compartida y retorna el file descriptor. En este metodo aun no indica la cantidad de memoria que sera compartida. La memoria es creada en modo lectura y escritura.

    long int memSpaces; //Variable usada para conocer cual es la cantidad de memoria que debe de ser compartida. ESta variable solo se asigna en la ejecucion del primer proceso pues este es quien define el tamano de la memoria.
    
    
    // Se ejecuta solo si la memoria compartida ya esta creada.
    if(fd == -1){ // Esta seccion de codigo es necesaria debido a que habran multiples procesos/instancias de este pograma ejecutandose simultaneamente.
        fd = shm_open(MEM_NAME, O_RDWR, S_IRUSR | S_IWUSR);  //Como ya existe la memoria compartida, solo hace falta abrirla en modo lectura/escritura
        struct stat mem_obj; //Struct utilizada para la creacion de la seccion de estadisticas.
        fstat(fd, &mem_obj);
        memSpaces = ((mem_obj.st_size)-(sizeof(int)*2))/sizeof(item);
    }else{ // Se ejecuta solo cuando la memoria es creada. 
        ftruncate(fd, (sizeof(item) * sizeOfChunk) + (sizeof(int)*2)); //Asigna a la memoria compartida el size que debe de tener.
        memSpaces = sizeOfChunk;
    }

    // Se abstrae la memoria compartida. Esta contempla las celdas de memoria (Con la capacidad por celda de almacenar un struct item) y el la ultima de las celdas que ha sido escrita y leida.
    typedef struct 
    {
        item items[memSpaces];
        int lastWrote;
        int lastRead;
    }items;

    // Se crea la representacion de la memoria a nivel de C. Notese como se crea un puntero a tipo de dato items. Este puntero contiene la direccion de memoria donde se encuentra la primera celda de la memoria compartida. Debido a esto tenemos que pasarle file descriptor de la memoria compartida.
    items *sharedMemory = mmap(0, sizeof(items), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    
    sem_t *semDeco, *semEnco; //Crea los semaforos.
    // Los recursos en este contexto son celdas de memoria.
    semEnco = sem_open(SEM_ENCO_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 10); // asigna 10 recursos al semaforo de encodificacion (?)
    semDeco = sem_open(SEM_DECO_NAME, O_CREAT| O_EXCL, S_IRUSR | S_IWUSR, 0); // Asigna cero recursos al semaforo de decodificacion (Inicialmente no hay recursos disponible para realizar decodificaciones)

    if (SEM_FAILED == semEnco){
        semEnco = sem_open(SEM_ENCO_NAME, 0);
    }

    if (SEM_FAILED == semDeco){
        semDeco = sem_open(SEM_DECO_NAME, 0);
    }
      
    // Abre al archivo que contiene lox pixeles de la imagen a manipular.
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

    while ((charRead = fgetc(fp)) != EOF) // La escritura en la memoria (los recursos se da hasta que se de la lectura completa del archivo recien abierto). Gracias al modo en el que la funcion de lectura de archivos se da. El ultimo caracter leido es identificado con un puntero en dicha posicion. Gracias a esto se puede identificar cual caracter leer despues. Este manejo por parte de dicha funcion permite que cada proceso sepa donde continuar la lectura del file.
    {
        if(mode == 0){
            printf("Press enter to continue\n");
            getchar();
        }

        sem_wait(semEnco); // Ejecucion de la zona critica. Acceso a uno de los recursos en caso de que hayan disponibles.
        printf("Entre...\n");
        foundNext = 0;
        index = sharedMemory->lastWrote;    
        
        printf("buscando\n");
        while(foundNext == 0){
           
            if(index == memSpaces) {
                index=0;
            }

            if(sharedMemory->items[index].canBeRead == 0){ // Significa que este recurso no puede ser usado pues aun fue ha sido leido por el decodificador. (Creo)
                foundNext = 1;
            } 
            else{
                index++ // Signica que ya puede ser leido.;
            }
        }
        printf("Voy a escribir en el %d, el valor %d\n", index, charRead);
        sharedMemory->items[index].pixel = charRead;
        sharedMemory->items[index].canBeRead = 1; // Signica que ya puede ser leido.
        sharedMemory->items[index].index = index; // Para que es?
        sharedMemory->lastWrote = index;

        sem_post(semDeco);
    }

    return 0;
}



//gcc -o out encodificador.c -lrt -pthread


//En este directorio se guarda la memoria compartida
// cd /dev/shm -> rm Memory_Shared


