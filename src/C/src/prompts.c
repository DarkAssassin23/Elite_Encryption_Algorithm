#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <openssl/sha.h>

#include "file_handling.h"
#include "globals.h"
#include "prompts.h"
#include "utils.h"

int prompt_for_keys_filename(char **filename)
{
    while (1)
    {
        printf("Enter the filename to save your keys to. ");
        printf("It should end in .keys\n");
        printf("Filename or 'q' to quit (default: %s): ", DEFAULT_KEYS_FILE);
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

        // Use the default
        if (strcmp(line, "") == 0)
        {
            free(line);
            line = strdup(DEFAULT_KEYS_FILE);
        }
        else if (!is_of_filetype(line, ".keys"))
        {
            printf("Invalid filename. It should end in .keys\n");
            free(line);
            continue;
        }

        char *path = get_keys_path(line);
        if (path == NULL)
            path = line;
        if (file_exists(path))
        {
            fprintf(stdout, "%sWARNING%s the file \'%s\' already exists.\n",
                    colors[COLOR_WARNING], colors[COLOR_RESET], line);
            printf(
                "Are you sure you want to override it? (y/n) (default: n): ");
            char *choice = NULL;
            size_t len = 0;
            len = getline(&choice, &len, stdin);
            // Replace new line with null terminator
            choice[len - 1] = '\0';
            if (strcmp(choice, "y") != 0 && strcmp(choice, "Y") != 0)
            {
                free(choice);
                free(line);
                continue;
            }
            free(choice);
        }
        if (path != line)
            free(path);

        *filename = line;
        return 1;
    }
}

int prompt_for_num_keys(void)
{
    while (1)
    {
        printf("Enter the number of keys to generate "
               "or 'q' to quit (default: %d): ",
               DEFAULT_NUM_KEYS);
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

        int num_keys = 0;
        int is_negative = 0;

        if (strcmp(line, "") == 0)
        {
            printf("Using default value of %d\n", DEFAULT_NUM_KEYS);
            num_keys = DEFAULT_NUM_KEYS;
        }
        else
        {
            num_keys = strtol(line, NULL, 10);
            is_negative = (line[0] == '-');
        }

        // Make sure number of keys is greater than 1 and
        // protect against integer underflows and overflows
        if (num_keys < 0 || is_negative)
        {
            printf("Invalid number of keys, should be at least 1.\n");
            free(line);
            continue;
        }
        free(line);
        return num_keys;
    }
}

int prompt_key_size(void)
{
    while (1)
    {
        printf("Enter the size each key should be, or 'q' to quit: ");
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

        int key_size = strtol(line, NULL, 10);
        int is_negative = line[0] == '-';

        // Make sure the key size is greater than 256-bits and
        // protect against integer underflows and overflows
        if (key_size < MIN_KEY_BITS || key_size % MIN_KEY_BITS != 0
            || is_negative)
        {
            printf("Invalid key size. The size must be a multiple of %d.\n",
                   MIN_KEY_BITS);
            free(line);
            continue;
        }
        free(line);
        return key_size;
    }
}

int prompt_for_num_threads(void)
{
    printf("Enter the number of threads to use (default: 1): ");
    char *line = NULL;
    size_t line_len = 0;
    line_len = getline(&line, &line_len, stdin);
    // Replace new line with null terminator
    line[line_len - 1] = '\0';

    int num_threads = 0;
    int is_negative = 0;

    if (strcmp(line, "") == 0)
        num_threads = 1;
    else
    {
        num_threads = strtol(line, NULL, 10);
        is_negative = (line[0] == '-');
    }

    // Make sure number of threads is greater than 1 and
    // protect against integer underflows and overflows
    if (num_threads <= 0 || is_negative)
        num_threads = 1;

    free(line);
    return num_threads;
}

int using_ghost_mode(void)
{
    int success = 0;
    printf("Would you like to enable Ghost Mode? (y/n) (default: n): ");
    char *line = NULL;
    size_t line_len = 0;
    line_len = getline(&line, &line_len, stdin);
    // Replace new line with null terminator
    line[line_len - 1] = '\0';

    if (strcmp(line, "y") == 0 || strcmp(line, "Y") == 0)
        success = 1;
    free(line);

    return success;
}

void ghost_mode_warning(void)
{
    printf(
        "%sWarning:%s You are about to encrypt the data "
        "with one time use keys.\nYou will be unable to decrypt the data "
        "unless you manually copy down\nthe keys after you encrypt your data",
        colors[COLOR_WARNING], colors[COLOR_RESET]);
}

int prompt_for_ghost_mode_keys(char ***keys, int *num_keys)
{
    int k = 0;
    int k_size = 1;
    size_t key_len = 0;
    char **temp_keys = malloc(sizeof(char *) * (k + k_size));
    if (temp_keys == NULL)
        return 0;

    printf("When you finish entering your keys, type \'done\' or hit enter"
           "\nEnter your keys below:\n");
    while (1)
    {
        printf("%d: ", (k + 1));
        char *line = NULL;
        size_t line_len = 0;
        line_len = getline(&line, &line_len, stdin);
        // Replace new line with null terminator
        line[line_len - 1] = '\0';
        if (strcmp(line, "") == 0 || strcmp(line, "done") == 0)
        {
            free(line);
            break;
        }

        if (k == k_size)
        {
            k_size += k;
            char **tmp = realloc(temp_keys, sizeof(char *) * k_size);
            if (tmp == NULL)
            {
                for (int i = 0; i < k; i++)
                    free(temp_keys[i]);
                free(temp_keys);
                free(line);
                return 0;
            }
            temp_keys = tmp;
        }

        if (strlen(line) % 64 != 0)
        {
            for (int i = 0; i < k; i++)
                free(temp_keys[i]);
            free(temp_keys);
            free(line);
            return 0;
        }

        if (k == 0)
            key_len = strlen(line);

        // Not using validate keys function since we don't need to
        // validate previous keys we've already validated
        int valid_hex = (line[strspn(line, "0123456789abcdefABCDEF")] == 0);
        int valid_len = (strlen(line) == key_len);
        if (!valid_hex || !valid_len)
        {
            for (int i = 0; i < k; i++)
                free(temp_keys[i]);
            free(temp_keys);
            free(line);
            return 0;
        }

        temp_keys[k] = strdup(line);
        free(line);
        k++;
    }
    if (k == 0)
    {
        free(temp_keys);
        temp_keys = NULL;
    }
    *keys = temp_keys;
    *num_keys = k;
    return 1;
}

int prompt_for_overwrite(int is_single_file)
{
    printf("Overwrite %s? (y/n) (default: y): ",
           (is_single_file) ? "the file" : "files in the directory");
    char *line = NULL;
    size_t line_len = 0;
    line_len = getline(&line, &line_len, stdin);
    // Replace new line with null terminator
    line[line_len - 1] = '\0';
    line_len--;

    int overwrite = 1;
    if (strcmp(line, "Y") != 0 && strcmp(line, "y") != 0
        && strcmp(line, "") != 0)
        overwrite = 0;

    free(line);

    return overwrite;
}

/**
 * @see https://stackoverflow.com/a/1786733
 */
char *get_password(const char *prompt)
{
    printf("%s", prompt);
    static struct termios oldt, newt;
    int i = 0;
    int c;
    size_t password_max_len = 8;
    char *password = malloc(password_max_len);
    if (password == NULL)
        return NULL;

    /*saving the old settings of STDIN_FILENO and copy settings for resetting*/
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    /*setting the approriate bit in the termios struct*/
    newt.c_lflag &= ~(ECHO);

    /*setting the new bits*/
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    /*reading the password from the console*/
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (i + 1 == password_max_len)
        {
            char *tmp = realloc(password, password_max_len *= 2);
            if (tmp == NULL)
            {
                free(password);
                return NULL;
            }
            password = tmp;
        }
        password[i++] = c;
    }
    password[i] = '\0';

    /*resetting our old STDIN_FILENO*/
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    printf("\n");
    return password;
}

char *get_hashed_password(int set_password)
{
    char *password1 = NULL;
    char *password_hash = malloc((SHA512_DIGEST_LENGTH * 2) + 1);
    if (password_hash == NULL)
        return NULL;

    if (set_password)
    {
        password1 = get_password("Enter a password for the keys file: ");
        if (password1 == NULL)
        {
            free(password_hash);
            fprintf(stderr, "%sError:%s Password is NULL.\n",
                    colors[COLOR_ERROR], colors[COLOR_RESET]);
            return NULL;
        }
        char *password2 = get_password("Re-type password: ");
        if (password2 == NULL)
        {
            free(password_hash);
            free(password1);
            fprintf(stderr, "%sError:%s Password is NULL.\n",
                    colors[COLOR_ERROR], colors[COLOR_RESET]);
            return NULL;
        }

        if (strlen(password1) != strlen(password2))
        {
            free(password1);
            free(password2);
            free(password_hash);
            fprintf(stderr, "%sError:%s Passwords don't match.\n",
                    colors[COLOR_ERROR], colors[COLOR_RESET]);
            return NULL;
        }
        if (strcmp(password1, password2) != 0)
        {
            free(password1);
            free(password2);
            free(password_hash);
            fprintf(stderr, "%sError:%s Passwords don't match.\n",
                    colors[COLOR_ERROR], colors[COLOR_RESET]);
            return NULL;
        }
        free(password2);
    }
    else
    {
        password1 = get_password("Password: ");
        if (password1 == NULL)
        {
            free(password_hash);
            fprintf(stderr, "%sError:%s Password is NULL.\n",
                    colors[COLOR_ERROR], colors[COLOR_RESET]);
            return NULL;
        }
    }

    unsigned char md[SHA512_DIGEST_LENGTH];
    SHA512((const unsigned char *) password1, strlen(password1), md);
    message_digest_to_hash(md, password_hash, SHA512_DIGEST_LENGTH);

    free(password1);
    if (password_hash == NULL)
        return NULL;

    return password_hash;
}
