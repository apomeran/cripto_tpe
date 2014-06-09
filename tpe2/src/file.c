#include <stdio.h>
#include <unistd.h>
#include "file.h"
#include "common.h"


struct data* read_image(const char* filename){
	FILE* file = fopen(filename,"r");   
	struct data* img = malloc(sizeof(struct data));
        fseek(file,0L, SEEK_END);
        img->len = ftell(file);
        fseek(file,0L, SEEK_SET);
	img->bytes = malloc(sizeof(unsigned char) * img->len);
	fread(img->bytes,sizeof(unsigned char), img->len, file);
	fclose(file);
	return img;
}

 int write_image(const char* filename){
	return 0;
}
 
 void printImage(){
     printf("Print BMP Matrix Image");
     return;
 }