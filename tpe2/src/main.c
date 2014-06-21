#include <stdio.h>
#include <string.h>

#include "common.h"
#include "file.h"

image_t* extract_image(char* filename);

int main(void) {
	
    image_t* secret_image = extract_image("src\\secret.bmp");
	int result = encode(secret_image,2);
    return 1;

}

image_t*  extract_image(char* filename) {
    image_t* image = read_image(filename);
//    write_image(image);
    return image;
}