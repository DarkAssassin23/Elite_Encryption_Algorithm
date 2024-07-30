#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/sha.h>

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

char *generate_key(HASH_TYPE hash_type)
{
    if (hash_type == HASH_TYPE_SHA256)
        return get_sha256_key();

    size_t hashlen = 128;
    size_t currlen = 0;
    char *key = NULL;

    int i = 0;
    if (hash_type == HASH_TYPE_SHA2048)
        i--;

    do
    {
        char *tmp = realloc(key, ((currlen + hashlen) + 1));
        if (tmp == NULL)
        {
            if (key != NULL)
                free(key);
            return NULL;
        }
        key = tmp;
        char *hash = get_sha512_key();
        if (hash == NULL)
        {
            free(key);
            return NULL;
        }

        sprintf(&key[currlen], "%s", hash);
        currlen += hashlen;
        free(hash);
        i++;
    } while (i < hash_type);
    return key;
}

char **generate_keys(HASH_TYPE hash_type, int num_keys)
{
    if (num_keys < 1)
        return NULL;

    char **keys = malloc(sizeof(char *) * num_keys);
    if (keys == NULL)
        return NULL;

    for (int x = 0; x < num_keys; x++)
    {
        keys[x] = generate_key(hash_type);
        if (keys[x] == NULL)
        {
            free_keys(keys, num_keys, NULL);
            return NULL;
        }
    }

    return keys;
}
