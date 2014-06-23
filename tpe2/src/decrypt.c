#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "decrypt.h"
#include "file.h"

#define IMG_MAX 3
#define MODULUS 251

void decrypt_bytes_k2(image_t * secret_image, image_t ** images, int block_position, int image_count);
void recover_block3(image_t * secret_image, image_t ** images, int block_position, int image_count);

image_t * decrypt(const char * directory, int k) {
	struct dirent *p_dirent;
  DIR* dir;
	dir = opendir(directory);
  int image_qty = 0;
	image_t * images[IMG_MAX];
	char * path;

	assure(dir != NULL, "Problem opening directory, check your syntax.\n");
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
	secret_image->id = "src/decrypted/decrypted_img.bmp";
  secret_image->size = images[0]->size;
  secret_image->offset = images[0]->offset;
  secret_image->bytes = (unsigned char *) malloc(secret_image->size - secret_image->offset);
  secret_image->header = (unsigned char *) malloc(secret_image->offset);
  memcpy(secret_image->header, images[0]->header, images[0]->offset);
  int i;

	void (*decrypt_bytes[2]) (image_t * secret, image_t ** images, int index, int image_qty);
	decrypt_bytes[0] = &decrypt_bytes_k2;
	decrypt_bytes[1] = &recover_block3;

  for (i = 0; i < secret_image->size - secret_image->offset; i += k) {
			if ((*decrypt_bytes[k - 2])) {
				(*decrypt_bytes[k - 2]) (secret_image, images, i, image_qty);
			}
  }
  return secret_image;
}

void decrypt_bytes_k2(image_t * secret_image, image_t ** images, int index, int image_qty) {
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

void recover_block3(image_t * secret_image, image_t ** images, int block_position, int image_count) {
    int coefficients[image_count][4];
    int i, j;
    for (i = 0; i < image_count; i++) {
        coefficients[i][0] = images[i]->bytes[block_position] >> 3;
        coefficients[i][1] = images[i]->bytes[block_position + 1] >> 3;
        // Shift 6 times to take parity bit out
        coefficients[i][2] = images[i]->bytes[block_position + 2] >> 3;
        coefficients[i][3] = ((images[i]->bytes[block_position] & 7) << 5) | ((images[i]->bytes[block_position + 1] & 7) << 2) | (images[i]->bytes[block_position + 2] & 3);
    }

    // Take the first item of every equation to 1 to solve.
    for (i = 0; i < image_count; i++) {
        unsigned char first_mod_inverse = inverse(coefficients[i][0]);
        for (j = 0; j < 4; j++) {
            coefficients[i][j] = ((int)coefficients[i][j] * first_mod_inverse) % 251;
        }
    }

    // Substract first row from every other
    for (i = 1; i < image_count; i++) {
        for (j = 0; j < 4; j++) {
            int result = ((int)coefficients[i][j] - coefficients[0][j]);
            coefficients[i][j] = (result < 0) ? result + 251 : result;
        }
    }

    unsigned char aux;
    if (coefficients[1][1] == 0) {
        for (j = 0; j < 4; j++) {
            aux = coefficients[1][j];
            coefficients[1][j] = coefficients[2][j];
            coefficients[2][j] = aux;
        }
    } else {
        if (coefficients[2][1] != 0) {
            // Take the second item of every equation below the first one to 1 to solve.
            for (i = 1; i < image_count; i++) {
                unsigned char second_mod_inverse = inverse(coefficients[i][1]);
                for (j = 1; j < 4; j++) {
                    coefficients[i][j] = ((int)coefficients[i][j] * second_mod_inverse) % 251;
                }
            }

            // Substract second row from every other
            // TODO: Modularize this
            for (i = 2; i < image_count; i++) {
                for (j = 0; j < 4; j++) {
                    int result = ((int)coefficients[i][j] - coefficients[1][j]);
                    coefficients[i][j] = (result < 0) ? result + 251 : result;
                }
            }
        }
    }

    if (coefficients[1][0] != 0 || coefficients[2][0] != 0 || coefficients[2][1] != 0 || coefficients[1][1] == 0 || coefficients[2][2] == 0) {
        printf("Something went wrong with the equations!\n");
        printf("%d %d %d\n%d %d %d\n", coefficients[1][0], coefficients[1][1], coefficients[1][2], coefficients[2][0], coefficients[2][1], coefficients[2][2]);
    }

    // Use the first three equations to solve the system.
		unsigned char z = (coefficients[2][3] * inverse(coefficients[2][2])) % 251;
    int y = (coefficients[1][3] - (coefficients[1][2] * z)) % 251;
    int x = (coefficients[0][3] - (coefficients[0][2] * z) - (coefficients[0][1] * y)) % 251;
		if (z < 0)
			z += 251;
		if (y < 0)
			y += 251;
		if (x < 0)
			x += 251;

    secret_image->bytes[block_position] = (unsigned char) x;
    secret_image->bytes[block_position + 1] = (unsigned char) y;
    secret_image->bytes[block_position + 2] = (unsigned char) z;
}
