#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include <string.h>

int width, height;
png_byte color_type;
png_byte bit_depth;
//png_bytep *row_pointers = NULL;

void printColorTypes(){
  printf("--------------COLOR TYPES---------------- \n");
  printf("PNG_COLOR_TYPE_GRAY: %d \n",PNG_COLOR_TYPE_GRAY);
  printf("PNG_COLOR_TYPE_RGB: %d \n",PNG_COLOR_TYPE_RGB);
  printf("PNG_COLOR_TYPE_PALETTE: %d \n",PNG_COLOR_TYPE_PALETTE);
  printf("PNG_COLOR_TYPE_GRAY_ALPHA: %d \n",PNG_COLOR_TYPE_GRAY_ALPHA);
  printf("PNG_COLOR_TYPE_RGB_ALPHA: %d \n",PNG_COLOR_TYPE_RGB_ALPHA);
  printf("------------------------------------------- \n");
}


void printColorTypesMasks(){
  printf("--------------COLOR TYPES MASKS---------------- \n");
  printf("PNG_COLOR_MASK_PALETTE: %d \n",PNG_COLOR_MASK_PALETTE);
  printf("PNG_COLOR_MASK_COLOR: %d \n",PNG_COLOR_MASK_COLOR);
  printf("PNG_COLOR_MASK_ALPHA: %d \n",PNG_COLOR_MASK_ALPHA);
  printf("----------------------------------------------- \n");
}



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
  //printColorTypes();
  //printColorTypesMasks();


  // Read any color_type into 8bit depth, RGBA format.

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

  //if (row_pointers) abort();

  //printf("The png_bytep size is: %d bytes.\nNumber of bytes needed for the entire rows \n\n", (int)sizeof(png_bytep));
  //printf("The number of bytes needed for each row is: %d bytes \n", png_get_rowbytes(png,info));
  png_bytep * row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
  for(int y = 0; y < height; y++) {
    row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
  }

  //printf("Row pointer val: %p \n", (png_byte*)row_pointers);

  png_read_image(png, row_pointers);

  // ---------- Codigo dentro del read - Encodificador --------------------------------
  //for(int y = 0; y < height; y++) {
  //    png_bytep row = row_pointers[y];
  //    for(int x = 0; x < width; x++) {
  //        png_bytep px = &(row[x * 4]);
            // En la lectura -  Trabajo del encodificador. La variable pixel es el struct
            //pixel.r = px[0]
            //pixel.g = px[1]
            //pixel.b = px[2]
            // Enco lleva el struct pixel hasta la mem compartida
      //}
  //}

  fclose(fp);

  png_destroy_read_struct(&png, &info, NULL);
}

void write_png_file(char *filename) {
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
            // ---- Aca va el codigo que manipula los pixeles tomados desde el txt ------
            if (y < 10 || y > borde || (y < 40 && y > 30)){
                px[0] = 0;
                px[1] = 0;
                px[2] = 0;
            }
            else{
                px[0] = 255;
                px[1] = 255;
                px[2] = 255;                  
            }
            px[3] = 255;
            // ---- Aca va el codigo que manipula los pixeles tomados desde el txt ------
       }
    }

    //if (!row_pointers_wr) abort();


    png_write_image(png, row_pointers_wr);
    png_write_end(png, NULL);

    for(int y = 0; y < height; y++) {
      free(row_pointers_wr[y]);
    }
    free(row_pointers_wr);

    fclose(fp);

    png_destroy_write_struct(&png, &info);
}

void process_png_file() {
  /*
    int bottomLim = 0;
    int topLim = 255;
    int redModifiedVal = 0;
    int greenModifiedVal = 0;
    int blueModifiedVal = 0;
    for(int y = 0; y < height; y++) {
        png_bytep row = row_pointers[y];
        for(int x = 0; x < width; x++) {
        png_bytep px = &(row[x * 4]);
        // Do something awesome for each pixel here...

        // ---------- Codigo dentro del read - Encodificador --------------------------------
        //for(int y = 0; y < height; y++) {
        //    png_bytep row = row_pointers_img_in[y];
        //    for(int x = 0; x < width; x++) {
        //        png_bytep px = &(row[x * 4]);
        //    }

        // En la lectura -  Trabajo del encodificador. LA variable pixel es el struct
        //pixel.r = px[0]
        //pixel.g = px[1]
        //pixel.b = px[2]

        // Enco lleva el struct pixel hasta la mem compartida



        // ---------- Codigo dentro del write - Decodificador ---------------------------------
        // Leer el file para extraer el width y el height :
        //for(int y = 0; y < height; y++) {
        //    png_bytep row = row_pointers_img_out[y];
        //    for(int x = 0; x < width; x++) {
        //        png_bytep px = &(row[x * 4]);
        //    

                    // En la escritura - Trabajo del Deco
                    // Leer los tres pixels del file:
                    // pr = fle.read(0)
                    // pg = fle.read(1)
                    // pb = fle.read(2)

                    // Aplicar a los pixeles pr,pg y pb la llave
                    // px[0] = pr && llave
                    // px[1] = pg && llave
                    // px[2] = pb && llave
             //}
        // Luego de colocar en el heap los valores que se quieren escribir en el png salgo de los for's
        // png_write_png(png, row_pointers_img_out)
        // ----------------------------------------------


            //if (px[0] > 200 && px[2] > 200 && px[2] > 200){
            // px[0] = 0;
            // px[1] = 0;
            // px[2] = 0;
            //}

        //printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
        }
    }
    */
}

int main(int argc, char *argv[]) {
  if(argc != 3) abort();

  char outFileName[50];
  strcpy(outFileName,argv[1]);
  strcat(outFileName, "Out.png");

  read_png_file(argv[1]);

  //process_png_file();

  write_png_file(outFileName);

  return 0;
}





