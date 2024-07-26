#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/sha.h>

#include "base64.h"
#include "encrypt.h"
#include "file_handling.h"
#include "globals.h"
#include "prompts.h"
#include "utils.h"

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
    while (cipher_text_len < data_len)
        cipher_text_len += key_len;

    return cipher_text_len;
}

size_t encrypt(unsigned char *data, size_t data_len,
               unsigned char **cipher_text, const char **keys, int num_keys)
{
    size_t key_len = strlen(keys[0]);
    size_t cipher_text_len = get_cipher_text_len(data_len, key_len);

    // Allocate memory for the cipher text
    unsigned char *temp = malloc(cipher_text_len + 1);
    if (temp == NULL)
        return 0;

    unsigned char prev_block[key_len];
    unsigned char key_block[key_len];
    // Iterate through each key
    for (int k = 0; k < num_keys; k++)
    {
        // Set the key_block equal to the current key
        memcpy(key_block, keys[k], key_len);
        for (int x = 0; x < cipher_text_len; x++)
        {
            // If the current index in the data to be encrypted is beyond
            // the length of the key use the previous block as the new key
            if (x != 0 && (x % key_len) == 0)
                memcpy(key_block, prev_block, key_len);

            unsigned char byte = 0;

            // Set the current byte to be XORed
            if (k == 0)
            {
                if (x < data_len)
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
    size_t encode_len = 0;
    int success = 1; // Assume encode success
    unsigned char *tmp = (unsigned char *) base64_encode(temp, cipher_text_len,
                                                         &encode_len);
    if (tmp == NULL)
    {
        tmp = temp;
        encode_len = cipher_text_len;
        success = 0;
    }
    *cipher_text = tmp;
    if (success)
        free(temp);
    return encode_len;
}

size_t encrypt_keys(char **keys, int num_keys,
                    unsigned char **encrypted_string)
{
    // We are creating a password
    char *password_hash = get_hashed_password(1);
    if (password_hash == NULL)
        return 0;

    size_t key_size = strlen(keys[0]);

    // Add salt to harden the encryption since we are only using
    // the one key that comes from the password
    char *salt = get_random_hexstr(key_size / 2);
    if (salt == NULL)
    {
        free(password_hash);
        return 0;
    }

    char *keys_string = keys_to_string((const char **) keys, num_keys);
    if (keys_string == NULL)
    {
        free(password_hash);
        free(salt);
        return 0;
    }

    size_t encrypted_keys_len = (key_size + 1) + strlen(keys_string);
    unsigned char *encrypted_keys = malloc(encrypted_keys_len);
    if (encrypted_keys == NULL)
    {
        free(password_hash);
        free(salt);
        free(keys_string);
        return 0;
    }

    memcpy(encrypted_keys, salt, strlen(salt));
    encrypted_keys[key_size] = '\n';
    memcpy(encrypted_keys + (key_size + 1), keys_string, strlen(keys_string));

    free(salt);
    free(keys_string);
    for (int x = 0; x < ROUNDS; x++)
    {
        unsigned char *unchanged = encrypted_keys;
        encrypted_keys_len = encrypt(unchanged, encrypted_keys_len,
                                     &encrypted_keys,
                                     (const char **) &password_hash, 1);
        free(unchanged);
    }
    *encrypted_string = encrypted_keys;
    free(password_hash);
    return encrypted_keys_len;
}

int encrypt_file(const char *filename, const char **keys, int num_keys)
{
    int success = 1;
    unsigned char *data = NULL;
    size_t file_size = read_in_file(filename, &data);
    if (file_size == -1)
        return 0;

    unsigned char *cipher_text = NULL;
    size_t cipher_text_size = encrypt(data, file_size, &cipher_text, keys,
                                      num_keys);
    if (cipher_text == NULL)
        return 0;

    char *output_file = get_output_filename(filename, 1);

    if (!save_to_file(output_file, cipher_text, cipher_text_size))
    {
        fprintf(stderr, "%sError:%s Saving data to the file failed\n",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
        success = 0;
    }

    free(data);
    free(cipher_text);
    free(output_file);
    return success;
}
