#pragma once

/**
* @brief Decrypt the given data with the given keys
* @param[in] data The data to decrypt
* @param[in] data_len The size of the data to decrypt
* @param[out] plain_text The resulting plain text post decryption
* @param[in] keys The keys to use for decryption
* @param[in] num_keys The number of keys being used for decryption
* @return Length of plain text
*/
size_t decrypt(unsigned char* data, size_t data_len, 
                unsigned char** cipher_text, const char** keys, int num_keys);