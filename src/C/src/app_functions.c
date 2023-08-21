#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "args.h"
#include "keygen.h"
#include "encrypt.h"
#include "decrypt.h"
#include "app_functions.h"

// TODO: make a better name
static const char DEFAULT_KEYS_FILE[] = "keys.keys";

char* get_output_filename(const char* filename, int encrypting)
{
    char* output_filename = NULL;
    char extention[] = ".eea";

    if(encrypting)
    {
        output_filename = malloc(strlen(filename) + sizeof(extention) + 1);
        if(output_filename == NULL)
            return NULL;

        sprintf(output_filename, "%s%s", filename, extention);
        return output_filename;
    }

    size_t output_filename_len = strlen(filename) - sizeof(extention) + 1;
    output_filename = malloc(output_filename_len + 1);
    if(output_filename == NULL)
            return NULL;

    strncpy(output_filename, filename, output_filename_len);
    output_filename[output_filename_len] = '\0';

    return output_filename;
}

int encrypt_file(const char* filename, const char** keys, int num_keys)
{
    int success = 1;
    // int num_keys = 3;
    // char** keys = generate_keys(HASH_TYPE_SHA256, num_keys);

    unsigned char* data = NULL;
    size_t file_size = read_in_file(filename, &data);
    if(file_size == -1)
        return 0;

    // printf("File contents: %s\n", data);
    // printf("File size: %zu bytes\n", file_size);

    unsigned char* cipher_text = NULL;
    size_t cipher_text_size = encrypt(data, file_size, &cipher_text, 
                                        keys, num_keys);
    if(cipher_text == NULL)
        return 0;

    char* output_file = get_output_filename(filename, 1);

    // printf("Cipher text: %s\n", cipher_text);
    // printf("Cipher text size: %zu\n", cipher_text_size);
    if(!save_to_file(output_file, cipher_text, cipher_text_size))
    {
        printf("Error saving data to the file\n");
        success = 0;
    }

    free(data);
    free(cipher_text);
    free(output_file);
    return success;
}

int decrypt_file(const char* filename, const char** keys, int num_keys)
{
    int success = 1;
    unsigned char* plain_text = NULL;
    unsigned char* cipher_text = NULL;
    size_t file_size = read_in_file(filename, &cipher_text);
    if(file_size == -1)
        return 0;

    size_t plain_text_size = decrypt(cipher_text, file_size, &plain_text, keys, num_keys);
    if(plain_text == NULL)
        return 0;

    // printf("Plain text: %s\n", (char*)plain_text);
    // printf("Plain text size: %zu\n", plain_text_size);

    char* output_file = get_output_filename(filename, 0);
    if(!save_to_file(output_file, plain_text, plain_text_size))
    {
        printf("Error saving data to the file\n");
        success = 0;
    }

    free(cipher_text);
    free(plain_text);
    free(output_file);
    return success;
}

/**
* @brief Convert the array of keys to a single string so they can be 
*   written to a file
* @param[in] keys The keys that will be saved
* @param[in] num_keys The number of keys that will be saved
* @return The keys as a single string
* @note The returned string must be freed 
*/
char* keys_to_string(const char** keys, int num_keys)
{
    size_t single_key_len = strlen(keys[0]);
    // Add enough extra characters for new lines
    size_t keys_string_len = (single_key_len * num_keys) + num_keys;
    char* keys_string = malloc(keys_string_len + 1);
    if(keys_string == NULL)
        return NULL;

    for(int k = 0; k < num_keys; k++)
        sprintf(&keys_string[k * (single_key_len + 1)], "%s%s", 
            keys[k], (k+1 != num_keys) ? "\n" : "");
    
    keys_string[keys_string_len] = '\0';
    return keys_string;
}

/**
* @brief Create new set of keys to use for encryption/decryption
* @param[in] hash_type The kind of SHA has to use
* @param[in] num_keys The number of keys to generate
* @param[in] filename Name fo the new keys file to create
* @return If the keys were generated and saved successfully
* @see HASH_TYPE enum for more info on the hash_type parameter
*/
int generate_new_keys(HASH_TYPE hash_type, int num_keys, const char* filename)
{
    int success = 1;
    char** keys = generate_keys(hash_type, num_keys);
    if(keys == NULL)
    {
        fprintf(stderr, "Error generating keys\n");
        return 0;
    }

    // TODO: look into encrypting this via AES
    char* keys_string = keys_to_string((const char**)keys, num_keys);
    printf("Individual Keys: \n");
    for(int k = 0; k < num_keys; k++)
    {
        printf("%d: %s\n",(k+1), keys[k]);
        free(keys[k]);
    }
    printf("\nKeys as one string: %s\n", keys_string);

    if(filename == NULL)
        filename = DEFAULT_KEYS_FILE;

    if(!save_to_file(filename, (unsigned char*)keys_string, strlen(keys_string)))
    {
        fprintf(stderr, "Error saving keys\n");
        success = 0;
    }

    free(keys);
    free(keys_string);
    return success;
}

/**
* @brief Prompt for which HASH_TYPE should be used for key generation
* @return Type of HASH_TYPE to use as an int, -1 for the user exiting
* @see HASH_TYPE enum for more info
*/
int get_hash_type_for_key_gen(void)
{
    while(1)
    {
        print_key_gen_menu();
        char* line = NULL;
        size_t line_len = 0;
        line_len = getline(&line, &line_len, stdin);
        // Replace new line with null terminator
        line[line_len-1] = '\0';
        if(strcmp(line, "q") == 0 || strcmp(line, "Q") == 0)
            return -1;

        int selection = strtol(line, NULL, 10);
        free(line);
        if(selection <= 0 || selection > num_key_gen_menu_items)
        {
            printf("Invalid selection\n");
            continue;
        }
        return selection - 1;
    }
}

/**
* @brief Prompt for how many keys should be generated
* @return The number of keys to generate, -1 for the user exiting
*/
int prompt_for_num_keys(void)
{
    while(1)
    {
        printf("Enter the number of keys to generate or 'q' to quit: ");
        char* line = NULL;
        size_t line_len = 0;
        line_len = getline(&line, &line_len, stdin);
        // Replace new line with null terminator
        line[line_len-1] = '\0';
        if(strcmp(line, "q") == 0 || strcmp(line, "Q") == 0)
            return -1;

        int num_keys = strtol(line, NULL, 10);
        int is_negative = (line[0] == '-');
        // Make sure number of keys is greater than 1 and 
        // protect against integer underflows and overflows
        if(num_keys <= 0 || is_negative)
        {
            printf("Invalid number of keys, should be at least 1.\n");
            free(line);
            continue;
        }
        free(line);
        return num_keys;
    }
}

/**
* @brief Creates a new keys file
* @return If new file was created
*/
int generate_new_keys_file(void)
{
    int hash_type = get_hash_type_for_key_gen();
    if(hash_type == -1)
        return 0; 
    
    int num_keys = prompt_for_num_keys();
    if(num_keys == -1)
        return 0;

    return generate_new_keys(hash_type, num_keys, NULL);
}

/**
* @brief Remove the users keys file
* @param[in] filename Name of the keys file to delete
*/
void delete_keys_file(const char* filename)
{
    int was_deleted = -1;
    printf("Are you sure you want to delete \'%s\'? (y/n): ", filename);
    char* line = NULL;
    size_t line_len = 0;
    line_len = getline(&line, &line_len, stdin);
    // Replace new line with null terminator
    line[line_len-1] = '\0';

    if(strcmp(line, "y") == 0 || strcmp(line, "Y") == 0)
        was_deleted = (remove(filename) == 0);
    
    free(line);

    if(was_deleted == 1)
        printf("Keys file was deleted successfully\n");
    else if(was_deleted == 0)
        fprintf(stderr, "Unable to delete the keys file\n");
    else
        printf("Deletion aborted.\n");
}

/**
* @brief Handle deleting keys files
*/
void delete_keys_file_manager(void)
{
    if(!keys_file_exists())
    {
        printf("You don't have any keys files\n");
        return;
    }
    do
    {
        size_t num_key_files = 0;
        char** keys_files_list = get_all_keys_files(&num_key_files);
        if(keys_files_list == NULL)
            return;

        printf("Select which keys file would you like to delete:\n");
        for(size_t f = 0; f < num_key_files; f++)
            printf("%zu: %s\n", (f + 1), keys_files_list[f]);
        if(num_key_files > 1)
            printf("(1-%zu) or 'q' to quit: ", num_key_files);
        else 
            printf("(1) or 'q' to quit: ");

        char* line = NULL;
        size_t line_len = 0;
        line_len = getline(&line, &line_len, stdin);
        // Replace new line with null terminator
        line[line_len-1] = '\0';
        
        if(strcmp(line, "q") == 0 || strcmp(line, "Q") == 0)
        {
            free(line);
            for(size_t f = 0; f < num_key_files; f++)
                free(keys_files_list[f]);
            free(keys_files_list);
            break;
        }

        int selection = strtol(line, NULL, 10);
        free(line);
        if(selection <= 0 || selection > num_key_files)
        {
            printf("Invalid selection.\n");
            continue;
        }
        delete_keys_file(keys_files_list[selection - 1]);
        
        for(size_t f = 0; f < num_key_files; f++)
            free(keys_files_list[f]);
        free(keys_files_list);
    } while(keys_file_exists());
}

void manage_keys(void)
{
    if(!keys_file_exists())
    {
        printf("No keys file exists. Would you like to create one? (y/n): ");
        char* line = NULL;
        size_t line_len = 0;
        line_len = getline(&line, &line_len, stdin);
        // Replace new line with null terminator
        line[line_len-1] = '\0';

        if(strcmp(line, "y") == 0 || strcmp(line, "Y") == 0)
            generate_new_keys_file();
        free(line);
    }
    while(1)
    {
        print_manage_keys_menu();
        char* line = NULL;
        size_t line_len = 0;
        line_len = getline(&line, &line_len, stdin);
        // Replace new line with null terminator
        line[line_len-1] = '\0';
        
        if(strcmp(line, "q") == 0 || strcmp(line, "Q") == 0)
        {
            free(line);
            return;
        }

        int selection = strtol(line, NULL, 10);
        free(line);
        if(selection <= 0 || selection > num_manage_keys_menu_items)
        {
            printf("Invalid selection.\n");
            continue;
        }
        MANAGE_KEYS_MENU_OPTIONS choice = selection - 1;
        switch(choice)
        {
            case MANAGE_KEYS_MENU_ADD:
                generate_new_keys_file();
                break;
            case MANAGE_KEYS_MENU_DELETE:
                delete_keys_file_manager();
                break;
            default:
                printf("Invalid selection\n");
                break;
        }
    }
}