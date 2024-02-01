#include "aes_encrypt.h"
#include <stdlib.h>
#include <time.h>
#include <mbedtls/aes.h>
#include <mbedtls/base64.h>
#include <mbedtls/error.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>

static mbedtls_entropy_context entropy;
static mbedtls_ctr_drbg_context ctr_drbg;

//======================================================================================================================
// Function: init_random
// Purpose: Initializes the random number generator
// Parameters: None
// Returns: 0 on success, non-zero on failure
//======================================================================================================================
int init_random() {
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    // Personalization data can be added here if needed
    const unsigned char pers[] = "aes_random";
    int ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, pers, sizeof(pers));
    
    return ret;
}

//======================================================================================================================
// Function: aes256_encrypt_cbc
// Purpose: Encrypt data using AES-256-CBC
// Parameters: input - data to encrypt, input_len - length of the data, key - 32 byte key, iv - 16 byte IV, output - encrypted data
// Returns: 0 on success, non-zero on failure
//======================================================================================================================
int aes256_encrypt_cbc(const uint8_t *input, size_t input_len, uint8_t *key, uint8_t *iv, uint8_t *output) {
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);

    int ret = mbedtls_aes_setkey_enc(&aes, key, 256);
    if (ret != 0) {
        mbedtls_aes_free(&aes);
        return ret;
    }

    ret = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, input_len, iv, input, output);
    mbedtls_aes_free(&aes);

    return ret;
}

//======================================================================================================================
// Function: generate_random_iv
// Purpose: Generates a cryptographically secure random IV
// Parameters: iv - buffer to store the IV, iv_size - size of the buffer
// Returns: None
//======================================================================================================================
void generate_random_iv(uint8_t* iv, size_t iv_size) {
    mbedtls_ctr_drbg_random(&ctr_drbg, iv, iv_size);
}

//======================================================================================================================
// Function: base64_encode
// Purpose: Encodes data to Base64 format
// Parameters: data - data to encode, input_length - length of the data
// Returns: Base64 encoded string, or NULL on failure
//======================================================================================================================
char* base64_encode(const uint8_t* data, size_t input_length) {
    size_t output_length = 0;
    // Calculate output length first
    mbedtls_base64_encode(NULL, 0, &output_length, data, input_length);

    // Allocate memory for the output
    char* encoded_data = (char*)malloc(output_length); // +1 not required as mbedtls accounts for the null terminator
    if (encoded_data == NULL) return NULL;

    if (mbedtls_base64_encode((unsigned char*)encoded_data, output_length, &output_length, data, input_length) != 0) {
        free(encoded_data);
        return NULL;
    }

    return encoded_data; // Output is null-terminated
}