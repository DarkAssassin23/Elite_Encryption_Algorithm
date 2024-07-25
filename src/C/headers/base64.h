#pragma once
#include <stddef.h>

/**
 * @brief Given some data, encode it in base64
 * @param[in] data The data to be encoded
 * @param[in] size The size of the data being encoded
 * @param[out] rsize The resulting size of the encoded data
 * @return the data encoded in base64
 * @note Resulting value must be freed if it is not NULL
 */
char *base64_encode(unsigned char *data, size_t size, size_t *rsize);

/**
 * @brief Given some data, decode it from base64
 * @param[in] data The data to be decoded
 * @param[in] size The size of the data being decoded
 * @param[out] rsize The resulting size of the decoded data
 * @return the data decoded from base64
 * @note Resulting value must be freed if it is not NULL
 */
unsigned char *base64_decode(unsigned char *data, size_t size, size_t *rsize);
