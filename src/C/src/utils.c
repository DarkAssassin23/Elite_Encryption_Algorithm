#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/rand.h>

#include "utils.h"

static const int MAX_TRIES = 5;

void message_digest_to_hash(unsigned char* md, char* hash, int digest_length)
{
    for(int x=0; x<digest_length; x++)
        sprintf(&hash[x*2], "%02hhx", md[x]);
}

char* get_random_hexstr(size_t size)
{
    unsigned char buffer[size];
    int tries = 0;
    int rc = 1;
    do
    {
        rc = RAND_bytes(buffer, sizeof(buffer));
        if(rc != 1)
            tries++;

    } while( rc != 1 && tries < MAX_TRIES);

    if(rc != 1)
    {
        printf("Error: exceeded the max tries to get random bytes\n");
        exit(rc);
    }

    char *hexstr = malloc((sizeof(buffer) * 2)+ 1);
    if(hexstr == NULL)
        return NULL;

    for(int x=0; x<sizeof(buffer); x++)
        sprintf(&hexstr[x*2], "%02hhx", buffer[x]);

    return hexstr;
}

char* keys_to_string(const char** keys, int num_keys)
{
    size_t single_key_len = strlen(keys[0]);
    // Add enough extra characters for new lines
    size_t keys_string_len = (single_key_len * num_keys) + num_keys;
    char* keys_string = malloc(keys_string_len + 1);
    if(keys_string == NULL)
        return NULL;

    for(int k = 0; k < num_keys; k++)
        sprintf(&keys_string[k * (single_key_len + 1)], "%s%s", 
            keys[k], (k+1 != num_keys) ? "\n" : "");
    
    keys_string[keys_string_len] = '\0';
    return keys_string;
}