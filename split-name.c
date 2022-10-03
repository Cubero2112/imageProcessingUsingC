
// A C/C++ program for splitting a string
// using strtok()
#include <stdio.h>
#include <string.h>
 
char *getPngName(char pngFile[]){
    char pngFileName[32];
    strcpy(pngFileName, pngFile);
}

//int main(int argc, char *argv[]) 
int main(int argc, char *argv[])
{
    char pngFileName[32];
    char pngJustFileName[32] = "";

    strcpy(pngFileName, argv[1]);

    printf("File name: %s \n", pngFileName);
    printf("File name lenght: %d \n", strlen(pngFileName));


    int justFileName = strlen(pngFileName) - 4;
    printf("Name size: %d \n", justFileName);
    int val;
    for (val=0; val<justFileName;val++){
        printf("Char: %c \n", pngFileName[val]);
        pngJustFileName[val] = pngFileName[val];
    }
    //pngJustFileName[val] = "\0";
    //pngJustFileName[justFileName] = "\0";
    printf("Just the name: %s \n", pngJustFileName);
    strcat(pngJustFileName, "Out.png");
    printf("OutName: %s \n", pngJustFileName);

    return 0;
}


