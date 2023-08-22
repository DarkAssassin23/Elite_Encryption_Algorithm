#pragma once

/**
* @brief Encrypt the given data with the given keys
* @param[in] data The data to encrypt
* @param[in] data_len The size of the data to encrypt
* @param[out] cipher_text The resulting cipher text post encryption
* @param[in] keys The keys to use for encryption
* @param[in] num_keys The number of keys being used for encryption
* @return Length of cipher text
*/
size_t encrypt(unsigned char* data, size_t data_len, 
                unsigned char** cipher_text, const char** keys, int num_keys);

/**
* @brief Encrypt the keys prior to saving them to a file with a password
* @param[in] keys_string The string containing all the keys
* @param[out] encrypted_string The string of keys encrypted
* @note encrypted_string must be freed
* @return Length of encrypted key string
*/
size_t encrypt_keys(char* keys_string, unsigned char** encrypted_string);