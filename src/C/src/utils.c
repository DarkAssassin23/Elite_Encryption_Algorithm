#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/rand.h>

#include "file_handling.h"
#include "globals.h"
#include "utils.h"

static const int MAX_TRIES = 5;

void message_digest_to_hash(unsigned char *md, char *hash, int digest_length)
{
    for (int x = 0; x < digest_length; x++)
        sprintf(&hash[x * 2], "%02hhx", md[x]);
}

char *get_random_hexstr(size_t size)
{
    unsigned char buffer[size];
    int tries = 0;
    int rc = 1;
    do
    {
        rc = RAND_bytes(buffer, sizeof(buffer));
        if (rc != 1)
            tries++;

    } while (rc != 1 && tries < MAX_TRIES);

    if (rc != 1)
    {
        fprintf(stderr,
                "%sError:%s Exceeded the max tries to get random "
                "bytes\n",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
        exit(rc);
    }

    char *hexstr = malloc((sizeof(buffer) * 2) + 1);
    if (hexstr == NULL)
        return NULL;

    for (int x = 0; x < sizeof(buffer); x++)
        sprintf(&hexstr[x * 2], "%02hhx", buffer[x]);

    return hexstr;
}

char *keys_to_string(const char **keys, int num_keys)
{
    size_t single_key_len = strlen(keys[0]);
    // Add enough extra characters for new lines
    size_t keys_string_len = (single_key_len * num_keys) + num_keys;
    char *keys_string = malloc(keys_string_len + 1);
    if (keys_string == NULL)
        return NULL;

    for (int k = 0; k < num_keys; k++)
        sprintf(&keys_string[k * (single_key_len + 1)], "%s%s", keys[k],
                (k + 1 != num_keys) ? "\n" : "");

    keys_string[keys_string_len] = '\0';
    return keys_string;
}

char **split_string(char *string, const char *delim, int *size)
{
    char **result = NULL;
    int count = 0;
    char *pch;

    // split
    pch = strtok(string, delim);
    while (pch != NULL)
    {
        result = realloc(result, sizeof(char *) * (count + 1));
        result[count] = malloc(strlen(pch) + 1);
        strcpy(result[count], pch);
        count++;
        pch = strtok(NULL, delim);
    }
    *size = count;
    return result;
}

/**
 * @see https://stackoverflow.com/a/8393473
 */
int validate_keys(const char **keys, int num_keys)
{
    size_t key_len = strlen(keys[0]);
    if (key_len % MIN_KEY_LEN != 0)
        return 0;

    for (int k = 0; k < num_keys; k++)
    {
        int valid_hex = (keys[k][strspn(keys[k], "0123456789abcdefABCDEF")]
                         == 0);
        int valid_len = (strlen(keys[k]) == key_len);
        if (!valid_hex || !valid_len)
            return 0;
    }
    return 1;
}

size_t find_key_len(const char *keys_string)
{
    // Minimum key length
    size_t key_len = MIN_KEY_LEN;
    size_t keys_string_len = strlen(keys_string);

    while (key_len <= keys_string_len)
    {
        if (keys_string[key_len] == '\n' || keys_string[key_len] == '\0')
            return key_len;
        key_len += MIN_KEY_LEN;
    }

    // No keys were found
    return -1;
}

char **load_keys(int *num_keys)
{
    if (!keys_file_exists())
    {
        printf("You don't have any keys files\n");
        return NULL;
    }
    do
    {
        size_t num_key_files = 0;
        char **keys_files_list = get_all_keys_files(&num_key_files);
        if (keys_files_list == NULL)
            return NULL;

        printf("Select which keys file would you like to use:\n");
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
            return NULL;
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
        size_t key_len = 0;
        keys_decrypted = load_keys_from_file(keys_files_list[selection - 1],
                                             num_keys, &key_len);

        for (size_t f = 0; f < num_key_files; f++)
            free(keys_files_list[f]);
        free(keys_files_list);

        return keys_decrypted;

    } while (keys_file_exists());
    return NULL;
}
