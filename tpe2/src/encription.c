#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "file.h"
#include "encription.h"
//#include <openssl/md5.h>
typedef unsigned char byte;
void k_2_encode(image_t**, image_t*, int);
void k_3_encode(image_t**, image_t*, int);
void k_4_encode(image_t**, image_t*, int);
char * byte_to_binary(int);

char * byte_to_binary(int x) {
    int z = 128;
    char * binary = calloc(sizeof(char), 9); //8 BITS + 1 '\0'
    binary[0] = '\0';
    for (; z > 0; z >>= 1) {
        strcat(binary, ((x & z) == z) ? "1" : "0");
    }
    return binary;
}

int
xorFromHashWrapper(unsigned char * hash, int length) {
	int i;
	char * hashInBits = calloc(sizeof(char), 1024);
	if (hashInBits == NULL) {
		// *error = CALLOC_ERROR;
		return -1;
	}

	for (i = 0; i < length; i++) {
		// printf("%d %d '\n'",byte_to_binary((int)hash[i]),i);
		strcat(hashInBits, byte_to_binary((int)hash[i]));
	}
	hashInBits[i] = '\0';
	return xorFromHash(hashInBits);
}

int
xorFromHash(char * hash) {
	int i;
	int length = strlen(hash);
	unsigned int result = hash[0] - '0';
	for (i = 1; i < length; i++) {
		result = result ^ (hash[i] - '0');
	}
	return result;
}

int encode(image_t* secret, int k, int n) {

    int shadow_count = n;
    shadow_count = 2;
    image_t * shadows[4];
    shadows[0] = read_image("src\\encode1.bmp");
    shadows[1] = read_image("src\\encode2.bmp");
	shadows[2] = read_image("src\\encode3.bmp");
    shadows[3] = read_image("src\\encode4.bmp");
    
    if (k == 2) {
        k_2_encode(shadows, secret, shadow_count);
		shadows[0]->id = "src\\encode1shadow.bmp";
		shadows[1]->id = "src\\encode2shadow.bmp";
		write_image(shadows[0]);
		write_image(shadows[1]);
    }
    if (k == 3) {
        k_3_encode(shadows, secret, shadow_count);
    }
	if (k == 4) {
        printf("Not supported");
    }

    return 1;
}

void k_2_encode(image_t** shadows, image_t* secret, int shadow_count) {
    int index, current_shadow, j, authentication_bit=0;
	//int **coefficient_matrix = calloc(sizeof(int*),shadow_count);
    int decal = 4;
    int img_size;
    byte secret_first_byte;
    byte shadow_first_byte;
    byte secret_second_byte;
    byte shadow_second_byte;
    byte decal_first_shadow_byte;
    byte decal_second_shadow_byte;
    int result;
    if (secret->size % 2 != 0) {
        printf("Not Supported !! '\n'");
        return;
    }
	
    img_size = secret->size - secret->offset;
    for (index = 0; index < img_size; index += 2) {
	
        secret_first_byte = secret->bytes[index];
        secret_second_byte = secret->bytes[index + 1];
        for (current_shadow = 0; current_shadow < shadow_count; current_shadow++) {
			shadow_first_byte = shadows[current_shadow]->bytes[index];
            decal_first_shadow_byte = secret_first_byte >> decal;
			shadow_second_byte = shadows[current_shadow]->bytes[index + 1];
            decal_second_shadow_byte = secret_second_byte >> decal;
            result = (decal_first_shadow_byte * secret_first_byte + decal_second_shadow_byte * secret_second_byte) % 251;
            shadows[current_shadow]->bytes[index] &= 0b11110000;
            shadows[current_shadow]->bytes[index] |= result >> 4;
            shadows[current_shadow]->bytes[index + 1] &= 0b11100000; //ONLY 3 . 1 FOR AUTHENTICATION BIT !!
            shadows[current_shadow]->bytes[index + 1] |= result & 0b00001111;
			authentication_bit = calculateAuth(shadows[current_shadow]->bytes[index], shadows[current_shadow]->bytes[index+1]);
			authentication_bit = 1;
			if (authentication_bit == 1){
			  shadows[current_shadow]->bytes[index + 1] |= 0b00001000;
			}
			else{
			  shadows[current_shadow]->bytes[index + 1] &= 0b11110111;
			}
		 }

    }
    return;
}

int calculateAuth(int a1, int a2){
	char hashValue[17]; // 2 BYTES . K=2 + 1 for '/o'
	char* a1_binary = byte_to_binary(a1);
	strcpy(hashValue,a1_binary);
	a2 = a2 << 1;
	char* a2_binary = byte_to_binary(a2);
	strcat(hashValue,a2_binary);
	// unsigned char* md = malloc(MD5_DIGGEST_LENGTH);
	// unsigned char* hash = MD5(hashValue,strlen(hashValue),md);
	unsigned char hash[] = "9214c48d83c004bf0afc2f0b8a02685d";
	return xorFromHashWrapper(hash,strlen(hash));
}

void k_3_encode(image_t** shadows, image_t* secret, int shadow_count) {


}
void k_4_encode(image_t** shadows, image_t* secret, int shadow_count) {
}


