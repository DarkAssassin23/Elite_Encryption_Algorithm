#pragma once

/**
* @enum FILE_TYPE
* @brief The kind of value returned from get_file_type()
*/
typedef enum
{
    FILE_TYPE_NA = -2,
    FILE_TYPE_OTHER = -1,
    FILE_TYPE_DIR = 0,
    FILE_TYPE_REG = 1
}FILE_TYPE;

/**
* @brief Get the name of the output file based on if you are encrypting or
*   decrypting
* @param[in] filename Current name of the file
* @param[in] encrypting If you are encrypting this file
* @return Name of the output file
* @note Return value must be freed
*/
char* get_output_filename(const char* filename, int encrypting);

/**
* @brief Prompt the user for the name of the file they would like to encrypt
* @param[in] encrypting If this file will be encrypted
* @return The name of the file to encrypt/decrypt
* @note Return value must be freed
*/
char* get_input_filename(int encrypting);

/**
* @brief Load in the keys for encryption form they key file
* @param[in] filename The name of the file to load the keys from
* @param[out] total_keys The number of keys loaded in from the file
* @param[out] key_len The length of an individual key
* @return The keys loaded in from the file
* @note The keys must be freed
*/
char** load_keys_from_file(const char* filename, int* total_keys, size_t* len);

/**
* @brief Checks to see if the file exists
* @param[in] filename File to see if it exists
* @return If the file exists
*/
int file_exists(const char* filename);

/**
* @brief Checks to see what kind of file type it is
* @param[in] path Path to file/directory to see its file type
* @return -2 - file doesn't exist, 
*         -1 - other,
*          0 - directory, 
*          1 - regular file, 
*/
int get_file_type(const char* path);

/**
* @brief Check if the given file is of the given file extention
* @param[in] filename The file to check
* @param[in] extention The extention to check for
* @return If the given file is of the extention type
*/
int is_of_filetype(const char* filename, const char* extention);

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