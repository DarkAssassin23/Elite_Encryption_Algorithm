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
size_t decrypt(unsigned char *data, size_t data_len,
               unsigned char **cipher_text, const char **keys, int num_keys);

/**
 * @brief Decrypt the keys prior to using them to a with the
 * password used to encrypt them
 * @param[in] encrypted_string The string of keys encrypted
 * @param[in] encrypted_size The size of the encrypted string of keys
 * @param[out] keys_string The string containing all the keys decrypted
 * @note keys_string must be freed
 * @return Length of keys string
 * @attention This function assumes the password entered is the same as
 * the one used to encrypt the keys. i.e. the keys you get back could be
 * bogus or invalid.
 */
size_t decrypt_keys(unsigned char *encrypted_string, size_t encrypted_size,
                    char **keys_string);

/**
 * @brief Decrypt the given file with the given keys
 * @param[in] filename The file to be decrypted
 * @param[in] keys The keys to be used for decryption
 * @param[in] num_keys The number of keys that are being used
 * @return If the file was decrypted successfully
 */
int decrypt_file(const char *filename, const char **keys, int num_keys);
