#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "common.h"
#include "decrypt.h"
#include "file.h"
#include "itoa.h"

#define MODULUS 251

void decrypt_bytes_k2(image_t * secret_image, image_t ** images, int index, int image_qty);
void decrypt_bytes_k3(image_t * secret_image, image_t ** images, int index, int image_qty);

image_t * decrypt(char * directory, int k, char * img_name) {

	struct dirent* p_dirent;
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
  secret_image->id = img_name;
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

void complete_left_zeros(char * string) {
	if (!string)
		return;
	char buffer[9];
	int length = strlen(string);


	if (length < 8) {
		strcpy(buffer, string);
		int i;
		for (i = 0; i < 8 - length; i++) {
			string[i] = '0';
		}
		strcpy(string + (8 - length), buffer);
	}
}

int binary_to_integer(char * bin) {
	int total = 0;
	while (*bin)
	{
		total *= 2;
		if (*bin++ == '1') total += 1;
	}
	return total;
}

int crop_binary(char * bin, int k) {
	char aux[9];
	char * zeros = "00000000";

	strncpy(aux, zeros, 8 - k);
	strncpy(aux + (8 - k), bin, k);

	return binary_to_integer(aux);
}

void decrypt_bytes_k2(image_t * img, image_t ** imgs, int index, int image_qty) {
    unsigned char polynomials[image_qty][3];
    int i, j;
		char aux_bytes[2][9];
		char * zeros = "00000000";
    for (i = 0; i < image_qty; i++) {
			itoa(imgs[i]->bytes[index], aux_bytes[0], 2);
			itoa(imgs[i]->bytes[index + 1], aux_bytes[1], 2);
			complete_left_zeros(aux_bytes[0]);
			complete_left_zeros(aux_bytes[1]);

			char aux[9];

			strncpy(aux, zeros, 4);
			strncpy(aux + 4, aux_bytes[0], 4);
			polynomials[i][0] = binary_to_integer(aux);

			strncpy(aux, zeros, 5);
			strncpy(aux + 5, aux_bytes[1], 3);
			polynomials[i][1] = binary_to_integer(aux);

			strcpy(aux, aux_bytes[0] + 4);
			strcat(aux, aux_bytes[1] + 4);
			polynomials[i][2] = binary_to_integer(aux);
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

    img->bytes[index] = x; img->bytes[index + 1] = y;
}

void decrypt_bytes_k3(image_t * img, image_t ** imgs, int index, int image_qty) {
    unsigned char polynomials[image_qty][4];
    int i, j;
		char aux_bytes[3][9];
		char * zeros = "00000000";
    for (i = 0; i < image_qty; i++) {
			itoa(imgs[i]->bytes[index], aux_bytes[0], 2);
			itoa(imgs[i]->bytes[index + 1], aux_bytes[1], 2);
			itoa(imgs[i]->bytes[index + 2], aux_bytes[2], 2);
			complete_left_zeros(aux_bytes[0]);
			complete_left_zeros(aux_bytes[1]);
			complete_left_zeros(aux_bytes[2]);

			char aux[9];

			strncpy(aux, zeros, 3);
			strncpy(aux + 3, aux_bytes[0], 5);
			polynomials[i][0] = binary_to_integer(aux);

			strncpy(aux, zeros, 3);
			strncpy(aux + 3, aux_bytes[1], 5);
			polynomials[i][1] = binary_to_integer(aux);

			strncpy(aux, zeros, 3);
			strncpy(aux + 3, aux_bytes[2], 5);
			polynomials[i][2] = binary_to_integer(aux);

			strcpy(aux, aux_bytes[0] + 5);

			strcat(aux, aux_bytes[1] + 5);

			strcat(aux, aux_bytes[2] + 6);
			polynomials[i][3] = binary_to_integer(aux);
    }

    for (i = 0; i < image_qty; i++) {
        unsigned char first_mod_inverse = inverse(polynomials[i][0]);
        for (j = 0; j < 4; j++) {
            polynomials[i][j] = ((int)polynomials[i][j] * first_mod_inverse) % MODULUS;
        }
    }

    for (i = 1; i < image_qty; i++) {
        for (j = 0; j < 4; j++) {
            int result = ((int)polynomials[i][j] - polynomials[0][j]);
	    if(result<0)
                polynomials[i][j] = result + MODULUS;
	    else 
		polynomials[i][j] = result;
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
                unsigned char second_mod_inverse = inverse(polynomials[i][1]);
                for (j = 1; j < 4; j++) {
                    polynomials[i][j] = ((int)polynomials[i][j] * second_mod_inverse) % MODULUS;
                }
            }
            for (i = 2; i < image_qty; i++) {
                for (j = 0; j < 4; j++) {
                    int result = ((int)polynomials[i][j] - polynomials[1][j]);
		    if(result < 0)
			polynomials[i][j] = result + MODULUS;
		    else
                	polynomials[i][j] = result;
                 
                }
            }
        }
    }

    unsigned char z = (polynomials[2][3] * inverse(polynomials[2][2])) % MODULUS;
    int y = ((polynomials[1][3] - (polynomials[1][2] * z)) * inverse(polynomials[1][1])) % MODULUS;
    while((y = y+MODULUS) < (0)); 


    int x = ((polynomials[0][3] - (polynomials[0][2] * z)) - (polynomials[0][1] * y)) % MODULUS;
    while (((x=x+MODULUS)) < (0));

    img->bytes[index] = x; img->bytes[index + 1] = y;  img->bytes[index + 2] = z;
}
