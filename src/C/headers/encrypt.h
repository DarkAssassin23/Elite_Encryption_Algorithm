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
* @param[in] keys The keys to be encrypted
* @param[in] num_keys The number of keys being encrypted
* @param[out] encrypted_string The string of keys encrypted
* @note encrypted_string must be freed
* @return Length of encrypted key string
*/
size_t encrypt_keys(char** keys, int num_keys, unsigned char** encrypted_string);

/**
* @brief Encrypt the given file with the given keys
* @param[in] filename The file to be encrypted
* @param[in] keys The keys to be used for encryption
* @param[in] num_keys The number of keys that are being used
* @return If the file was encrypted successfully 
*/
int encrypt_file(const char* filename, const char** keys, int num_keys);