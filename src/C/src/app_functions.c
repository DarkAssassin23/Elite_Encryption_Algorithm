#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "utils.h"
#include "menu.h"
#include "keygen.h"
#include "encrypt.h"
#include "decrypt.h"
#include "prompts.h"
#include "file_handling.h"
#include "app_functions.h"

/**
* @brief Create new set of keys to use for encryption/decryption
* @param[in] hash_type The kind of SHA has to use
* @param[in] num_keys The number of keys to generate
* @param[in] filename Name fo the new keys file to create
* @return If the keys were generated and saved successfully
* @see HASH_TYPE enum for more info on the hash_type parameter
*/
static int generate_new_keys(HASH_TYPE hash_type, int num_keys, const char* filename)
{
    int success = 1;
    char** keys = generate_keys(hash_type, num_keys);
    if(keys == NULL)
    {
        fprintf(stderr, "Error generating keys\n");
        return 0;
    }

    unsigned char* keys_encrypted = NULL;
    size_t encrypted_keys_string_len = encrypt_keys(keys, num_keys, &keys_encrypted);
    if(keys_encrypted == NULL)
    {
        fprintf(stderr, "Error encrypting keys\n");
        for(int k = 0; k < num_keys; k++)
            free(keys[k]);
        free(keys);
        return 0;
    }

    for(int k = 0; k < num_keys; k++)
    {
        printf("%d: %s\n",(k+1), keys[k]);
        free(keys[k]);
    }
    free(keys);

    if(filename == NULL)
        filename = DEFAULT_KEYS_FILE;

    if(!save_to_file(filename, keys_encrypted, encrypted_keys_string_len))
    {
        fprintf(stderr, "Error saving keys\n");
        success = 0;
    }

    free(keys_encrypted);
    return success;
}

/**
* @brief Prompt for which HASH_TYPE should be used for key generation
* @return Type of HASH_TYPE to use as an int, -1 for the user exiting
* @see HASH_TYPE enum for more info
*/
static int get_hash_type_for_key_gen(void)
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
        {
            free(line);
            return -1;
        }

        int selection = 0; 
        if(strcmp(line, "") == 0)
        {
            printf("Using default selection of %d\n", DEFAULT_KEY_SELECTION);
            selection = DEFAULT_KEY_SELECTION;
        }
        else
            selection = strtol(line, NULL, 10);

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
* @brief Creates a new keys file
* @return If new file was created
*/
static int generate_new_keys_file(void)
{
    int hash_type = get_hash_type_for_key_gen();
    if(hash_type == -1)
        return 0; 
    
    int num_keys = prompt_for_num_keys();
    if(num_keys == -1)
        return 0;

    char* filename = NULL;
    if(prompt_for_keys_filename(&filename) < 0)
    {
        if(filename != NULL)
            free(filename);
        return 0;
    }

    int rc = generate_new_keys(hash_type, num_keys, filename);
    free(filename);
    return rc;
}

/**
* @brief Remove the users keys file
* @param[in] filename Name of the keys file to delete
*/
static void delete_keys_file(const char* filename)
{
    int was_deleted = -1;
    printf("Are you sure you want to delete \'%s\'? (y/n) "
        "(default: n): ", filename);
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
static void delete_keys_file_manager(void)
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

/**
* @brief View keys from a keys file
*/
static void view_keys(void)
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

        printf("Select which keys file would you like to view:\n");
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

        int selection = 0;
        if(strcmp(line, "") == 0)
            selection = 1;
        else
            selection = strtol(line, NULL, 10);

        free(line);
        if(selection <= 0 || selection > num_key_files)
        {
            printf("Invalid selection.\n");
            continue;
        }

        char** keys_decrypted = NULL;
        int num_keys = 0;
        size_t key_len = 0;
        keys_decrypted = load_keys_from_file(keys_files_list[selection - 1], 
                                                &num_keys, &key_len);
        printf("Keys in %s\n", keys_files_list[selection - 1]);
        for(int k = 0; k < num_keys; k++)
        {
            printf("%d: %s\n",(k+1), keys_decrypted[k]);
            free(keys_decrypted[k]);
        }
        free(keys_decrypted);
        
        for(size_t f = 0; f < num_key_files; f++)
            free(keys_files_list[f]);
        free(keys_files_list);

        return;

    } while(keys_file_exists());
}

/**
* @brief Function to handle checking and generating a keys file if one does
*   not currently exist
* @return If keys file is present
*/
static int handle_no_keys(void)
{
    int success = 1;
    if(!keys_file_exists())
    {
        success = 0;
        printf("No keys file exists.\n");
        printf("Would you like to create one? (y/n) (default: y): ");
        char* line = NULL;
        size_t line_len = 0;
        line_len = getline(&line, &line_len, stdin);
        // Replace new line with null terminator
        line[line_len-1] = '\0';

        if(strcmp(line, "y") == 0 || strcmp(line, "Y") == 0 ||
            strcmp(line, "") == 0)
            success = generate_new_keys_file();
        free(line);
    }
    return success;
}

void manage_keys(void)
{
    handle_no_keys();

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
            case MANAGE_KEYS_MENU_VIEW:
                view_keys();
                break;
            default:
                printf("Invalid selection\n");
                break;
        }
    }
}

void do_encryption(void)
{
    if(!handle_no_keys())
    {
        // Ghost mode?
        printf("You cannot encrypt files without having a keys file\n");
        return;
    }
    while(1)
    {
        // Menu for files vs directories and ghost mode
        char* filename = get_input_filename(1); // We are encrypting
        if(filename == NULL)
            return;

        int num_keys = 0;
        char** keys = load_keys(&num_keys);

        if(keys == NULL)
            return;

        if(encrypt_file(filename, (const char**)keys, num_keys))
            printf("%s was encrypted successfully\n", filename);
        else
            fprintf(stderr,"Error: Failed to encrypt %s\n", filename);
        
        free(filename);
        for(int k = 0; k < num_keys; k++)
            free(keys[k]);
        free(keys);
        return;
    }
}