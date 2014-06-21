#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "decript.h"
#include "file.h"

#define IMG_MAX 3
#define MODULUS 251

image_t * decript(const char * directory, int k) {
	struct dirent *p_dirent;
  DIR* dir;
	dir = opendir(directory);
  int image_qty = 0;
	image_t * images[IMG_MAX];
	char * path;

	assure(dir != NULL, "Problem opening directory, check your sintax.\n");
  while ((p_dirent = readdir(dir)) != NULL) {
 	 	if(strstr(p_dirent->d_name, ".bmp") && image_qty < IMG_MAX) {
 	 		path = calloc(strlen(directory) + strlen(p_dirent->d_name) + 2, 1);
        strcpy(path, directory);
        strcat(path, "/");
        strcat(path, p_dirent->d_name);
        images[image_qty++] = read_image(path);
        free(path);
 	 	}
  }
  closedir(dir);

  assure(image_qty >= k, "You didn't provide the necessary amount of pics.\n");

  image_t * secret_image = (image_t *) malloc(sizeof(image_t));
  secret_image->size = images[0]->size;
  secret_image->offset = images[0]->offset;
  secret_image->bytes = (unsigned char *) malloc(secret_image->size - secret_image->offset);
  secret_image->header = (unsigned char *) malloc(secret_image->offset);
  memcpy(secret_image->header, images[0]->header, images[0]->offset);
  int i;
  for (i = 0; i < secret_image->size - secret_image->offset; i += k) {
      inner_decript(secret_image, images, k, i, image_qty);
  }
  return secret_image;
}

void inner_decript(image_t * secret_image, image_t ** images, int k, int index, int image_qty) {
    switch(k) {
        case 2:
            decript_bytes_k2(secret_image, images, index, image_qty);
            break;
        case 3:
            decript_bytes_k3(secret_image, images, index, image_qty);
            break;
    }
}

void decript_bytes_k2(image_t * secret_image, image_t ** images, int index, int image_qty) {
    unsigned char polynomials[image_qty][3];
    int i, j;
    for (i = 0; i < image_qty; i++) {
      polynomials[i][0] = images[i]->bytes[index] >> 4;
      polynomials[i][1] = images[i]->bytes[index + 1] >> 5;
      polynomials[i][2] = ((images[i]->bytes[index] & 0x0F) << 4) | (images[i]->bytes[index + 1] & 0x0F);
    }

    for (i = 0; i < image_qty; i++) {
        unsigned char _inverse = inverse(polynomials[i][0]);
        for (j = 0; j < 3; j++) {
            polynomials[i][j] = ((int)polynomials[i][j] * _inverse) % MODULUS;
        }
    }

    for (j = 0; j < 3; j++) {
        int result = ((int)polynomials[1][j] - polynomials[0][j]);
        polynomials[1][j] = (result < 0) ? result + MODULUS : result;
    }

    unsigned char y = (polynomials[1][2] * inverse(polynomials[1][1])) % MODULUS;
    unsigned char x = (polynomials[0][2] - (polynomials[0][1] * y)) % MODULUS;

    secret_image->bytes[index] = x;
    secret_image->bytes[index + 1] = y;
}

void decript_bytes_k3(image_t * secret_image, image_t ** images, int index, int image_qty) {
    unsigned char polynomials[image_qty][4];
    int i, j;
    for (i = 0; i < image_qty; i++) {
        polynomials[i][0] = images[i]->bytes[index] >> 3;
        polynomials[i][1] = images[i]->bytes[index + 1] >> 3;
        polynomials[i][2] = images[i]->bytes[index + 2] >> 3;
        polynomials[i][3] = ((images[i]->bytes[index] & 7) << 5) | ((images[i]->bytes[index + 1] & 7) << 2) | (images[i]->bytes[i + 2] & 3);
    }

    for (i = 0; i < image_qty; i++) {
        unsigned char _inverse = inverse(polynomials[i][0]);
        for (j = 0; j < 4; j++) {
            polynomials[i][j] = ((int)polynomials[i][j] * _inverse) % MODULUS;
        }
    }
    for (i = 1; i < image_qty; i++) {
        for (j = 0; j < 4; j++) {
            int result = ((int)polynomials[i][j] - polynomials[0][j]);
            polynomials[i][j] = (result < 0) ? result + MODULUS : result;
        }
    }

    unsigned char aux;
    if (polynomials[1][1] == 0) {
        for (j = 0; j < 4; j++) {
            aux = polynomials[1][j];
            polynomials[1][j] = polynomials[2][j];
            polynomials[2][j] = aux;
        }
    } else {
        if (polynomials[2][1] != 0) {
            for (i = 1; i < image_qty; i++) {
                unsigned char _inverse = inverse(polynomials[i][1]);
                for (j = 1; j < 4; j++) {
                    polynomials[i][j] = ((int)polynomials[i][j] * _inverse) % MODULUS;
                }
            }

            for (i = 2; i < image_qty; i++) {
                for (j = 0; j < 4; j++) {
                    int result = ((int)polynomials[i][j] - polynomials[1][j]);
                    polynomials[i][j] = (result < 0) ? result + MODULUS : result;
                }
            }
        }
    }

    unsigned char z = (polynomials[2][3] * inverse(polynomials[2][2])) % MODULUS;
    unsigned char y = (polynomials[1][3] - (polynomials[1][2] * z)) % MODULUS;
    unsigned char x = (polynomials[0][3] - (polynomials[0][2] * z) - (polynomials[0][1] * y)) % MODULUS;

    secret_image->bytes[index] = x;
    secret_image->bytes[index + 1] = y;
    secret_image->bytes[index + 2] = z;
}
