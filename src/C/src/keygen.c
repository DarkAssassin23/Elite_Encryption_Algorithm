#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/sha.h>

#include "globals.h"
#include "keygen.h"
#include "utils.h"

static char *get_sha256_key(void)
{
    char *hash = malloc((SHA256_DIGEST_LENGTH * 2) + 1);
    if (hash == NULL)
        return NULL;

    unsigned char md[SHA256_DIGEST_LENGTH];
    char *hexstr = get_random_hexstr(SHA256_DIGEST_LENGTH);
    if (hexstr == NULL)
    {
        free(hash);
        return NULL;
    }

    SHA256((const unsigned char *) hexstr, strlen(hexstr), md);
    message_digest_to_hash(md, hash, SHA256_DIGEST_LENGTH);

    free(hexstr);
    return hash;
}

static char *get_sha512_key(void)
{
    char *hash = malloc((SHA512_DIGEST_LENGTH * 2) + 1);
    if (hash == NULL)
        return NULL;

    unsigned char md[SHA512_DIGEST_LENGTH];
    char *hexstr = get_random_hexstr(SHA256_DIGEST_LENGTH);
    if (hexstr == NULL)
    {
        free(hash);
        return NULL;
    }

    SHA512((const unsigned char *) hexstr, strlen(hexstr), md);
    message_digest_to_hash(md, hash, SHA512_DIGEST_LENGTH);

    free(hexstr);
    return hash;
}

char *generate_key(size_t hash_size)
{
    if (hash_size % MIN_KEY_BITS != 0)
        return NULL;

    size_t curr_len = 0, curr_size = 0;
    const int sha512_len = 128, sha256_len = 64;
    char *key = NULL;
    while (curr_size < hash_size)
    {
        int inc = ((hash_size - curr_size) > MIN_KEY_BITS) ? sha512_len
                                                           : sha256_len;
        char *tmp = realloc(key, (curr_len + inc + 1));
        if (tmp == NULL)
        {
            if (key != NULL)
                free(key);
            return NULL;
        }
        key = tmp;
        char *hash = (inc == sha256_len) ? get_sha256_key() : get_sha512_key();
        if (hash == NULL)
        {
            free(key);
            return NULL;
        }

        sprintf(&key[curr_len], "%s", hash);
        curr_len += inc;
        curr_size += (inc == sha256_len) ? MIN_KEY_BITS : MIN_KEY_BITS * 2;
        free(hash);
    }
    return key;
}

char **generate_keys(size_t hash_size, int num_keys)
{
    if (num_keys < 1 && hash_size % MIN_KEY_BITS != 0)
        return NULL;

    char **keys = malloc(sizeof(char *) * num_keys);
    if (keys == NULL)
        return NULL;

    for (int x = 0; x < num_keys; x++)
    {
        keys[x] = generate_key(hash_size);
        if (keys[x] == NULL)
        {
            for (int i = x - 1; i >= 0; i--)
                free(keys[x]);
            free(keys);
            return NULL;
        }
    }

    return keys;
}
