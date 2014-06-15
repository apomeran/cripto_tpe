#include <stdlib.h>
#include <stdio.h>
#include "file.h"
#include "encription.h"
typedef unsigned char byte;
void k_2_encode(BITMAPIMAGE**, BITMAPIMAGE*, int);
void k_3_encode(BITMAPIMAGE**, BITMAPIMAGE*, int);

int encode(BITMAPIMAGE* img_to_encript, int k) {

    int image_count = 2;
    BITMAPIMAGE * imgs[2];
    imgs[0] = read_image("src\\g.bmp");
    imgs[1] = read_image("src\\o.bmp");
    if (k == 2) {
        k_2_encode(imgs, img_to_encript, 2);
    }
    if (k == 3) {
        k_3_encode(imgs, img_to_encript, image_count);
    }

    return 1;
}

void k_2_encode(BITMAPIMAGE** imgs, BITMAPIMAGE* img_to_encript, int image_count) {
    int index, current_image;
    int decal = 4;
    int img_size;
    byte enc_first_byte;
    byte enc_second_byte;
    byte decal_first_byte;
    byte decal_second_byte;
    int result;
    if (img_to_encript->info_header.biSize % 2 != 0) {
        printf("Not Supported");
        return;
    }
    img_size = img_to_encript->info_header.biWidth * img_to_encript->info_header.biHeight;
    for (index = 0; index < img_size; index += 2) {
        enc_first_byte = img_to_encript->bytes[index];
        enc_second_byte = img_to_encript->bytes[index + 1];
        for (current_image = 0; current_image < image_count; current_image++) {
            decal_first_byte = imgs[current_image]->bytes[index] >> decal;
            decal_second_byte = imgs[current_image]->bytes[index + 1] >> decal;
            result = (decal_first_byte * enc_first_byte + decal_second_byte * enc_second_byte) % 251;
            images[current_image]->bitmap[index] &= 0xF0;
            images[current_image]->bitmap[index] |= result & 0X0F;
            images[current_image]->bitmap[index + 1] &= 0xF0;
            images[current_image]->bitmap[index + 1] |= result & 0X0F;
        }

    }
    return;
}

void k_3_encode(BITMAPIMAGE** imgs, BITMAPIMAGE* img_to_encript, int image_count) {


}
