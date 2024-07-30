#pragma once

/**
 * @enum HashType enum of the diffrent hash types
 * @brief Types of available hash functions
 * @details Types of hash functions where SHA1024 and 2048
 * are a collection of SHA512 hashes to get to the length of what a
 * theoretical 1024 and 2048 hash would be (128 and 256 bytes respectively)
 */
typedef enum
{
    HASH_TYPE_SHA256 = 0,
    HASH_TYPE_SHA512 = 1,
    HASH_TYPE_SHA1024 = 2,
    HASH_TYPE_SHA2048 = 3,
    HASH_TYPE_OTHER = 4
} HashType;

/**
 * @brief Create a single key to use for encryption/decryption
 * @param[in] hash_size The size of the key to generate
 * @return A single key
 */
char *generate_key(size_t hash_size);

/**
 * @brief Create an array of keys to use for encryption/decryption
 * @param[in] hash_size The size of keys to generate
 * @param[in] num_keys The number of keys to generate
 * @return The array of keys
 */
char **generate_keys(size_t hash_size, int num_keys);
