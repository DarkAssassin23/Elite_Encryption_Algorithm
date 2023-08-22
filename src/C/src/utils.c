#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <openssl/sha.h>

// #ifdef WIN32
// #include <io.h>
// #define F_OK 0
// #define access _access
// #else
#include <unistd.h>
#include <termios.h>
// #endif

#include <openssl/rand.h>

#include "utils.h"

static const int MAX_TRIES = 5;

void message_digest_to_hash(unsigned char* md, char* hash, int digest_length)
{
    for(int x=0; x<digest_length; x++)
        sprintf(&hash[x*2], "%02hhx", md[x]);
}

char* get_random_uint64_hexstr(void)
{
    unsigned char buffer[8];
    int tries = 0;
    int rc = 1;
    do
    {
        rc = RAND_bytes(buffer, sizeof(buffer));
        if(rc != 1)
            tries++;

    } while( rc != 1 && tries < MAX_TRIES);

    if(rc != 1)
    {
        printf("Error: exceeded the max tries to get random bytes\n");
        exit(rc);
    }

    char *hexstr = malloc((sizeof(buffer) * 2)+ 1);
    if(hexstr == NULL)
        return NULL;

    for(int x=0; x<sizeof(buffer); x++)
        sprintf(&hexstr[x*2], "%02hhx", buffer[x]);

    return hexstr;
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

char* get_hashed_password(void)
{
    char* password1 = get_password("Enter a password for the keys file: ");
    char* password2 = get_password("Re-type password: ");

    if(strlen(password1) != strlen(password2))
    {
        free(password1);
        free(password2);
        fprintf(stderr,"Error: Passwords don't match\n");
        return NULL;
    }
    if(strcmp(password1, password2) != 0)
    {
        free(password1);
        free(password2);
        fprintf(stderr,"Error: Passwords don't match\n");
        return NULL;
    }
    char* password_hash = malloc((SHA512_DIGEST_LENGTH * 2) + 1);
    if(password_hash == NULL)
    {
        free(password1);
        free(password2);
        return NULL;
    }

    unsigned char md[SHA512_DIGEST_LENGTH];

    SHA512((const unsigned char*)password1, strlen(password1), md);
    message_digest_to_hash(md, password_hash, SHA512_DIGEST_LENGTH);

    free(password1);
    free(password2);

    if(password_hash == NULL)
        return NULL;

    return password_hash;
}

int file_exists(const char* filename)
{
    return (access(filename, F_OK) == 0);
}

int is_keys_file(const char* filename)
{
    const char extention[] = ".keys";
    size_t filename_len = strlen(filename);
    if(filename_len < sizeof(extention))
        return 0;
    
    char* cur_file_extention = strrchr(filename, '.');
    if(cur_file_extention == NULL)
        return 0;

    return (strcmp(cur_file_extention, extention) == 0);
}

int keys_file_exists(void)
{
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if(!d)
        return 0;

    int success = 0;
    while ((dir = readdir(d)) != NULL) 
    {
        if(is_keys_file(dir->d_name))
        {
            success = 1;
            break;
        }
    }
    closedir(d);

    return success;
}

char** get_all_keys_files(size_t* key_files_count)
{
    char** file_list = malloc(sizeof(char*));
    if(file_list == NULL)
    {
        *key_files_count = 0;
        return NULL;
    }

    size_t count = *key_files_count;
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if(!d)
        return 0;

    while ((dir = readdir(d)) != NULL) 
    {
        if(is_keys_file(dir->d_name))
        {
            size_t cur_filename_len = strlen(dir->d_name);

            file_list = realloc(file_list, sizeof(file_list) + cur_filename_len + 1);

            if(file_list == NULL)
            {
                *key_files_count = 0;
                return NULL;
            }

            file_list[count] = strdup(dir->d_name);
            count++;
        }
    }
    closedir(d);
    *key_files_count = count;

    return file_list;
}

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

int save_to_file(const char* filename, unsigned char* data, size_t bytes_to_write)
{
    FILE* fout = fopen(filename, "wb");
    if(fout == NULL)
    {
        printf("Error opening file \'%s\'\n", filename);
        return 0;
    }

    size_t bytes_written = fwrite(data, sizeof(data[0]), bytes_to_write, fout);
    printf("Wrote %zu bytes out of %zu requested\n", bytes_written,  bytes_to_write);
    fclose(fout);
    return 1;
}

size_t read_in_file(const char* filename, unsigned char** buffer)
{
    FILE* fin = fopen(filename, "rb");
    if(fin == NULL)
    {
        printf("Error opening file \'%s\'\n", filename);
        return -1;
    }

    fseek(fin, 0, SEEK_END);
    size_t file_size = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    *buffer = malloc(file_size + 1);
    if(*buffer == NULL)
        return -1;

    size_t read_bytes = fread(*buffer, sizeof(unsigned char), file_size, fin);
    fclose(fin);
    buffer[read_bytes] = 0;
    return read_bytes;
}