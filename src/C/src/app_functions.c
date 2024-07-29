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

/**
 * @brief Create new set of keys to use for encryption/decryption
 * @param[in] hash_type The kind of SHA has to use
 * @param[in] num_keys The number of keys to generate
 * @param[in] filename Name fo the new keys file to create
 * @return If the keys were generated and saved successfully
 * @see HASH_TYPE enum for more info on the hash_type parameter
 */
static int generate_new_keys(HASH_TYPE hash_type, int num_keys,
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
        for (int k = 0; k < num_keys; k++)
            free(keys[k]);
        free(keys);
        return 0;
    }

    for (int k = 0; k < num_keys; k++)
    {
        printf("%d: %s\n", (k + 1), keys[k]);
        free(keys[k]);
    }
    free(keys);

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
 * @brief Prompt for which HASH_TYPE should be used for key generation
 * @return Type of HASH_TYPE to use as an int, -1 for the user exiting
 * @see HASH_TYPE enum for more info
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
        if (selection <= 0 || selection > num_key_gen_menu_items)
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
    if (hash_type == -1)
        return 0;

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

    int rc = generate_new_keys(hash_type, num_keys, filename);
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
        {
            printf("Invalid selection.\n");
            continue;
        }
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
 *   not currently exist
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
 * @brief Encrypt a single file based on user input
 * @param[in] ghost_mode Whether we are encrypting in ghost mode
 * @note Ghost Mode opts to use new randomly generated keys, rather
 *       than keys from a keys file
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
    char **keys = NULL;

    if (ghost_mode)
    {
        int hash_type = get_hash_type_for_key_gen();
        if (hash_type == -1)
        {
            free(filename);
            return;
        }

        num_keys = prompt_for_num_keys();
        if (num_keys == -1)
        {
            free(filename);
            return;
        }

        keys = generate_keys(hash_type, num_keys);
    }
    else if (!ghost_mode && handle_no_keys())
        keys = load_keys(&num_keys);
    else
    {
        fprintf(stderr,
                "%sError:%s You cannot encrypt files without having "
                "a keys file\nIf you wish to encrypt files without having a "
                "keys file select\nthe ghost mode option\n",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
        free(filename);
        return;
    }

    if (keys == NULL)
    {
        fprintf(stderr, "%sEncryption failed:%s  Keys were NULL.\n",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
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
    for (int k = 0; k < num_keys; k++)
    {
        if (ghost_mode)
            printf("%d: %s\n", (k + 1), keys[k]);
        free(keys[k]);
    }
    free(keys);
    return;
}

/**
 * @brief Encrypt an entire directory and its subdirectories
 *   based on user input
 * @param[in] ghost_mode Whether we are encrypting in ghost mode
 * @note Ghost Mode opts to use new randomly generated keys, rather
 *       than keys from a keys file
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
    char **keys = NULL;

    if (ghost_mode)
    {
        int hash_type = get_hash_type_for_key_gen();
        if (hash_type == -1)
        {
            free(dir_name);
            free(contents);
            return;
        }

        num_keys = prompt_for_num_keys();
        if (num_keys == -1)
        {
            free(dir_name);
            free(contents);
            return;
        }

        keys = generate_keys(hash_type, num_keys);
    }
    else if (!ghost_mode && handle_no_keys())
        keys = load_keys(&num_keys);
    else
    {
        fprintf(stderr,
                "%sError:%s You cannot encrypt files without having "
                "a keys file\nIf you wish to encrypt files without having a "
                "keys file select\nthe ghost mode option\n",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
        free(dir_name);
        free(contents);
        return;
    }

    if (keys == NULL)
    {
        fprintf(stderr, "%sEncryption failed:%s  Keys were NULL.\n",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
        free(dir_name);
        free(contents);
        return;
    }

    int arr_size = 0;
    char **files_list = split_string(contents, "\n", &arr_size);
    free(contents);
    if (files_list == NULL)
    {
        for (int k = 0; k < num_keys; k++)
            free(keys[k]);
        free(keys);
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

    for (int k = 0; k < num_keys; k++)
    {
        if (ghost_mode)
            printf("%d: %s\n", (k + 1), keys[k]);
        free(keys[k]);
    }
    free(keys);
    return;
}

/**
 * @brief Encrypt a single file based on user input
 * @param[in] ghost_mode Whether we are encrypting in ghost mode
 * @note Ghost Mode opts to use new randomly generated keys, rather
 *       than keys from a keys file
 */
static void decrypt_single_file_mode(int ghost_mode)
{
    char *filename = get_input_filename(0); // We are decrypting
    if (filename == NULL)
        return;

    int overwrite = prompt_for_overwrite(1); // single file

    int num_keys = 0;
    char **keys = NULL;
    int has_keys_file = handle_no_keys();

    if (!ghost_mode && has_keys_file)
        keys = load_keys(&num_keys);
    else if (!ghost_mode && !has_keys_file)
    {
        fprintf(stderr,
                "%sError:%s You cannot decrypt files without having "
                "a keys file\nIf you wish to decrypt files without having a "
                "keys file select\nthe ghost mode option",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
        free(filename);
        return;
    }
    else if (!prompt_for_ghost_mode_keys(&keys, &num_keys))
    {
        fprintf(stderr, "%sError:%s Invalid key provided\n",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
        free(filename);
        return;
    }

    if (keys == NULL)
    {
        free(filename);
        fprintf(stderr, "No keys were provided. Aborting...\n");
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
    for (int k = 0; k < num_keys; k++)
        free(keys[k]);
    free(keys);
    return;
}

/**
 * @brief Decrypt an entire directory and its subdirectories
 *   based on user input
 * @param[in] ghost_mode Whether we are decrypting in ghost mode
 * @note Ghost Mode opts to use new randomly generated keys, rather
 *       than keys from a keys file
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
    char **keys = NULL;
    int has_keys_file = handle_no_keys();

    if (!ghost_mode && has_keys_file)
        keys = load_keys(&num_keys);
    else if (!ghost_mode && !has_keys_file)
    {
        fprintf(stderr,
                "%sError:%s You cannot decrypt files without having "
                "a keys file\nIf you wish to decrypt files without having a "
                "keys file select\nthe ghost mode option",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
        free(dir_name);
        free(contents);
        return;
    }
    else if (!prompt_for_ghost_mode_keys(&keys, &num_keys))
    {
        fprintf(stderr, "%sError:%s Invalid key provided\n",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
        free(dir_name);
        free(contents);
        return;
    }

    if (keys == NULL)
    {
        free(dir_name);
        free(contents);
        fprintf(stderr, "No keys were provided. Aborting...\n");
        return;
    }

    int arr_size = 0;
    char **files_list = split_string(contents, "\n", &arr_size);
    free(contents);
    if (files_list == NULL)
    {
        for (int k = 0; k < num_keys; k++)
            free(keys[k]);
        free(keys);
        free(dir_name);
        fprintf(stderr,
                "%sError:%s Getting the list of individual files failed.\n",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
        return;
    }
    start_dir_decrypt_threads(files_list, arr_size, (const char **) keys,
                              num_keys, overwrite, threads);

    free(dir_name);
    for (int k = 0; k < num_keys; k++)
    {
        free(keys[k]);
    }
    free(keys);
    return;
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
        if (selection <= 0 || selection > num_manage_keys_menu_items)
        {
            printf("Invalid selection.\n");
            continue;
        }
        MANAGE_KEYS_MENU_OPTIONS choice = selection - 1;
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
        if (selection <= 0 || selection > num_encrypt_decrypt_menu_items)
        {
            printf("Invalid selection.\n");
            continue;
        }
        ENCRYPT_DECRYPT_MENU_OPTIONS choice = selection - 1;

        switch (choice)
        {
            case ENCRYPT_DECRYPT_MENU_FILE:
                encrypt_single_file_mode(ghost);
                return;
            case ENCRYPT_DECRYPT_MENU_DIR:
                encrypt_directory_mode(ghost);
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
        if (selection <= 0 || selection > num_encrypt_decrypt_menu_items)
        {
            printf("Invalid selection.\n");
            continue;
        }
        ENCRYPT_DECRYPT_MENU_OPTIONS choice = selection - 1;

        switch (choice)
        {
            case ENCRYPT_DECRYPT_MENU_FILE:
                decrypt_single_file_mode(ghost);
                return;
            case ENCRYPT_DECRYPT_MENU_DIR:
                decrypt_directory_mode(ghost);
                return;
            default:
                printf("Invalid selection\n");
                break;
        }
    }
}
