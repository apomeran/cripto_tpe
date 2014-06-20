#include <stdio.h>
#include "file.h"
#include "common.h"

image_t* read_image(const char* filename) {
    int byte_ammount = -1;
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error while reading BMP Image \n");
        return NULL;
    }
    image_t* img = malloc(sizeof(struct tagImage));
   
	//READ IMG SIZE
	fseek(file, 2, SEEK_CUR);
	fread(&img->size, sizeof(int),1,file);
	//READ OFFSET
	fseek(file, 4, SEEK_CUR);
	fread(&img->offset, sizeof(int),1,file);
	//Rewind
	//rewind(file);
	fseek(file, 0L, SEEK_SET); 
   
	img->header = malloc((sizeof (unsigned char) * (img->offset+1)));
    fread(img->header, sizeof(unsigned char), img->offset, file);
    img->bytes = malloc(sizeof (unsigned char) * (img->size - img->offset + 1));
    fread(img->bytes, sizeof(unsigned char), img->size - img->offset, file);
    fclose(file);
    return img;
}

void write_image(image_t* img) {
	FILE * file = fopen("src\\out.bmp", "w+");
    fwrite(&img->header, img->offset+1, 1, file);
    fseek(file, img->offset+1, SEEK_SET);
    fwrite(img->bytes, img->size - img->offset, 1, file);
    fclose(file);
    return;
}

void printImage(image_t* img) {
    printf("Print BMP Matrix Image");
    return;
}