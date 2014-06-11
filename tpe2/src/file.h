#ifndef __file__
#define __file__

#include "common.h"

BITMAPIMAGE* read_image(const char*);
void write_image(BITMAPIMAGE*);
void file_error(FILE*);
void printImage(BITMAPIMAGE* img);
#endif
