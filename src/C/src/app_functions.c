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
int generate_new_keys(HASH_TYPE hash_type, int num_keys, const char* filename)
{
    int success = 1;
    char** keys = generate_keys(hash_type, num_keys);
    if(keys == NULL)
    {
        fprintf(stderr, "Error generating keys\n");
        return 0;
    }

    // Here for testing
    char* keys_string = keys_to_string((const char**)keys, num_keys);

    unsigned char* keys_encrypted = NULL;
    size_t encrypted_keys_string_len = encrypt_keys(keys, num_keys, &keys_encrypted);

    // Here just for testing to make sure encrypting and decrypting the passwords works
    // printf("Pre-encrypt:\n%s\n",keys_string);
    // for(size_t c = 0; c < encrypted_keys_string_len; c++)
    //     printf("%c"/*"%02hhx"*/, keys_encrypted[c]);
    // printf("\n");

    char* keys_decrypted = NULL;
    size_t keys_string_len = decrypt_keys(keys_encrypted, encrypted_keys_string_len, &keys_decrypted);

    printf("post-encrypt:\n%s\n", keys_decrypted);
    printf("encryption and decryption %s\n", (strcmp(keys_decrypted, keys_string) == 0 ? "succeeded" : "failed"));
    free(keys_decrypted);
    

    // printf("Individual Keys: \n");
    for(int k = 0; k < num_keys; k++)
    {
        //printf("%d: %s\n",(k+1), keys[k]);
        free(keys[k]);
    }
    //printf("\nKeys as one string: %s\n", keys_string);

    if(filename == NULL)
        filename = DEFAULT_KEYS_FILE;

    //if(!save_to_file(filename, (unsigned char*)keys_string, strlen(keys_string)))
    if(!save_to_file(filename, keys_encrypted, keys_string_len))
    {
        fprintf(stderr, "Error saving keys\n");
        success = 0;
    }

    free(keys);
    free(keys_string);
    free(keys_encrypted);
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
int generate_new_keys_file(void)
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
void delete_keys_file(const char* filename)
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
        printf("No keys file exists.\n");
        printf("Would you like to create one? (y/n) (default: y): ");
        char* line = NULL;
        size_t line_len = 0;
        line_len = getline(&line, &line_len, stdin);
        // Replace new line with null terminator
        line[line_len-1] = '\0';

        if(strcmp(line, "y") == 0 || strcmp(line, "Y") == 0 ||
            strcmp(line, "") == 0)
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