#include <stdio.h>          // printf
#include <stdlib.h>         // free
#include <string.h>         // strlen
#include <openssl/sha.h>    // SHA256*, SHA512*

#include "utils.h"
#include "keygen.h"
#include "encrypt.h"

void test_keygen(void)
{
    for(int x = 0; x < 4; x++)
    {
        char *key = generate_key(x);
        printf("key: %s\n", key);
        free(key);
    }
}

void test_multiple_keygen(void)
{
    for(int x=1; x<=5; x++)
    {
        char** keys = generate_keys(HASH_TYPE_SHA512, x);
        printf("Keys:\n");
        for(int k=0; k<x; k++)
        {
            printf("%d.: %s\n", (k+1), keys[k]);
            free(keys[k]);
        }
        free(keys);
    }
}

void test_hashing(void)
{
    unsigned char md[SHA256_DIGEST_LENGTH];
    char hash[(SHA256_DIGEST_LENGTH * 2) + 1];
    char* hexstr = get_random_uint64_hexstr();

    SHA256((const unsigned char*)hexstr, strlen(hexstr), md);
    message_digest_to_hash(md, hash, SHA256_DIGEST_LENGTH);

    unsigned char md512[SHA512_DIGEST_LENGTH];
    char hash512[(SHA512_DIGEST_LENGTH * 2) + 1];

    SHA512((const unsigned char*)hexstr, strlen(hexstr), md512);
    message_digest_to_hash(md512, hash512, SHA512_DIGEST_LENGTH);

    printf("SHA256 hash of %s: %s\n", hexstr, hash);
    printf("SHA512 hash of %s: %s\n", hexstr, hash512);

    free(hexstr);
}

int main (int argc, char** argv)
{
    //test_hashing();
    //test_keygen();
    //test_multiple_keygen();


    int num_keys = 3;
    char** keys = generate_keys(HASH_TYPE_SHA512, num_keys);

    unsigned char* data = NULL;
    size_t file_size = read_in_file("file.txt", &data);
    printf("File contents: %s\n", data);
    printf("File size: %zu bytes\n", file_size);

    unsigned char* cipher_text = NULL;
    size_t cipher_text_size = encrypt(data, file_size, &cipher_text, 
                                        keys, num_keys);
    if(cipher_text == NULL)
    {
        printf("Error encrypting data\n");
        return -1;
    }
    
    printf("Keys:\n");
    for(int k=0; k<num_keys; k++)
    {
        printf("%d.: %s\n", (k+1), keys[k]);
        free(keys[k]);
    }

    if(!save_to_file("file.txt.eea", cipher_text, cipher_text_size))
        printf("Error saving all the data to the file\n");

    free(data);
    free(keys);
    free(cipher_text);
    return 0;
}