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