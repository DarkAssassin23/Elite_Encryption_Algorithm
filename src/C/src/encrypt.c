#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

#include "globals.h"
#include "utils.h"
#include "encrypt.h"

/**
* @brief Return the size of how big the resulting cipher text will be
* @param[in] data_len The size data that will be encrypted
* @param[in] key_len The size of the keys that will be used to encrypt
* @return The size of the resulting cipher_text
*/
static size_t get_cipher_text_len(size_t data_len, size_t key_len)
{
    size_t cipher_text_len = key_len;

    // The cipher text must be a multiple of the key length
    while(cipher_text_len < data_len)
        cipher_text_len += key_len;

    return cipher_text_len;
}

size_t encrypt(unsigned char* data, size_t data_len, 
                unsigned char** cipher_text, const char** keys, int num_keys)
{
    size_t key_len = strlen(keys[0]);
    size_t cipher_text_len = get_cipher_text_len(data_len, key_len);

    // Allocate memory for the cipher text
    unsigned char* temp = malloc(cipher_text_len + 1);
    if(temp == NULL)
        return 0;

    unsigned char prev_block[key_len];
    unsigned char key_block[key_len];
    // Iterate through each key
    for(int k = 0; k < num_keys; k++)
    {
        // Set the key_block equal to the current key
        memcpy(key_block, keys[k], key_len);
        for(int x = 0; x < cipher_text_len; x++)
        {
            // If the current index in the data to be encrypted is beyond 
            // the length of the key use the previous block as the new key
            if(x != 0 && (x % key_len) == 0)
                memcpy(key_block, prev_block, key_len);
            
            unsigned char byte = 0;

            // Set the current byte to be XORed
            if(k == 0)
            {
                if(x < data_len)
                    byte = data[x];
                else
                    byte = PADDING;
            }
            else
                byte = temp[x];

            // XOR the byte with the current byte in the key
            temp[x] = key_block[x % key_len] ^ byte;
            prev_block[x % key_len] = key_block[x % key_len] ^ byte;
        }
    }
    *cipher_text = temp;
    return cipher_text_len;
}

size_t encrypt_keys(char* keys_string, unsigned char** encrypted_string)
{
    char* password_hash = get_hashed_password();
    if(password_hash == NULL)
        return 0;

    unsigned char* encrypted_keys = (unsigned char*)strdup(keys_string);
    size_t encrypted_keys_len = strlen(keys_string);
    for(int x = 0; x < ROUNDS; x++)
    {
        encrypted_keys_len = encrypt(encrypted_keys, 
                    encrypted_keys_len, &encrypted_keys, 
                    (const char**)&password_hash, 1);
    }
    *encrypted_string = encrypted_keys;
    free(password_hash);
    return encrypted_keys_len;
}