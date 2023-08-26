#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <openssl/sha.h>

#include "globals.h"
#include "utils.h"
#include "file_handling.h"
#include "prompts.h"

int prompt_for_keys_filename(char** filename)
{
    while(1)
    {
        printf("Enter the filename to save your keys to. ");
        printf("It should end in .keys\n");
        printf("Filename or 'q' to quit (default: %s): ", DEFAULT_KEYS_FILE);
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
        
        // Use the default
        if(strcmp(line, "") == 0)
        {
            free(line);
            line = strdup(DEFAULT_KEYS_FILE);
        }
        else if(!is_of_filetype(line, ".keys"))
        {
            printf("Invalid filename. It should end in .keys\n");
            free(line);
            continue;
        }

        if(file_exists(line))
        {
            printf("WARNING the file \'%s\' already exists.\n", line);
            printf("Are you sure you want to override it? (y/n) (default: n): ");
            char* choice = NULL;
            size_t len = 0;
            len = getline(&choice, &len, stdin);
            // Replace new line with null terminator
            choice[len-1] = '\0';
            if(strcmp(choice, "y") != 0 && strcmp(choice, "Y") != 0)
            {
                free(choice);
                free(line);
                continue;
            }
            free(choice);
        }
        *filename = line;
        return 1;
    }
}

int prompt_for_num_keys(void)
{
    while(1)
    {
        printf("Enter the number of keys to generate "
            "or 'q' to quit (default %d): ", DEFAULT_NUM_KEYS);
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

        int num_keys = 0;
        int is_negative = 0;

        if(strcmp(line, "") == 0)
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

int prompt_for_ghost_mode_confirmation(void)
{
    int success = 0;
    printf("%sWarning:%s You are about to encrypt the data "
        "with one time use keys.\nYou will be unable to decrypt the data "
        "unless you manually copy down\nthe keys after you encrypt your data"
		"\n\nAre you sure you would like to continue? (y/n) (default: n) ",
        colors[COLOR_WARNING], colors[COLOR_RESET]);
    char* line = NULL;
    size_t line_len = 0;
    line_len = getline(&line, &line_len, stdin);
    // Replace new line with null terminator
    line[line_len-1] = '\0';

    if(strcmp(line, "y") == 0 || strcmp(line, "Y") == 0)
        success = 1;
    free(line);

    return success;
}

/**
* @see https://stackoverflow.com/a/1786733
*/
char* get_password(const char* prompt)
{
    printf("%s", prompt);
    static struct termios oldt, newt;
    int i = 0;
    int c;
    size_t password_max_len = 8;
    char* password = malloc(password_max_len);
    if(password == NULL)
        return NULL;

    /*saving the old settings of STDIN_FILENO and copy settings for resetting*/
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;

    /*setting the approriate bit in the termios struct*/
    newt.c_lflag &= ~(ECHO);          

    /*setting the new bits*/
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);

    /*reading the password from the console*/
    while ((c = getchar())!= '\n' && c != EOF)
    {
        if(i+1 == password_max_len)
        {
            password = realloc(password, password_max_len *= 2);
            if(password == NULL)
                return NULL;
        }
        password[i++] = c;
    }
    password[i] = '\0';

    /*resetting our old STDIN_FILENO*/ 
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    printf("\n");
    return password;
}

char* get_hashed_password(int set_password)
{
    char* password1 = NULL;
    char* password_hash = malloc((SHA512_DIGEST_LENGTH * 2) + 1);
    if(password_hash == NULL)
        return NULL;
    
    if(set_password)
    {
        password1 = get_password("Enter a password for the keys file: ");
        char* password2 = get_password("Re-type password: ");

        if(strlen(password1) != strlen(password2))
        {
            free(password1);
            free(password2);
            fprintf(stderr,"%sError:%s Passwords don't match\n",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
            return NULL;
        }
        if(strcmp(password1, password2) != 0)
        {
            free(password1);
            free(password2);
            fprintf(stderr,"%sError:%s Passwords don't match\n",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
            return NULL;
        }
        free(password2);
    }
    else
        password1 = get_password("Password: ");

    unsigned char md[SHA512_DIGEST_LENGTH];

    SHA512((const unsigned char*)password1, strlen(password1), md);
    message_digest_to_hash(md, password_hash, SHA512_DIGEST_LENGTH);

    free(password1);

    if(password_hash == NULL)
        return NULL;

    return password_hash;
}