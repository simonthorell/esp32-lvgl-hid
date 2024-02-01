#ifndef AES_ENCRYPT_H
#define AES_ENCRYPT_H

#include <stdint.h>
#include <string.h>

// Function to perform AES-256 CBC encryption on three blocks of 16 bytes each
// Input: input_data - an array of three 16-byte blocks (48 bytes in total)
//        key - a 256-bit (32-byte) encryption key
//        iv - a 128-bit (16-byte) initialization vector
// Output: encrypted_data - a pointer to the encrypted data (must be pre-allocated)
// Returns 0 on success, -1 on failure
int aes256_cbc_encrypt(const uint8_t* input_data, const uint8_t* key, const uint8_t* iv, uint8_t* encrypted_data);

#endif // AES_ENCRYPT_H