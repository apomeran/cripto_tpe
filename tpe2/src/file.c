#include <stdio.h>
#include "file.h"
#include "common.h"
BITMAPIMAGE* read_image(const char* filename) {
    int byte_ammount = -1;
    int extra_header_size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    FILE* file = fopen(filename, "r+");
    if (file == NULL) {
        file_error(file);
        return NULL;
    }
    BITMAPIMAGE* img = malloc(sizeof (BITMAPIMAGE));
    //  fseek(file,0L, SEEK_END);
    //  img->len = ftell(file);
    //  fseek(file,0L, SEEK_SET);
    fread(&img->file_header, sizeof (BITMAPFILEHEADER), 1, file);
    fread(&img->info_header, sizeof (BITMAPINFOHEADER), 1, file);
    img->extra_header = malloc(sizeof(unsigned char) * img->file_header.bfOffBits - extra_header_size);
    fread(img->extra_header, img->file_header.bfOffBits - extra_header_size, 1, file);

    fseek(file, img->file_header.bfOffBits, SEEK_SET);
    byte_ammount = img->info_header.biHeight * img->info_header.biWidth;
    if (byte_ammount < 0) {
        file_error(file);
        return NULL;
    }
    img->bytes = malloc(sizeof (unsigned char) * byte_ammount);
    fread(img->bytes, sizeof (unsigned char), byte_ammount, file);
    fclose(file);
    return img;
}

void file_error(FILE* f) {
    if (f != NULL)
        close(f);
    printf("Error while reading BMP Image \n");
    return;
}

void write_image(BITMAPIMAGE* img) {
    int extra_header_size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    int byte_ammount = 0;
    FILE * file = fopen("src\\out.bmp", "w+");
    fwrite(&img->file_header, sizeof (BITMAPFILEHEADER), 1, file);
    fwrite(&img->info_header, sizeof (BITMAPINFOHEADER), 1, file);
    fwrite(img->extra_header, img->file_header.bfOffBits - extra_header_size, 1, file);
    byte_ammount = img->info_header.biHeight * img->info_header.biWidth;
    fseek(file, img->file_header.bfOffBits, SEEK_SET);
    fwrite(img->bytes, byte_ammount, 1, file);
    fclose(file);
    return;
}

void printImage(BITMAPIMAGE* img) {
    printf("Print BMP Matrix Image");
    return;
}