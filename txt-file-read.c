#include <stdio.h>
#include <string.h>
#include<stdio.h>
#include<stdio.h>


int main()
{

    //read any text file from currect directory
    char const* const fileName = "img.txt";

    FILE* file = fopen(fileName, "r"); 

    if(!file){
        printf("\n Unable to open : %s ", fileName);
        return -1;
    }

    char delim[] = " ";

    char line[500];

    int pixelData[15];
    int pos = 0;

    while (fgets(line, sizeof(line), file)) {
        printf("---------------------------------------------------- \n");
        printf("LINE ----> %s", line); 
        int init_size = strlen(line);
        char *ptr = strtok(line, delim);
	    while (ptr != NULL)
	    {
	    	printf("Value stored: %s in the position %d\n", ptr,pos);
            int val2Store = atoi(ptr);
            printf("Value taked: %d\n", val2Store);
            pixelData[pos] = val2Store;
            printf("Value already stored: %d\n", pixelData[pos]);
	    	ptr = strtok(NULL, delim);
            //printf("Pos value: %d\n", pos);
            pos++;
	    }
        pos = 0;

        printf("RED: %d \nGREEN: %d \nBLUE: %d \nHEIGHT: %d | WIDTH: %d\n", pixelData[0],pixelData[1],pixelData[2],pixelData[3],pixelData[4]);
        printf("RED: %d \nGREEN: %d \nBLUE: %d \nHEIGHT: %d | WIDTH: %d\n", pixelData[5],pixelData[6],pixelData[7],pixelData[8],pixelData[9]);
        printf("RED: %d \nGREEN: %d \nBLUE: %d \nHEIGHT: %d | WIDTH: %d\n", pixelData[10],pixelData[11],pixelData[12],pixelData[13],pixelData[14]);
        printf("------------------------------------------------------ \n");
    }
    fclose(file);


	return 0;
}