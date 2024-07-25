#include <stdlib.h>

#include <openssl/bio.h>
#include <openssl/evp.h>

#include "base64.h"

char *base64_encode(unsigned char *data, size_t size, size_t *rsize)
{
    *rsize = EVP_ENCODE_LENGTH(size);
    char *encoded = calloc(*rsize + 1, sizeof(char));
    if (encoded == NULL)
    {
        *rsize = 0;
        return NULL;
    }

    *rsize = EVP_EncodeBlock((unsigned char *) encoded, data, (int) size);

    if (!*rsize)
    {
        free(encoded);
        encoded = NULL;
    }
    return encoded;
}

unsigned char *base64_decode(unsigned char *data, size_t size, size_t *rsize)
{
    *rsize = EVP_DECODE_LENGTH(size);
    unsigned char *decoded = calloc(*rsize + 1, sizeof(char));
    if (decoded == NULL)
    {
        *rsize = 0;
        return NULL;
    }

    int ret = EVP_DecodeBlock(decoded, data, (int) size);

    // Decode failed
    if (ret == -1)
    {
        *rsize = 0;
        free(decoded);
        decoded = NULL;
    }

    return decoded;
}
