#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "decrypt.h"
#include "file.h"

#define IMG_MAX 3
#define MODULUS 251

void decrypt_bytes_k2(image_t * secret_image, image_t ** images, int index, int image_qty);
void decrypt_bytes_k3(image_t * secret_image, image_t ** images, int index, int image_qty);

image_t * decrypt(const char * directory, int k) {
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
	secret_image->id = "src/decrypted/dec_img.bmp";
  secret_image->size = images[0]->size;
  secret_image->offset = images[0]->offset;
  secret_image->bytes = (unsigned char *) malloc(secret_image->size - secret_image->offset);
  secret_image->header = (unsigned char *) malloc(secret_image->offset);
  memcpy(secret_image->header, images[0]->header, images[0]->offset);

	void (*decrypt_bytes[2]) (image_t *, image_t **, int, int);
	decrypt_bytes[0] = &decrypt_bytes_k2;
	decrypt_bytes[1] = &decrypt_bytes_k3;

  int i;
  for (i = 0; i < secret_image->size - secret_image->offset; i += k) {
      (*decrypt_bytes[k-2])(secret_image, images, i, image_qty);
  }
  return secret_image;
}

void decrypt_bytes_k2(image_t * secret_image, image_t ** images, int index, int image_qty) {
    unsigned char polynomials[image_qty][3];
    int i, j;
		unsigned char aux_bytes[8][2];
    for (i = 0; i < image_qty; i++) {
			// itoa(images[i]->bytes[index], aux_bytes[0], 2);
			// itoa(images[i]->bytes[index + 1], aux_bytes[1], 2);
			// printf("%s %s\n", aux_bytes[0], aux_bytes[1]);
			// memcpy(polynomials[i][0], aux_bytes[0], 4);
			// memcpy(polynomials[i][1], aux_bytes[1], 3);
			// memcpy(polynomials[i][2], aux_bytes[0] + 4, 4);
			// memcpy(polynomials[i][2], aux_bytes[1] + 4, 4);
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

void decrypt_bytes_k3(image_t * secret_image, image_t ** images, int index, int image_qty) {
    unsigned char polynomials[image_qty][4];
    int i, j;
    for (i = 0; i < image_qty; i++) {
        polynomials[i][0] = images[i]->bytes[index] >> 3;
        polynomials[i][1] = images[i]->bytes[index + 1] >> 3;
        // Shift 6 times to take parity bit out
        polynomials[i][2] = images[i]->bytes[index + 2] >> 3;
        polynomials[i][3] = ((images[i]->bytes[index] & 7) << 5) | ((images[i]->bytes[index + 1] & 7) << 2) | (images[i]->bytes[index + 2] & 3);
    }

    // Take the first item of every equation to 1 to solve.
    for (i = 0; i < image_qty; i++) {
        unsigned char first_mod_inverse = inverse(polynomials[i][0]);
        for (j = 0; j < 4; j++) {
            polynomials[i][j] = ((int)polynomials[i][j] * first_mod_inverse) % MODULUS;
        }
    }

    // Substract first row from every other
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
            // Take the second item of every equation below the first one to 1 to solve.
            for (i = 1; i < image_qty; i++) {
                unsigned char second_mod_inverse = inverse(polynomials[i][1]);
                for (j = 1; j < 4; j++) {
                    polynomials[i][j] = ((int)polynomials[i][j] * second_mod_inverse) % MODULUS;
                }
            }
            // TODO: Modularize this
            for (i = 2; i < image_qty; i++) {
                for (j = 0; j < 4; j++) {
                    int result = ((int)polynomials[i][j] - polynomials[1][j]);
                    polynomials[i][j] = (result < 0) ? result + MODULUS : result;
                }
            }
        }
    }

    if (polynomials[1][0] != 0 || polynomials[2][0] != 0 || polynomials[2][1] != 0 || polynomials[1][1] == 0 || polynomials[2][2] == 0) {
        printf("Something went wrong with the equations!\n");
        //printf("%d %d %d\n%d %d %d\n", coefficients[1][0], coefficients[1][1], coefficients[1][2], coefficients[2][0], coefficients[2][1], coefficients[2][2]);
    }

    // Use the first three equations to solve the system.
    unsigned char z = (polynomials[2][3] * inverse(polynomials[2][2])) % MODULUS;
    int y = ((polynomials[1][3] - (polynomials[1][2] * z)) * inverse(polynomials[1][1])) % MODULUS;
    while(y < 0) {
      y += MODULUS;
    }
    int x = ((polynomials[0][3] - (polynomials[0][2] * z)) - (polynomials[0][1] * y)) % MODULUS;
    while (x < 0) {
      x += MODULUS;
    }

    secret_image->bytes[index] = x;
    secret_image->bytes[index + 1] = y;
    secret_image->bytes[index + 2] = z;
}
