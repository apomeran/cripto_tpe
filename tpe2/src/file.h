#ifndef __file__
#define __file__

#include <stdio.h>
#include "common.h"

image_t* read_image(const char*);
void write_image(image_t*);
void file_error(FILE*);
void printImage(image_t* img);
image_t ** read_images_from_dir(char *, int);

#endif
