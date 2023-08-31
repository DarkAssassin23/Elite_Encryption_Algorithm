#pragma once

/**
* @brief Function to spin up threads to encrypt multiple files at once
* @param[in] files_list The list files to be encrypted
* @param[in] num_files The number of files to be encrypted
* @param[in] keys The keys to be used for encryption
* @param[in] num_keys The number of keys
* @param[in] overwrite Should the files be overwritten
* @param[in] threads The number of threads to use (default: 1)
* @note The array of files will be freed
*/
void start_dir_encrypt_threads(char** files_list, int num_files,
                                const char** keys, int num_keys, 
                                int overwrite, int threads);

/**
* @brief Function to spin up threads to decrypt multiple files at once
* @param[in] files_list The list files to be decrypted
* @param[in] num_files The number of files to be decrypted
* @param[in] keys The keys to be used for decryption
* @param[in] num_keys The number of keys
* @param[in] overwrite Should the files be overwritten
* @param[in] threads The number of threads to use (default: 1)
* @note The array of files will be freed
*/
void start_dir_decrypt_threads(char** files_list, int num_files,
                                const char** keys, int num_keys, 
                                int overwrite, int threads);