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
* @brief Create a randomized hex number and return it as a string
* @param[in] size The number of randomized bytes to create
* @return Random hex number as a hex string
*/
char* get_random_hexstr(size_t size);

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
* @brief Checks to see if they keys are valid
* @param[in] keys The keys that will be checked
* @param[in] num_keys The number of keys
* @return If they keys are valid
*/
int validate_keys(const char** keys, int num_keys);

/**
* @brief Find the length of each key, given a string of keys
* @param[in] keys_string The string of keys
* @return The length of the keys
*/
size_t find_key_len(const char* keys_string);