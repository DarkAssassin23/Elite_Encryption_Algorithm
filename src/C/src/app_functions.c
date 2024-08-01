#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app_functions.h"
#include "decrypt.h"
#include "encrypt.h"
#include "file_handling.h"
#include "globals.h"
#include "keygen.h"
#include "menu.h"
#include "prompts.h"
#include "thread_functions.h"
#include "utils.h"

static const char
    *GHOST_ENCRYPT_PRINT = "The data was encrypted with the following keys:";

/**
 * @brief Create new set of keys to use for encryption/decryption
 * @param[in] hash_type The kind of SHA has to use
 * @param[in] num_keys The number of keys to generate
 * @param[in] filename Name fo the new keys file to create
 * @return If the keys were generated and saved successfully
 * @see HashType enum for more info on the hash_type parameter
 */
static int generate_new_keys(HashType hash_type, int num_keys,
                             const char *filename)
{
    int success = 1;
    char **keys = generate_keys(hash_type, num_keys);
    if (keys == NULL)
    {
        fprintf(stderr, "%sError:%s Generating keys failed\n",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
        return 0;
    }

    unsigned char *keys_encrypted = NULL;
    size_t encrypted_keys_string_len = encrypt_keys(keys, num_keys,
                                                    &keys_encrypted);
    if (keys_encrypted == NULL)
    {
        fprintf(stderr, "%sError:%s Encrypting keys failed\n",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
        free_keys(keys, num_keys, NULL);
        return 0;
    }

    free_keys(keys, num_keys, "");

    if (filename == NULL)
        filename = DEFAULT_KEYS_FILE;
    char *full_path = get_keys_path(filename);
    if (!save_to_file(full_path, keys_encrypted, encrypted_keys_string_len))
    {
        fprintf(stderr, "%sError:%s Saving keys failed\n", colors[COLOR_ERROR],
                colors[COLOR_RESET]);
        success = 0;
    }
    if (full_path != NULL)
        free(full_path);

    free(keys_encrypted);
    return success;
}

/**
 * @brief Convert the given HashType to a size in bits
 * @param[in] ht The hash type
 * @return The size of the corresponding hash type in bits
 */
static int hash_type_to_size(HashType ht)
{
    if (ht < HASH_TYPE_OTHER)
        return KEY_BIT_SIZES[(int) ht];
    return prompt_key_size();
}

/**
 * @brief Prompt for which HashType should be used for key generation
 * @return Type of HashType to use as an int, -1 for the user exiting
 * @see HashType enum for more info
 */
static int get_hash_type_for_key_gen(void)
{
    while (1)
    {
        print_key_gen_menu();
        char *line = NULL;
        size_t line_len = 0;
        line_len = getline(&line, &line_len, stdin);
        // Replace new line with null terminator
        line[line_len - 1] = '\0';
        if (strcmp(line, "q") == 0 || strcmp(line, "Q") == 0)
        {
            free(line);
            return -1;
        }

        int selection = 0;
        if (strcmp(line, "") == 0)
        {
            printf("Using default selection of %d\n", DEFAULT_KEY_SELECTION);
            selection = DEFAULT_KEY_SELECTION;
        }
        else
            selection = strtol(line, NULL, 10);

        free(line);
        if (selection <= 0 || selection > NUM_KEY_GEN_MENU_ITEMS)
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
    int hash_type = 0, hash_size = -1;
    do
    {
        hash_type = get_hash_type_for_key_gen();
        if (hash_type == -1)
            return 0;
        hash_size = hash_type_to_size(hash_type);
    } while (hash_size == -1);

    int num_keys = prompt_for_num_keys();
    if (num_keys == -1)
        return 0;

    char *filename = NULL;
    if (prompt_for_keys_filename(&filename) < 0)
    {
        if (filename != NULL)
            free(filename);
        return 0;
    }

    int rc = generate_new_keys(hash_size, num_keys, filename);
    free(filename);
    return rc;
}

/**
 * @brief Remove the users keys file
 * @param[in] filename Name of the keys file to delete
 */
static void delete_keys_file(const char *filename)
{
    int was_deleted = -1;
    printf("Are you sure you want to delete \'%s\'? (y/n) "
           "(default: n): ",
           filename);
    char *line = NULL;
    size_t line_len = 0;
    line_len = getline(&line, &line_len, stdin);
    // Replace new line with null terminator
    line[line_len - 1] = '\0';

    if (strcmp(line, "y") == 0 || strcmp(line, "Y") == 0)
    {
        char *full_path = get_keys_path(filename);
        if (full_path == NULL)
            was_deleted = 0;
        else
        {
            was_deleted = (remove(full_path) == 0);
            free(full_path);
        }
    }

    free(line);

    if (was_deleted == 1)
        printf("Keys file was deleted successfully\n");
    else if (was_deleted == 0)
        fprintf(stderr, "Unable to delete the keys file\n");
    else
        printf("Deletion aborted.\n");
}

/**
 * @brief Handle deleting keys files
 */
static void delete_keys_file_manager(void)
{
    if (!keys_file_exists())
    {
        printf("You don't have any keys files\n");
        return;
    }
    do
    {
        size_t num_key_files = 0;
        char **keys_files_list = get_all_keys_files(&num_key_files);
        if (keys_files_list == NULL)
            return;

        printf("Select which keys file would you like to delete:\n");
        for (size_t f = 0; f < num_key_files; f++)
            printf("%zu: %s\n", (f + 1), keys_files_list[f]);
        if (num_key_files > 1)
            printf("(1-%zu) or 'q' to quit: ", num_key_files);
        else
            printf("(1) or 'q' to quit: ");

        char *line = NULL;
        size_t line_len = 0;
        line_len = getline(&line, &line_len, stdin);
        // Replace new line with null terminator
        line[line_len - 1] = '\0';

        if (strcmp(line, "q") == 0 || strcmp(line, "Q") == 0)
        {
            free(line);
            for (size_t f = 0; f < num_key_files; f++)
                free(keys_files_list[f]);
            free(keys_files_list);
            break;
        }

        int selection = strtol(line, NULL, 10);
        free(line);
        if (selection <= 0 || selection > num_key_files)
            printf("Invalid selection.\n");
        else
            delete_keys_file(keys_files_list[selection - 1]);

        for (size_t f = 0; f < num_key_files; f++)
            free(keys_files_list[f]);
        free(keys_files_list);
    } while (keys_file_exists());
}

/**
 * @brief View keys from a keys file
 */
static void view_keys(void)
{
    if (!keys_file_exists())
    {
        printf("You don't have any keys files\n");
        return;
    }
    do
    {
        size_t num_key_files = 0;
        char **keys_files_list = get_all_keys_files(&num_key_files);
        if (keys_files_list == NULL)
            return;

        printf("Select which keys file would you like to view:\n");
        for (size_t f = 0; f < num_key_files; f++)
            printf("%zu: %s\n", (f + 1), keys_files_list[f]);
        if (num_key_files > 1)
            printf("(1-%zu) or 'q' to quit", num_key_files);
        else
            printf("(1) or 'q' to quit");
        printf(" (default: 1): ");

        char *line = NULL;
        size_t line_len = 0;
        line_len = getline(&line, &line_len, stdin);
        // Replace new line with null terminator
        line[line_len - 1] = '\0';

        if (strcmp(line, "q") == 0 || strcmp(line, "Q") == 0)
        {
            free(line);
            for (size_t f = 0; f < num_key_files; f++)
                free(keys_files_list[f]);
            free(keys_files_list);
            break;
        }

        int selection = 0;
        if (strcmp(line, "") == 0)
            selection = 1;
        else
            selection = strtol(line, NULL, 10);

        free(line);
        if (selection <= 0 || selection > num_key_files)
        {
            printf("Invalid selection.\n");
            continue;
        }

        char **keys_decrypted = NULL;
        int num_keys = 0;
        size_t key_len = 0;
        char *key_file = get_keys_path(keys_files_list[selection - 1]);
        keys_decrypted = load_keys_from_file(key_file, &num_keys, &key_len);
        if (key_file != NULL)
            free(key_file);

        if (keys_decrypted == NULL)
        {
            for (size_t f = 0; f < num_key_files; f++)
                free(keys_files_list[f]);
            free(keys_files_list);
            printf("No keys loaded. Aborting...\n");
            return;
        }

        printf("Keys in %s\n", keys_files_list[selection - 1]);
        for (int k = 0; k < num_keys; k++)
        {
            printf("%d: %s\n", (k + 1), keys_decrypted[k]);
            free(keys_decrypted[k]);
        }
        free(keys_decrypted);

        for (size_t f = 0; f < num_key_files; f++)
            free(keys_files_list[f]);
        free(keys_files_list);

        return;

    } while (keys_file_exists());
}

/**
 * @brief Function to handle checking and generating a keys file if one does
 * not currently exist
 * @return If keys file is present
 */
static int handle_no_keys(void)
{
    int success = 1;
    if (!keys_file_exists())
    {
        success = 0;
        printf("No keys file exists.\n");
        printf("Would you like to create one? (y/n) (default: y): ");
        char *line = NULL;
        size_t line_len = 0;
        line_len = getline(&line, &line_len, stdin);
        // Replace new line with null terminator
        line[line_len - 1] = '\0';

        if (strcmp(line, "y") == 0 || strcmp(line, "Y") == 0
            || strcmp(line, "") == 0)
            success = generate_new_keys_file();
        free(line);
    }
    return success;
}

/**
 * @brief Prompt the user for what keys they would like to use
 * @param[in] ghost Whether we are using ghost mode
 * @param[in] encrypt Whether or not we are encrypting
 * @param[out] num The number of keys in the returned list
 * @return List of keys
 */
static char **keys_prompt(int ghost, int encrypt, int *num)
{
    const char *type = encrypt ? "encrypt" : "decrypt";
    const char *type2 = encrypt ? "Encryption" : "Decryption";
    char **keys = NULL;
    *num = 0;

    if (ghost && encrypt)
    {
        int hash_type = get_hash_type_for_key_gen();
        if (hash_type = -1)
            return NULL;
        int hash_size = hash_type_to_size(hash_type);
        if (hash_size == -1)
            return NULL;

        *num = prompt_for_num_keys();
        if (*num == -1)
            return NULL;

        keys = generate_keys(hash_size, *num);
    }
    else if (ghost && !encrypt)
    {
        if (!prompt_for_ghost_mode_keys(&keys, num))
        {
            fprintf(stderr, "%sError:%s Invalid key provided\n",
                    colors[COLOR_ERROR], colors[COLOR_RESET]);
            return NULL;
        }
    }
    else
    {
        if (handle_no_keys())
            keys = load_keys(num);
        else
            goto keys_prompt_need_file;
    }

    if (keys == NULL)
    {
        fprintf(stderr, "%s%s failed:%s  Keys were NULL.\n",
                colors[COLOR_ERROR], type2, colors[COLOR_RESET]);
        return NULL;
    }
    return keys;

keys_prompt_need_file:
    fprintf(stderr,
            "%sError:%s You cannot %s data without having "
            "a keys file\nIf you wish to %s data without having a "
            "keys file, enable\nghost mode.\n",
            colors[COLOR_ERROR], colors[COLOR_RESET], type, type);
    return NULL;
}

/**
 * @brief Encrypt a single file based on user input
 * @param[in] ghost_mode Whether we are encrypting in ghost mode
 * @note Ghost Mode opts to use new randomly generated keys, rather
 * than keys from a keys file
 */
static void encrypt_single_file_mode(int ghost_mode)
{
    char *filename = get_input_filename(1); // We are encrypting
    if (filename == NULL)
        return;

    if (get_file_type(filename) != FILE_TYPE_REG)
    {
        fprintf(stderr, "%sError:%s %s is not a regular file\n",
                colors[COLOR_ERROR], colors[COLOR_RESET], filename);
        free(filename);
        return;
    }

    int overwrite = prompt_for_overwrite(1); // single file

    int num_keys = 0;
    char **keys = keys_prompt(ghost_mode, 1, &num_keys);
    if (keys == NULL)
    {
        free(filename);
        return;
    }

    int encryption_success = encrypt_file(filename, (const char **) keys,
                                          num_keys);
    if (encryption_success)
        fprintf(stdout, "%sEncryption success:%s %s\n%s",
                colors[COLOR_SUCCESS], colors[COLOR_RESET], filename,
                (ghost_mode ? "Encrypted with the following keys:\n" : ""));
    else
        fprintf(stderr, "%sEncryption failed:%s  %s\n", colors[COLOR_ERROR],
                colors[COLOR_RESET], filename);

    if (encryption_success && overwrite)
        remove(filename);

    free(filename);
    const char *print = ghost_mode ? GHOST_ENCRYPT_PRINT : NULL;
    free_keys(keys, num_keys, print);
    return;
}

/**
 * @brief Encrypt an entire directory and its subdirectories
 * based on user input
 * @param[in] ghost_mode Whether we are encrypting in ghost mode
 * @note Ghost Mode opts to use new randomly generated keys, rather
 * than keys from a keys file
 */
static void encrypt_directory_mode(int ghost_mode)
{
    char *dir_name = get_input_dir_name(1); // We are encrypting
    if (dir_name == NULL)
        return;

    char *contents = get_dir_contents(dir_name);
    if (strcmp(contents, "") == 0)
    {
        printf("The directory \'%s\' is empty. There is nothing to do\n",
               dir_name);
        free(contents);
        free(dir_name);
        return;
    }
    // Replace the last newline with a null terminator
    contents[strlen(contents) - 1] = '\0';

    int overwrite = prompt_for_overwrite(0); // not single file
    int threads = prompt_for_num_threads();

    int num_keys = 0;
    char **keys = keys_prompt(ghost_mode, 1, &num_keys);
    if (keys == NULL)
    {
        free(contents);
        free(dir_name);
        return;
    }

    int arr_size = 0;
    char **files_list = split_string(contents, "\n", &arr_size);
    free(contents);
    if (files_list == NULL)
    {
        free_keys(keys, num_keys, NULL);
        free(dir_name);
        fprintf(stderr,
                "%sError:%s Getting the list of individual files failed.\n",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
        return;
    }
    start_dir_encrypt_threads(files_list, arr_size, (const char **) keys,
                              num_keys, overwrite, threads);

    free(dir_name);
    if (ghost_mode)
        printf("Your files were encrypted with the following keys:\n");

    const char *print = ghost_mode ? GHOST_ENCRYPT_PRINT : NULL;
    free_keys(keys, num_keys, print);
    return;
}

/**
 * @brief Encrypt a single file based on user input
 * @param[in] ghost_mode Whether we are encrypting in ghost mode
 * @note Ghost Mode opts to use new randomly generated keys, rather
 * than keys from a keys file
 */
static void decrypt_single_file_mode(int ghost_mode)
{
    char *filename = get_input_filename(0); // We are decrypting
    if (filename == NULL)
        return;

    int overwrite = prompt_for_overwrite(1); // single file

    int num_keys = 0;
    char **keys = keys_prompt(ghost_mode, 0, &num_keys);
    if (keys == NULL)
    {
        free(filename);
        return;
    }

    int decryption_success = decrypt_file(filename, (const char **) keys,
                                          num_keys);
    if (decryption_success)
        fprintf(stdout, "%sDecryption success:%s %s\n", colors[COLOR_SUCCESS],
                colors[COLOR_RESET], filename);
    else
        fprintf(stderr, "%sDecryption failed:%s  %s\n", colors[COLOR_ERROR],
                colors[COLOR_RESET], filename);

    if (decryption_success && overwrite)
        remove(filename);

    free(filename);
    free_keys(keys, num_keys, NULL);
    return;
}

/**
 * @brief Decrypt an entire directory and its subdirectories
 * based on user input
 * @param[in] ghost_mode Whether we are decrypting in ghost mode
 * @note Ghost Mode opts to use new randomly generated keys, rather
 * than keys from a keys file
 */
static void decrypt_directory_mode(int ghost_mode)
{
    char *dir_name = get_input_dir_name(0); // We are Decrypting
    if (dir_name == NULL)
        return;

    char *contents = get_dir_contents(dir_name);
    if (strcmp(contents, "") == 0)
    {
        printf("The directory \'%s\' is empty. There is nothing to do\n",
               dir_name);
        free(contents);
        free(dir_name);
        return;
    }
    // Replace the last newline with a null terminator
    contents[strlen(contents) - 1] = '\0';

    int overwrite = prompt_for_overwrite(0); // not single file

    int threads = prompt_for_num_threads();

    int num_keys = 0;
    char **keys = keys_prompt(ghost_mode, 1, &num_keys);
    if (keys == NULL)
    {
        free(dir_name);
        free(contents);
        return;
    }

    int arr_size = 0;
    char **files_list = split_string(contents, "\n", &arr_size);
    free(contents);
    if (files_list == NULL)
    {
        free_keys(keys, num_keys, NULL);
        free(dir_name);
        fprintf(stderr,
                "%sError:%s Getting the list of individual files failed.\n",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
        return;
    }
    start_dir_decrypt_threads(files_list, arr_size, (const char **) keys,
                              num_keys, overwrite, threads);

    free(dir_name);
    free_keys(keys, num_keys, NULL);
    return;
}

/**
 * @brief Encrypt and decrypt plain text
 * @param[in] ghost_mode Encrypting/decrypting with ghost mode
 * @param[in] encrypting Are we encrypting
 */
static void text_mode(int ghost_mode, int encrypting)
{
    const char *type = encrypting ? "encrypt" : "decrypt";
    const char *type2 = encrypting ? "Encryption" : "Decryption";
    const char done_str[] = "</done>";
    int reading = 1;
    size_t max_size = 64, size = 0;

    char *text = calloc(max_size, sizeof(char));
    if (text == NULL)
    {
        fprintf(stderr, "%sError:%s Failed to allocate memory. Aborting...\n",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
        return;
    }

    printf("When you finish entering your text, type: %s\n", done_str);
    printf("Enter the text you would like to %s:\n", type);
    while (reading)
    {
        char *line = NULL;
        size_t line_len = 0;
        line_len = getline(&line, &line_len, stdin);
        // Replace new line with null terminator
        line[line_len - 1] = '\0';
        if (ends_with(line, done_str))
        {
            line_len -= sizeof(done_str);
            line[line_len] = 0;
            reading = 0;
        }
        else
            line[line_len - 1] = '\n';

        if (buff_resize(&text, &max_size, (line_len + size) + 1) != 0)
        {
            fprintf(stderr,
                    "%sError:%s Failed to resize text buffer. Aborting...\n",
                    colors[COLOR_ERROR], colors[COLOR_RESET]);
            free(text);
            free(line);
            return;
        }
        strcat(text, line);
        size += line_len;

        free(line);
    }

    // Make sure text was entered
    if (strcmp(text, "") == 0)
    {
        printf("No text entered. Nothing to do.\n");
        free(text);
        return;
    }

    int num_keys = 0;
    char **keys = keys_prompt(ghost_mode, encrypting, &num_keys);
    if (keys == NULL)
    {
        free(text);
        return;
    }

    unsigned char *result = NULL;
    int ret = 0;
    if (encrypting)
        ret = encrypt((unsigned char *) text, size, &result,
                      (const char **) keys, num_keys);
    else
        ret = decrypt((unsigned char *) text, size, &result,
                      (const char **) keys, num_keys);

    const char *print = ghost_mode ? GHOST_ENCRYPT_PRINT : NULL;
    free_keys(keys, num_keys, print);

    if (ret)
        fprintf(stdout, "%s%s Success!%s\n%s\n", colors[COLOR_SUCCESS], type2,
                colors[COLOR_RESET], (char *) result);
    else
        fprintf(stderr, "%s%s Failed...%s\n", colors[COLOR_ERROR], type2,
                colors[COLOR_RESET]);

    free(text);
    free(result);
}

void manage_keys(void)
{
    handle_no_keys();

    while (1)
    {
        print_manage_keys_menu();
        char *line = NULL;
        size_t line_len = 0;
        line_len = getline(&line, &line_len, stdin);
        // Replace new line with null terminator
        line[line_len - 1] = '\0';

        if (strcmp(line, "q") == 0 || strcmp(line, "Q") == 0)
        {
            free(line);
            return;
        }

        int selection = strtol(line, NULL, 10);
        free(line);
        if (selection <= 0 || selection > NUM_MANAGE_KEYS_MENU_ITEMS)
        {
            printf("Invalid selection.\n");
            continue;
        }
        ManageKeysMenuOptions choice = selection - 1;
        switch (choice)
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
    int ghost = using_ghost_mode();
    if (ghost)
        ghost_mode_warning();

    while (1)
    {
        print_encrypt_decrypt_menu(1); // Encrypting
        char *line = NULL;
        size_t line_len = 0;
        line_len = getline(&line, &line_len, stdin);
        // Replace new line with null terminator
        line[line_len - 1] = '\0';

        if (strcmp(line, "q") == 0 || strcmp(line, "Q") == 0)
        {
            free(line);
            return;
        }

        int selection = 0;
        if (strcmp(line, "") == 0)
            selection = 1;
        else
            selection = strtol(line, NULL, 10);

        free(line);
        if (selection <= 0 || selection > NUM_ENCRYPT_DECRYPT_MENU_ITEMS)
        {
            printf("Invalid selection.\n");
            continue;
        }
        EncryptDecryptMenuOptions choice = selection - 1;

        switch (choice)
        {
            case ENCRYPT_DECRYPT_MENU_FILE:
                encrypt_single_file_mode(ghost);
                return;
            case ENCRYPT_DECRYPT_MENU_DIR:
                encrypt_directory_mode(ghost);
                return;
            case ENCRYPT_DECRYPT_MENU_TEXT:
                text_mode(ghost, 1);
                return;
            default:
                printf("Invalid selection\n");
                break;
        }
    }
}

void do_decryption(void)
{
    int ghost = using_ghost_mode();
    while (1)
    {
        print_encrypt_decrypt_menu(0); // Decrypting
        char *line = NULL;
        size_t line_len = 0;
        line_len = getline(&line, &line_len, stdin);
        // Replace new line with null terminator
        line[line_len - 1] = '\0';

        if (strcmp(line, "q") == 0 || strcmp(line, "Q") == 0)
        {
            free(line);
            return;
        }

        int selection = 0;
        if (strcmp(line, "") == 0)
            selection = 1;
        else
            selection = strtol(line, NULL, 10);

        free(line);
        if (selection <= 0 || selection > NUM_ENCRYPT_DECRYPT_MENU_ITEMS)
        {
            printf("Invalid selection.\n");
            continue;
        }
        EncryptDecryptMenuOptions choice = selection - 1;

        switch (choice)
        {
            case ENCRYPT_DECRYPT_MENU_FILE:
                decrypt_single_file_mode(ghost);
                return;
            case ENCRYPT_DECRYPT_MENU_DIR:
                decrypt_directory_mode(ghost);
                return;
            case ENCRYPT_DECRYPT_MENU_TEXT:
                text_mode(ghost, 0);
                return;
            default:
                printf("Invalid selection\n");
                break;
        }
    }
}
