
// A C/C++ program for splitting a string
// using strtok()
#include <stdio.h>
#include <string.h>
 
#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include <string.h>

int width, height;
png_byte color_type;
png_byte bit_depth;

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


void read_png_file(char *filename) {
  FILE *fp = fopen(filename, "rb");

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png) abort();

  png_infop info = png_create_info_struct(png);
  if(!info) abort();

  if(setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  png_read_info(png, info);

  width      = png_get_image_width(png, info);
  height     = png_get_image_height(png, info);
  color_type = png_get_color_type(png, info);
  bit_depth  = png_get_bit_depth(png, info);
  //printf("PNG %s information:\nWidth: %d \nHeight: %d \nColor Type: %d \nBit depth: %d \n", filename, width, height, color_type, bit_depth);



  if(bit_depth == 16)
    png_set_strip_16(png);

  if(color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png);

  // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
  if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand_gray_1_2_4_to_8(png);

  if(png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);

  // These color_type don't have an alpha channel then fill it with 0xff.
  if(color_type == PNG_COLOR_TYPE_RGB ||
     color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_PALETTE)
     {
        
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

     }

  if(color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png);

  png_read_update_info(png, info);


  //printf("The png_bytep size is: %d bytes.\nNumber of bytes needed for the entire rows \n\n", (int)sizeof(png_bytep));
  //printf("The number of bytes needed for each row is: %d bytes \n", png_get_rowbytes(png,info));
  png_bytep * row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
  for(int y = 0; y < height; y++) {
    row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
  }

  //printf("Row pointer val: %p \n", (png_byte*)row_pointers);

  png_read_image(png, row_pointers);

  item actualPixel;
  // ---------- Codigo dentro del read - Encodificador --------------------------------
  for(int y = 0; y < height; y++) {
      png_bytep row = row_pointers[y];
      for(int x = 0; x < width; x++) {
          png_bytep px = &(row[x * 4]);
            // En la lectura -  Trabajo del encodificador. La variable actualPixel es el struct
            actualPixel.pixel_r = px[0];
            actualPixel.pixel_g = px[1];
            actualPixel.pixel_b = px[2];

            /*
            Aca se usa el codigo de encodificacion que lleva cada pixel a la memoria compartida.              
            */ 
      }
  }

  fclose(fp);

  png_destroy_read_struct(&png, &info, NULL);
}

void write_png_file(char *filename, char *txtFileNameArg) {
    int y;

    FILE *fp = fopen(filename, "wb");
    if(!fp) abort();    

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) abort();

    png_infop info = png_create_info_struct(png);
    if (!info) abort();

    if (setjmp(png_jmpbuf(png))) abort();

    png_init_io(png, fp);

    // Output is 8bit depth, RGBA format.
    png_set_IHDR(
    png,
    info,
    width, height,
    8,
    PNG_COLOR_TYPE_RGBA,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);


    png_bytep *row_pointers_wr = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for(int y = 0; y < height; y++) {
      row_pointers_wr[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
    }
    int borde = height - 10;
    for(int y = 0; y < height; y++) {
        png_bytep row = row_pointers_wr[y];
        for(int x = 0; x < width; x++) {
            png_bytep px = &(row[x * 4]);

            /*
            1. Leer el txt
            char  txtFileName[32] = "";
            strcp(txtFileName,txtFileNameArg);

            FILE* file = fopen(fileName, "r"); 

            if(!file){
                printf("\n Unable to open : %s ", fileName);
                return -1;
            }

            char delim[] = " ";

            char line[500];

            int pixelData[5];
            int pos = 0;
            
            // Loop para iterar en cada pixel dentro del heap
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

                2. Escribir en el heap usando la variable de  px.            
                px[0] = pixelData[0];
                px[1] = pixelData[1];
                px[2] = pixelData[2];            
                px[3] = 255;     

                printf("RED: %d \nGREEN: %d \nBLUE: %d \nHEIGHT: %d | WIDTH: %d\n", pixelData[0],pixelData[1],pixelData[2],pixelData[3],pixelData[4]);
                printf("------------------------------------------------------ \n");
            }
            fclose(file);


            */
       }
    }


    png_write_image(png, row_pointers_wr);
    png_write_end(png, NULL);

    for(int y = 0; y < height; y++) {
      free(row_pointers_wr[y]);
    }
    free(row_pointers_wr);

    fclose(fp);

    png_destroy_write_struct(&png, &info);
}




char *getPngName(char pngFileName[], char pngJustFileName[]){
    //char pngFileName[32];
    //strcpy(pngFileName, pngFile);

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


}

//int main(int argc, char *argv[]) 
int main(int argc, char *argv[])
{
    char pngFileName[32];
    char pngJustFileName[32] = "";

    // Trabajo del Enco
    read_png_file(argv[1]);
    



    // ------------------ Codigo del deco ----------------
    strcpy(pngFileName, argv[1]);
    getPngName(pngFileName, pngJustFileName);
    printf("OutName: %s \n", pngJustFileName);
    char txtFileName[32] = "Hola.png";

    // Trabajo del Deco
    write_png_file(pngJustFileName, txtFileName);

    return 0;
}


