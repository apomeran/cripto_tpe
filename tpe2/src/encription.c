#include <stdlib.h>
#include <stdio.h>
#include "file.h"
#include "encription.h"
typedef unsigned char byte;
void k_2_encode(image_t**, image_t*, int);
void k_3_encode(image_t**, image_t*, int);

int encode(image_t* secret, int k, int n) {

    int shadow_count = n;
    image_t * shadows[4];
    shadows[0] = read_image("src\\encode1.bmp");
    shadows[1] = read_image("src\\encode2.bmp");
	shadows[2] = read_image("src\\encode3.bmp");
    shadows[3] = read_image("src\\encode4.bmp");
    
    if (k == 2) {
        k_2_encode(shadows, secret, shadow_count);
    }
    if (k == 3) {
        k_3_encode(shadows, secret, shadow_count);
    }
	if (k == 4) {
        k_4_encode(shadows, secret, shadow_count);
    }

    return 1;
}

void k_2_encode(image_t** shadows, image_t* secret, int shadow_count) {
    int index, current_shadow;
    int decal = 4;
    int img_size;
    byte secret_first_byte;
    byte secret_second_byte;
    byte decal_first_shadow_byte;
    byte decal_second_shadow_byte;
    int result;
    if (secret->size % 2 != 0) {
        printf("Not Supported");
        return;
    }
    img_size = secret->size - secret->offset;
    for (index = 0; index < img_size; index += 2) {
        secret_first_byte = secret->bytes[index];
        secret_second_byte = secret->bytes[index + 1];
        for (current_shadow = 0; current_shadow < shadow_count; current_shadow++) {
            decal_first_shadow_byte = shadows[current_shadow]->bytes[index] >> decal;
            decal_second_shadow_byte = shadows[current_shadow]->bytes[index + 1] >> decal;
            result = (decal_first_shadow_byte * secret_first_byte + decal_second_shadow_byte * secret_second_byte) % 251;
            shadows[current_shadow]->bytes[index] &= 0xF0;
            shadows[current_shadow]->bytes[index] |= result >> 4;
            shadows[current_shadow]->bytes[index + 1] &= 0xF0;
            shadows[current_shadow]->bytes[index + 1] |= result & 0X0F;
        }

    }
    return;
}

void k_3_encode(image_t** shadows, image_t* secret, int shadow_count) {


}
void k_4_encode(image_t** shadows, image_t* secret, int shadow_count) {


}
