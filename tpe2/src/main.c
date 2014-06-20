#include <stdio.h>
#include <string.h>

#include "common.h"
#include "file.h"

int extract_image();

int main(void) {

    int result = extract_image();
    return result;

}

int extract_image() {
    int height = 156;
    int width = height;
    int i;
    image_t* image = read_image("src\\g.bmp");
    write_image(image);
    //for (i = 0; i < height * width; i++) {
     //   printf("%d", image->bytes[i]);
    //}

    return 0;
}