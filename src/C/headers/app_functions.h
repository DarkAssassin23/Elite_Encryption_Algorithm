#pragma once

/**
* @brief Encrypt the given file with the given keys
* @param[in] filename The file to be encrypted
* @param[in] keys The keys to be used for encryption
* @param[in] num_keys The number of keys that are being used
* @return If the file was encrypted successfully 
*/
int encrypt_file(const char* filename, const char** keys, int num_keys);

/**
* @brief Decrypt the given file with the given keys
* @param[in] filename The file to be decrypted
* @param[in] keys The keys to be used for decryption
* @param[in] num_keys The number of keys that are being used
* @return If the file was decrypted successfully 
*/
int decrypt_file(const char* filename, const char** keys, int num_keys);

/**
* @brief Manage the keys used for encryption and decryption
*/
void manage_keys(void);
