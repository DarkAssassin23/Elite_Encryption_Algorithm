#pragma once

/**
 * @enum HASH_TYPE enum of the diffrent hash types
 * @brief Types of available hash functions
 * @details Types of hash functions where SHA1024 and 2048
 *   are a collection of SHA512 hashes to get to the length of what a
 *   theoretical 1024 and 2048 hash would be (128 and 256 bytes respectively)
 */
typedef enum
{
    HASH_TYPE_SHA256 = 0,
    HASH_TYPE_SHA512 = 1,
    HASH_TYPE_SHA1024 = 2,
    HASH_TYPE_SHA2048 = 3
} HASH_TYPE;

/**
 * @brief Create a single key to use for encryption/decryption
 * @param[in] hash_type The kind of SHA has to use
 * @return A single key
 * @see HASH_TYPE enum for more info on the hash_type parameter
 */
char *generate_key(HASH_TYPE hash_type);

/**
 * @brief Create an array of keys to use for encryption/decryption
 * @param[in] hash_type The kind of SHA has to use
 * @param[in] num_keys The number of keys to generate
 * @return The array of keys
 * @see HASH_TYPE enum for more info on the hash_type parameter
 */
char **generate_keys(HASH_TYPE hash_type, int num_keys);
