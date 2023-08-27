#pragma once

/**
* @brief Prompt for the name of the file the keys should be saved too
* @param[out] filename Variable to set the filename too
* @return If the file is valid, -1 for the user exiting
* @note Value passed int must be freed
*/
int prompt_for_keys_filename(char** filename);

/**
* @brief Prompt for how many keys should be generated
* @return The number of keys to generate, -1 for the user exiting
*/
int prompt_for_num_keys(void);

/**
* @brief Warn the user they are about to use ghost mode and ask if they
*       wish to proceed
* @return Whether or not to proceed with ghost mode
*/
int prompt_for_ghost_mode_confirmation(void);

/**
* @brief Prompt the user for the keys to use for ghost mode decryption
* @param[out] keys The keys that the user entered
* @param[out] num_keys The number of keys the user entered
* @return Whether or not the keys were valid
*/
int prompt_for_ghost_mode_keys(char*** keys, int* num_keys);

/**
* @brief Prompt the user for a password and return the result
* @param[in] prompt The prompt to display to the user
* @return The password entered by the user
* @note The returned password must be freed
*/
char* get_password(const char* prompt);

/**
* @brief Prompt the user for a password and return the result as a hash
* @param[in] set_password Is the password being created or just entered
* @return The password entered by the user as a hash
* @note The returned password must be freed
*/
char* get_hashed_password(int set_password);