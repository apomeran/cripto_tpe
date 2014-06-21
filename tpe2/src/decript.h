image_t * encript(image_t secret, char* directory, int k);
image_t * decript(const char * directory, int k);
void inner_decript(image_t * secret_image, image_t ** images, int k, int block_position, int image_count);
void decript_bytes_k2(image_t * secret_image, image_t ** images, int block_position, int image_count);
void decript_bytes_k3(image_t * secret_image, image_t ** images, int block_position, int image_count);
