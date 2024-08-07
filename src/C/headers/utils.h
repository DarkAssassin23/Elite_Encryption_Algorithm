#pragma once

/**
 * @brief Convert the message digest returned by SHA256 and SHA512 into a
 * hex string
 * @param[in] md The message digest to convert to a hex string
 * @param[out] hash The value to set the result hex string to
 * @param[in] digest_length The length of the message digest
 */
void message_digest_to_hash(unsigned char *md, char *hash, int digest_length);

/**
 * @brief Create a randomized hex number and return it as a string
 * @param[in] size The number of randomized bytes to create
 * @return Random hex number as a hex string
 */
char *get_random_hexstr(size_t size);

/**
 * @brief Convert the array of keys to a single string so they can be
 * written to a file
 * @param[in] keys The keys that will be saved
 * @param[in] num_keys The number of keys that will be saved
 * @return The keys as a single string
 * @note The returned string must be freed
 */
char *keys_to_string(const char **keys, int num_keys);

/**
 * @brief Split the given string into an array of strings based on
 * the delimiter
 * @param[in] str The string to split
 * @param[in] delim The delimiter to split the string on
 * @param[out] size The resulting arrays size
 * @return The array of strings split on the delimiter
 * @note The returned string array must be freed
 */
char **split_string(char *str, const char *delim, int *size);

/**
 * @brief Trim the leading and trailing white space from the string
 * @param[in] str The string to trim
 * @return The string with no leading or trailing whitespace
 */
char *trim(char *str);

/**
 * @brief Check if a given string starts with another string
 * @param[in] str The string we are checking
 * @param[in] start The sequence to see if str starts with
 * @return If `str` starts with `start`
 */
int starts_with(const char *str, const char *start);

/**
 * @brief Check if a given string ends with another string
 * @param[in] str The string we are checking
 * @param[in] start The sequence to see if str ends with
 * @return If `str` ends with `end`
 */
int ends_with(const char *str, const char *end);

/**
 * @brief Checks to see if they keys are valid
 * @param[in] keys The keys that will be checked
 * @param[in] num_keys The number of keys
 * @return If they keys are valid
 */
int validate_keys(const char **keys, int num_keys);

/**
 * @brief Find the length of each key, given a string of keys
 * @param[in] keys_string The string of keys
 * @return The length of the keys
 */
size_t find_key_len(const char *keys_string);

/**
 * @brief Load in the users preferred keys from a keys file
 * @param[out] num_keys The number of keys read in
 * @return List of keys
 * @note List of keys must be freed
 */
char **load_keys(int *num_keys);

/**
 * @brief Free the list of keys
 * @param[in] keys The list of keys to be freed
 * @param[in] num The number of keys in the list
 * @param[in] print Text to display for printing the keys
 */
void free_keys(char **keys, int num, const char *print);

/**
 * @brief Resize the buffer if needed
 * @param buffer The buffer to resize
 * @param max The current maximum size of the buffer
 * @param required The minimum size the buffer needs to be
 * @return 0 on success, 1 if something went wrong
 */
int buff_resize(char **buffer, size_t *max, size_t required);
