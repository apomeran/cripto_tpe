#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "file.h"
#include "common.h"

image_t* read_image(const char* filename) {
    int byte_ammount = -1;
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error while reading BMP Image \n");
        return NULL;
    }
    image_t* img = malloc(sizeof(struct tagImage));

	img->id = calloc(strlen(filename)+1, sizeof(char));
	strcpy(img->id,filename);
	//READ IMG SIZE
	fread(&img->first_2_byte, sizeof(short),1,file);
	fread(&img->size, sizeof(int),1,file);
	fread(&img->second_4_byte, sizeof(int),1,file);
	//READ OFFSET
	fread(&img->offset, sizeof(int),1,file);
	//Rewind
	//rewind(file);
	fseek(file, 0L, SEEK_SET);

	img->header = calloc(img->offset+1, sizeof (unsigned char));
	fread(img->header, sizeof(unsigned char), img->offset, file);
	img->bytes = calloc(img->size - img->offset + 1,sizeof (unsigned char));
	fread(img->bytes, sizeof(unsigned char), img->size - img->offset, file);
	fclose(file);
	return img;
}

void write_image(image_t* img) {
	FILE * file = fopen(img->id, "w+");
  if (file) {
    fwrite(img->header, img->offset, 1, file);
    fwrite(img->bytes, img->size - img->offset, 1, file);
    fclose(file);
  }
  return;
}

void printImage(image_t* img) {
    printf("Print BMP Matrix Image");
    return;
}

image_t ** read_images_from_dir(char * directory, int n) {
  int image_qty = 0;
  struct dirent *p_dirent;
  DIR* dir;
  dir = opendir(directory);
  image_t ** images = malloc(IMG_MAX * sizeof(image_t *));
  char * path;

  assure(dir != NULL, "Problem opening directory, check your sintax.\n");
  while ((p_dirent = readdir(dir)) != NULL) {
      if(strstr(p_dirent->d_name, ".bmp") && image_qty < IMG_MAX && image_qty <= n) {
        path = calloc(strlen(directory) + strlen(p_dirent->d_name) + 2, 1);
        strcpy(path, directory);
        strcat(path, "/");
        strcat(path, p_dirent->d_name);

        // images[image_qty] = malloc(sizeof(image_t));
        images[image_qty++] = read_image(path);
        free(path);
      }
  }
  closedir(dir);

  return images;
}
