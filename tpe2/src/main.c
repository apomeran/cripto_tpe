#include <stdio.h>
#include <string.h>

#include "common.h"
#include "file.h"

int extract_image();

int main(void) {

  //-------Begin parsing arguments
  int isRecovery = FALSE;
  int k = -1;
  int n = -1;
  char image[40] = {0};
  char dir[40] = {0};

  int opt = 0;

  static struct option long_options[] = {
        {"secret",    required_argument, 0,  's' },
        {"dir",       required_argument, 0,  'i' },
        {"d",         no_argument,       0,  'd' },
        {"r",         no_argument,       0,  'r' },
        {"k",         required_argument, 0,  'k' },
        {"n",         required_argument, 0,  'n' },
        {0,           0,                 0,  0   }
  };

  int long_index =0;
  while ((opt = getopt_long_only(argc, argv,"", long_options, &long_index )) != -1) {
    switch (opt){
      case 's':
        strcpy(image, optarg);
        break;
      case 'i':
        strcpy(dir, optarg);
        break;
      case 'd':
        isRecovery = FALSE;
        break;
      case 'r':
        isRecovery = TRUE;
        break;
      case 'k':
        k = atoi(optarg);
        break;
      case 'n':
        n = atoi(optarg);
        break;
      default:
        printf("dafuq i'm doing here\n");
        break;
    }
  }
  //-------End parsing arguments

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
