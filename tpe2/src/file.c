#include <stdio.h>
#include <unistd.h>

#include "file.h"

int fsize(FILE* file){
	return ftell(file);
}

struct data* read_image(const char* filename){
	FILE* file = fopen(filename,"r");
	struct data* img = malloc(sizeof(struct data));
	img->len = fsize(file);
	img->bytes = malloc(sizeof(unsigned char) * img->len);
	fread(img->bytes,sizeof(unsigned char), img->len, file);
	fclose(file);
	return img;
}