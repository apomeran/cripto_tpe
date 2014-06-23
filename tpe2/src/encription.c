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
int checkLinealDependencyK2(int, int, byte, byte, image_t**, int);
int checkLinealDependencyK3(int, int, byte, byte, byte, image_t**, int);
char * byte_to_binary(int);
int modular_inverse(int i);

int modInverse(int i) {
    i %= 251;
    return inverses[i];
}

char * byte_to_binary(int x) {
    int z = 128;
    char * binary = calloc(sizeof (char), 9); //8 BITS + 1 '\0'
    binary[0] = '\0';
    for (; z > 0; z >>= 1) {
        strcat(binary, ((x & z) == z) ? "1" : "0");
    }
    return binary;
}

int
xorFromHashWrapper(unsigned char * hash, int length) {
    int i;
    char * hashInBits = calloc(sizeof (char), 1024);
    if (hashInBits == NULL) {
        return -1;
    }
    for (i = 0; i < length; i++) {
        // printf("%d %d '\n'",byte_to_binary((int)hash[i]),i);
        strcat(hashInBits, byte_to_binary((int) hash[i]));
    }
    hashInBits[i] = '\0';
    return xorFromHash(hashInBits);
}

int xorFromHash(char * hash) {
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

    if (k == 2) {
        shadows[0] = read_image("src\\encode1.bmp");
        shadows[1] = read_image("src\\encode2.bmp");
        shadows[2] = read_image("src\\encode3.bmp");
        shadows[3] = read_image("src\\encode4.bmp");

        if ((secret->size - secret->offset) % 2 != 0) {
            printf("Not Supported. The image needs to be Modulus 2 == 0 '\n'");
            return -1;
        }
        k_2_encode(shadows, secret, shadow_count);
        shadows[0]->id = "src\\encode1shadow.bmp";
        shadows[1]->id = "src\\encode2shadow.bmp";
        write_image(shadows[0]);
        write_image(shadows[1]);
    }
    if (k == 3) {
        shadows[0] = read_image("src\\k3-1.bmp");
        shadows[1] = read_image("src\\k3-2.bmp");
        shadows[2] = read_image("src\\k3-3.bmp");
        if ((secret->size - secret->offset) % 3 != 0) {
            printf("Not Supported. The image needs to be Modulus 3 == 0 '\n'");
            return -1;
        }
        k_3_encode(shadows, secret, shadow_count);
        shadows[0]->id = "src\\k3-1-shadow.bmp";
        shadows[1]->id = "src\\k3-2-shadow.bmp";
        shadows[2]->id = "src\\k3-3-shadow.bmp";
        write_image(shadows[0]);
        write_image(shadows[1]);
        write_image(shadows[2]);

    }
    if (k == 4) {
        printf("Not supported");
        return 1;
    }
    return -1;
}

void k_2_encode(image_t** shadows, image_t* secret, int shadow_count) {
    int index, current_shadow, j, authentication_bit = 0;
    //int **coefficient_matrix = calloc(sizeof(int*),shadow_count);
    int decal = 4;
    int img_size;
    int count = 0;
    byte secret_first_byte;
    byte shadow_first_byte;
    byte secret_second_byte;
    byte shadow_second_byte;
    byte decal_first_shadow_byte;
    byte decal_second_shadow_byte;
    int result;

    img_size = secret->size - secret->offset;
    for (index = 0; index < img_size; index += 2) {
        secret_first_byte = secret->bytes[index];
        secret_second_byte = secret->bytes[index + 1];
        for (current_shadow = 0; current_shadow < shadow_count; current_shadow++) {
            shadow_first_byte = shadows[current_shadow]->bytes[index];
            decal_first_shadow_byte = shadow_first_byte >> decal;
            shadow_second_byte = shadows[current_shadow]->bytes[index + 1];
            decal_second_shadow_byte = shadow_second_byte >> decal;
            while (checkLinealDependencyK2(index, current_shadow, decal_first_shadow_byte, decal_second_shadow_byte, shadows, shadow_count) == 1) {
                count++;
                decal_first_shadow_byte = (decal_first_shadow_byte + 1) % 15;
            }
            result = (decal_first_shadow_byte * secret_first_byte + decal_second_shadow_byte * secret_second_byte) % 251;
            shadows[current_shadow]->bytes[index] &= 0b11110000;
            shadows[current_shadow]->bytes[index] |= result >> 4;
            shadows[current_shadow]->bytes[index + 1] &= 0b11100000; //ONLY 3 . 1 FOR AUTHENTICATION BIT !!
            shadows[current_shadow]->bytes[index + 1] |= result & 0b00001111;
            authentication_bit = calculateAuth(shadows[current_shadow]->bytes[index], shadows[current_shadow]->bytes[index + 1]);
            // authentication_bit = 1;
            if (authentication_bit == 1) {
                shadows[current_shadow]->bytes[index + 1] |= 0b00001000;
            } else {
                shadows[current_shadow]->bytes[index + 1] &= 0b11110111;
            }
        }
    }
    printf("cambio %d '\n'", count);
    return;
}

int calculateAuth(int a1, int a2) {
    char hashValue[17]; // 2 BYTES . K=2 + 1 for '/o'
    char* a1_binary = byte_to_binary(a1);
    strcpy(hashValue, a1_binary);
    a2 = a2 << 1;
    char* a2_binary = byte_to_binary(a2);
    strcat(hashValue, a2_binary);
    // unsigned char* md = malloc(MD5_DIGGEST_LENGTH);
    // unsigned char* hash = MD5(hashValue,strlen(hashValue),md);
    unsigned char hash[] = "9214c48d83c004bf0afc2f0b8a02685d";
    return xorFromHashWrapper(hash, strlen(hash));
}

int calculateAuthK3(int a1, int a2, int a3) {
    char hashValue[25]; // 3 BYTES . K=3 + 1 for '/o'
    char* a1_binary = byte_to_binary(a1);
    strcpy(hashValue, a1_binary);
    a2 = a2 << 1;
    char* a2_binary = byte_to_binary(a2);
    strcat(hashValue, a2_binary);
    a3 = a3 << 1;
    char* a3_binary = byte_to_binary(a3);
    strcat(hashValue, a3_binary);
    // unsigned char* md = malloc(MD5_DIGGEST_LENGTH);
    // unsigned char* hash = MD5(hashValue,strlen(hashValue),md);
    unsigned char hash[] = "9214c48d83c004bf0afc2f0b8a02685d";
    return xorFromHashWrapper(hash, strlen(hash));
}

void k_3_encode(image_t** shadows, image_t* secret, int shadow_count) {
    int index, current_shadow, j, authentication_bit = 0;
    //int **coefficient_matrix = calloc(sizeof(int*),shadow_count);
    int decal = 3;
    int img_size;
    int count = 0;
    byte secret_first_byte;
    byte shadow_first_byte;
    byte secret_second_byte;
    byte shadow_second_byte;
    byte secret_third_byte;
    byte shadow_third_byte;
    byte decal_first_shadow_byte;
    byte decal_second_shadow_byte;
    byte decal_third_shadow_byte;
    int result;

    img_size = secret->size - secret->offset;
    for (index = 0; index < img_size; index += 3) {
        secret_first_byte = secret->bytes[index];
        secret_second_byte = secret->bytes[index + 1];
        secret_third_byte = secret->bytes[index + 2];
        for (current_shadow = 0; current_shadow < shadow_count; current_shadow++) {
            shadow_first_byte = shadows[current_shadow]->bytes[index];
            decal_first_shadow_byte = shadow_first_byte >> decal;
            shadow_second_byte = shadows[current_shadow]->bytes[index + 1];
            decal_second_shadow_byte = shadow_second_byte >> decal;
            shadow_third_byte = shadows[current_shadow]->bytes[index + 2];
            decal_third_shadow_byte = shadow_third_byte >> decal;
            while (checkLinealDependencyK3(index, current_shadow, decal_first_shadow_byte, decal_second_shadow_byte, decal_third_shadow_byte, shadows, shadow_count) == 1) {
                count++;
                decal_first_shadow_byte = (decal_first_shadow_byte + 1) % 15;
            }
            result = (decal_first_shadow_byte * secret_first_byte + decal_second_shadow_byte * secret_second_byte + decal_third_shadow_byte * secret_third_byte) % 251;
            shadows[current_shadow]->bytes[index] &= 0b11111000;
            shadows[current_shadow]->bytes[index] |= result >> 5;
            shadows[current_shadow]->bytes[index + 1] &= 0b11111000;
            shadows[current_shadow]->bytes[index + 1] |= (result & 0b00011100) >> 2;
            shadows[current_shadow]->bytes[index + 2] &= 0b11111000;
            shadows[current_shadow]->bytes[index + 2] |= (result & 0b00000011);
            // authentication_bit = calculateAuthK3(shadows[current_shadow]->bytes[index], shadows[current_shadow]->bytes[index + 1], shadows[current_shadow]->bytes[index + 2]);
            authentication_bit = 1;
            if (authentication_bit == 1) {
                shadows[current_shadow]->bytes[index + 2] |= 0b00000100;
            } else {
                shadows[current_shadow]->bytes[index + 1] &= 0b11111011;
            }
        }
    }
    return;
}

void k_4_encode(image_t** shadows, image_t* secret, int shadow_count) {
    return;
}

double getMultiplier(int a, int b) {
    if (a > b) {
        return (a * 1.0) / b;
    } else {
        return (b * 1.0) / a;
    }
}

int isDependent(int a, int b) {
    return ((a % b == 0) || (b % a == 0));
}

int checkLinealDependencyK3(int index, int current_shadow, byte first_byte, byte second_byte, byte third_byte, image_t** shadows, int n) {
    int i;
	int auxInverse = modInverse(first_byte);
    unsigned char firstIterationA = (auxInverse * second_byte) % 251;
    unsigned char firstIterationB = (auxInverse * third_byte) % 251;

    for (i = 0; i < n; i++) {
        if (i != current_shadow) {
		    byte b1 = shadows[i]->bytes[index] >> 3;
			byte b2 = shadows[i]->bytes[index+1] >> 3;
		    byte b3 = shadows[i]->bytes[index+2] >> 3;
            int auxInverseB = modInverse(b1);
			unsigned char currentIterationA = (auxInverseB * b2) % 251;
            unsigned char currentIterationB = (auxInverseB * b3) % 251;
            if (currentIterationA == firstIterationA && currentIterationB == firstIterationB) {
                printf("ee \n");
				return 1;
				
            }
        }
    }
    return 0;
}

int checkLinealDependencyK2(int index, int current_shadow, byte first_byte, byte second_byte, image_t** shadows, int n) {
    int i;
    unsigned char firstIteration = (modInverse(first_byte) * second_byte) % 251;
    for (i = 0; i < n; i++) {
        if (i != current_shadow) {
		    byte b1 = shadows[i]->bytes[index] >> 4;
		    byte b2 = shadows[i]->bytes[index+1] >> 4;
            unsigned char currentIteration = (modInverse(b1) * b2) % 251;
            if (currentIteration == firstIteration) {
                return 1;
            }
        }
    }
    return 0;
}

int checkLinealDependencyK2Direct(int index, int current_shadow, byte first_byte, byte second_byte, image_t** shadows, int n) {
    int i;
    double first_multiplier, second_multiplier;
    double epsilon = 0.0001;
    for (i = 0; i < n; i++) {
        if (i != current_shadow) {
            if (isDependent(first_byte, shadows[i]->bytes[index] >> 4)) {
                first_multiplier = getMultiplier(first_byte, shadows[i]->bytes[index] >> 4);
                if (isDependent(second_byte, shadows[i]->bytes[index + 1] >> 4)) {
                    second_multiplier = getMultiplier(first_byte, shadows[i]->bytes[index + 1] >> 4);
                    if (first_multiplier - second_multiplier < epsilon)
                        return 1;
                }
            }
        }
    }
    return 0;
}
