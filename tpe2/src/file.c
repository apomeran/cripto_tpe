#include <stdio.h>
#include "file.h"
#include "common.h"
#include <string.h>

image_t* read_image(const char* filename) {
    int byte_ammount = -1;
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error while reading BMP Image \n");
        return NULL;
    }
    image_t* img = malloc(sizeof(struct tagImage));

	img->id = calloc(strlen(filename)+1, sizeof(char));
	strcpy(img->id,filename);
	//READ IMG SIZE
	fread(&img->first_2_byte, sizeof(short),1,file);
	fread(&img->size, sizeof(int),1,file);
	fread(&img->second_4_byte, sizeof(int),1,file);
	//READ OFFSET
	fread(&img->offset, sizeof(int),1,file);
	//Rewind
	//rewind(file);
	fseek(file, 0L, SEEK_SET);

	img->header = calloc(img->offset+1, sizeof (unsigned char));
  fread(img->header, sizeof(unsigned char), img->offset, file);
  img->bytes = calloc(img->size - img->offset + 1,sizeof (unsigned char));
  fread(img->bytes, sizeof(unsigned char), img->size - img->offset, file);
  fclose(file);
  return img;
}

void write_image(image_t* img) {
	FILE * file = fopen("src\\out.bmp", "w+");
    fwrite(img->header, img->offset, 1, file);
    fwrite(img->bytes, img->size - img->offset, 1, file);
    fclose(file);
    return;
}

void printImage(image_t* img) {
    printf("Print BMP Matrix Image");
    return;
}
