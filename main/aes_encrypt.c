#include "aes_encrypt.h"
#include "mbedtls/aes.h"
#include "mbedtls/error.h"

//======================================================================================================================
// Function: encrypt_string
// Purpose: Encrypt a string using AES-128-CBC
// Parameters: key - 16 byte key, iv - 16 byte initialization vector, input - string to encrypt
// Returns: Encrypted string in hexadecimal format
//======================================================================================================================

int aes256_cbc_encrypt(const uint8_t* input_data, const uint8_t* key, const uint8_t* iv, uint8_t* encrypted_data) {
    // Initialize mbedtls context for AES-256
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);

    // Set AES key
    mbedtls_aes_setkey_enc(&aes, key, 256);

    // Define input and output buffers
    uint8_t input[16]; // AES block size is 16 bytes
    uint8_t output[16]; // AES block size is 16 bytes
    mbedtls_aes_context ctx;
    mbedtls_aes_init(&ctx);

    // Set IV (Initialization Vector)
    mbedtls_aes_setkey_enc(&ctx, key, 256);
    uint8_t temp_iv[16]; // Temporary IV storage

    // Initialize IV with the provided IV
    memcpy(temp_iv, iv, 16);

    for (int i = 0; i < 3; i++) {
        // Copy the current block from input_data to input buffer
        memcpy(input, input_data + (i * 16), 16);

        // XOR the current block with the IV
        for (int j = 0; j < 16; j++) {
            input[j] ^= temp_iv[j];
        }

        // Perform encryption
        mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, 16, temp_iv, input, output);

        // Copy the encrypted data to the output buffer
        memcpy(encrypted_data + (i * 16), output, 16);
    }

    // Clean up mbedtls contexts
    mbedtls_aes_free(&aes);
    mbedtls_aes_free(&ctx);

    return 0; // Success
}