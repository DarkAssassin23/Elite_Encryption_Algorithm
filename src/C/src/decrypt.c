#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "decrypt.h"

static const unsigned char PADDING = 0;

/**
* @brief Gets the previous block
* @param[in] data The data to decrypt
* @param[in] start The start index of the next block
* @param[out] key_block The key block that will be set as the previous block
* @param[in] key_len The length of the keys to use for decryption
*/
static void get_prev_block(unsigned char* data, size_t start, 
                    unsigned char** key_block, size_t key_len)
{
    // Setup the key_block as the second to last block
    memcpy(*key_block, &data[start], key_len);
}

/**
* @brief Handle encrypting cipher text that contains multiple blocks
*       where one block is the length of the key
* @param[in] data The data to decrypt
* @param[in] data_len The size of the data to decrypt
* @param[out] being_decrypted The resulting value of decryption
* @param[in] key_block The key block that will to decrypt the data
* @param[in] key_len The length of the keys to use for decryption
*/
static void decrypt_multi_block(unsigned char* data, size_t data_len,
                            unsigned char** being_decrypted, 
                            unsigned char* key_block, size_t key_len)
{
    // Allocate memory and set a temporary variables to handle 
    // data being modified in the function 
    unsigned char* temp = malloc(data_len);
    if(temp == NULL)
    {
        printf("Error: unable to allocate memory for decryption.\n");
        exit(EXIT_FAILURE);
    }
    memcpy(temp, *being_decrypted, data_len);

    size_t start = data_len - (key_len * 2);
    get_prev_block(data, start, &key_block, key_len);

    size_t count = 0;
    size_t key_index = key_len - 1;
    for(size_t x = data_len - 1; x >= key_len; x--)
    {
        // Once we have completed a block we
        // reset the key to one block before the one
        // we are about to xor
        if(count==key_len)
        {
            size_t prev_block_start = (x - ((key_len * 2) - 1));
            get_prev_block(data, prev_block_start, &key_block, key_len);
            count = 0;
            key_index = key_len - 1;
        }
        
        temp[x] = key_block[key_index] ^ data[x];
        key_index--;
        count++;
    }
    // Copy the data back to the output variable and free
    // our temporary variable
    memcpy(*being_decrypted, temp, data_len);
    free(temp);
}

/**
* @brief Remove excess padding at the end of the plain text added 
*   during encryption
* @param[in] plain_text The plain text with the padding to remove
* @param[in] plain_text_len The current length of the plain text
* @return The resulting length of the plain text after padding is removed
*/
static size_t remove_padding(unsigned char** plain_text, size_t plain_text_len)
{
    size_t plain_text_size = plain_text_len;
    
    unsigned char* temp = *plain_text;
    while(temp[plain_text_size - 1] == PADDING)
        plain_text_size--;

    *plain_text = realloc(*plain_text, (plain_text_size + 1)); 
    return plain_text_size;
}

size_t decrypt(unsigned char* data, size_t data_len, 
                unsigned char** plain_text, char** keys, int num_keys)
{
    size_t key_len = strlen(keys[0]);

    // Allocate memory for the plain text
    unsigned char* temp = malloc(data_len + 1);
    if(temp == NULL)
        return 0;

    unsigned char *key_block = malloc(key_len);
    for(int k = num_keys-1; k >= 0; k--)
    {
        // Set the data to decrypt to the result from the previous 
        // round of decryption
        if(k != (num_keys - 1))
            memcpy(data, temp, data_len);
        
        // Check to see if we have more than one block to decrypt
        if(data_len >= (key_len * 2))
            decrypt_multi_block(data, data_len, &temp, key_block, key_len);

        // We are at the last block, so the previous block is the key
        memcpy(key_block, keys[k], key_len);
        for(int x = (key_len - 1); x >= 0; x--)
            temp[x] = key_block[x] ^ data[x];
    }
    free(key_block);
    size_t plain_text_size = remove_padding(&temp, data_len);
    *plain_text = temp;
    return plain_text_size;
}