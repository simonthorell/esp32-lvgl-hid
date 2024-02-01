#ifndef AES_ENCRYPT_H
#define AES_ENCRYPT_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define AES_BLOCK_SIZE 16

int init_random();
int aes256_encrypt_cbc(const uint8_t *input, size_t input_len, uint8_t *key, uint8_t *iv, uint8_t *output);
void generate_random_iv(uint8_t* iv, size_t iv_size);
char* base64_encode(const uint8_t* data, size_t input_length);

#endif // AES_ENCRYPT_H