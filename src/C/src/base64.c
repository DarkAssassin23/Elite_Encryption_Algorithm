#include <stdlib.h>
#include <string.h>

#include <openssl/bio.h>
#include <openssl/evp.h>

#include "base64.h"

/**
 * @brief Calculates the length of a encoded base64 string
 * @param[in] size The current size of the data to be encoded
 * @return The length of the data once encoded
 * @ref: https://stackoverflow.com/a/32140193
 */
static size_t calc_encode_length(const size_t size)
{
    return ((4 * size / 3) + 3) & ~3;
}

char *base64_encode(unsigned char *data, size_t size, size_t *rsize)
{
    *rsize = calc_encode_length(size);
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

/**
 * @brief Calculates the length of a decoded base64 string
 * @param[in] b64input The encoded base64 string
 * @param[in] size Max size of the base64 encoded string
 * @return The length of the data once decoded
 * @ref: https://gist.github.com/barrysteyn/4409525
 */
static size_t calc_decode_length(const char *b64input, const size_t size)
{
    size_t len = strnlen(b64input, size);
    int padding = 0;

    // Last two chars are =
    if (b64input[len - 1] == '=' && b64input[len - 2] == '=')
        padding = 2;
    else if (b64input[len - 1] == '=') // last char is =
        padding = 1;

    return (len * 0.75) - padding;
}

unsigned char *base64_decode(unsigned char *data, size_t size, size_t *rsize)
{
    *rsize = calc_decode_length((char *) data, size);
    unsigned char *decoded = calloc(*rsize + 2, sizeof(char));
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
