#pragma once

/**
* @brief Convert the message digest returned by SHA256 and SHA512 into a 
*   hex string
* @param[in] md The message digest to convert to a hex string
* @param[out] hash The value to set the result hex string to
* @param[in] digest_length The length of the message digest
*/
void message_digest_to_hash(unsigned char* md, char* hash, int digest_length);

/**
* @brief Create a randomized hex number the size of a uint64_t and return 
*   it as a string
* @return Random uint64_t as a hex string
*/
char* get_random_uint64_hexstr(void);

/**
* @brief Checks to see if the file exists
* @param[in] filename File to see if it exists
* @return If the file exists
*/
int file_exists(const char* filename);

/**
* @brief Check if the given file is a keys file
* @param[in] filename The file to check if it is a keys file
* @return If the given file is a keys file
*/
int is_keys_file(const char* filename);

/**
* @brief Checks to see if any keys files exist
* @return If any keys files exists in the current directory
*/
int keys_file_exists(void);

/**
* @brief Gets all the keys files in the current directory
* @param[out] key_files_count The number of keys files found
* @return The list of keys files in the current directory
* @note Return value must be freed
*/
char** get_all_keys_files(size_t* key_files_count);

/**
* @brief Convert the array of keys to a single string so they can be 
*   written to a file
* @param[in] keys The keys that will be saved
* @param[in] num_keys The number of keys that will be saved
* @return The keys as a single string
* @note The returned string must be freed 
*/
char* keys_to_string(const char** keys, int num_keys);

/**
* @brief Save the cipher text to a file
* @param[in] filename The file to save the data to
* @param[in] data The data to be written to the file
* @param[in] bytes_to_write The number of bytes to write to the file
* @return If the save was successful
*/
int save_to_file(const char* filename, unsigned char* data, size_t bytes_to_write);

/**
* @brief Read in data from a file
* @param[in] filename The file to read the data from
* @param[in] buffer The buffer store the contents of the file in
* @return The number of bytes read in, -1 if the reading failed
*/
size_t read_in_file(const char* filename, unsigned char** buffer);