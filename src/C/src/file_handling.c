#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "globals.h"
#include "utils.h"
#include "decrypt.h"
#include "file_handling.h"

char* get_output_filename(const char* filename, int encrypting)
{
    char* output_filename = NULL;

    if(encrypting)
    {
        output_filename = malloc(strlen(filename) + 
                                    sizeof(EEA_FILE_EXTENTION) + 1);
        if(output_filename == NULL)
            return NULL;

        sprintf(output_filename, "%s%s", filename, EEA_FILE_EXTENTION);
        return output_filename;
    }

    size_t output_filename_len = strlen(filename) - 
                                        sizeof(EEA_FILE_EXTENTION) + 1;

    output_filename = malloc(output_filename_len + 1);
    if(output_filename == NULL)
            return NULL;

    strncpy(output_filename, filename, output_filename_len);
    output_filename[output_filename_len] = '\0';

    return output_filename;
}

char* get_input_filename(int encrypting)
{
    char* input_filename = NULL;
    
    printf("Enter the name of the file you would like to %s: ",
            (encrypting) ? "encrypt" : "decrypt");
    char* line = NULL;
    size_t line_len = 0;
    line_len = getline(&line, &line_len, stdin);
    // Replace new line with null terminator
    line[line_len-1] = '\0';
    line_len--;
    if(!file_exists(line))
    {
        fprintf(stderr, "Error: The file, \'%s\', does not exist\n", line);
        free(line);
        return NULL;
    }

    if(!encrypting)
    {
        if(!is_of_filetype(line, EEA_FILE_EXTENTION))
        {
            fprintf(stderr, "Error: The file needs to be a \'%s\' file "
                "in order to be decrypted\n", EEA_FILE_EXTENTION);
            free(line);
            return NULL;
        }
    }

    input_filename = strdup(line);
    free(line);

    return input_filename;
}

char** load_keys_from_file(const char* filename, int* total_keys, size_t* len)
{
    // Make sure the file exists and it is a keys file
    if(!file_exists(filename) || !is_of_filetype(filename, ".keys"))
        return NULL;

    unsigned char* encrypted_keys_string = NULL;
    size_t file_size = read_in_file(filename, &encrypted_keys_string);
    if(encrypted_keys_string == NULL)
        return NULL;

    char* keys_string = NULL;
    size_t key_string_len = decrypt_keys(encrypted_keys_string,
                                            file_size, &keys_string);
                                        
    free(encrypted_keys_string);
    if(keys_string == NULL)
        return NULL;

    size_t key_len = find_key_len(keys_string);
    if(key_string_len < key_len)
    {
        fprintf(stderr, "An error occured decrypting your keys.\n"
            "Make sure the keys in this file are valid "
            "and you entered the correct password\n");
        free(keys_string);
        return NULL;
    }

    int num_keys = (key_string_len / key_len);
    size_t index = 0;
    char** keys = malloc(sizeof(char*) * num_keys);
    if(keys == NULL)
        return NULL;

    for(int k = 0; k < num_keys; k++)
    {
        char* key = malloc(key_len + 1);
        if(key == NULL)
        {
            fprintf(stderr, "Error occurred mallocing memory for keys. "
                "Aborting...");
            exit(EXIT_FAILURE);
        }
        strncpy(key, &keys_string[index], key_len);
        key[key_len] = '\0';
        keys[k] = key;
        index += key_len + 1;
    }
    free(keys_string);
    *len = key_len;
    *total_keys = num_keys;
    if(!validate_keys((const char**)keys, num_keys))
    {
        fprintf(stderr,"Error: Invalid keys detected\n"
            "Make sure you entered your password correctly\n");

        exit(EXIT_FAILURE);
    }
    return keys;
}

int file_exists(const char* filename)
{
    return (access(filename, F_OK) == 0);
}

int get_file_type(const char* path)
{
    if(!file_exists(path))
        return FILE_TYPE_NA;

    struct stat path_stat;
    stat(path, &path_stat);
    if(S_ISREG(path_stat.st_mode))
        return FILE_TYPE_REG;

    if(S_ISDIR(path_stat.st_mode))
        return FILE_TYPE_DIR;

    return FILE_TYPE_OTHER;
}

int is_of_filetype(const char* filename, const char* extention)
{
    size_t filename_len = strlen(filename);
    size_t extention_len = strlen(extention);
    if(filename_len < extention_len)
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
        if(is_of_filetype(dir->d_name, ".keys"))
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
        if(is_of_filetype(dir->d_name, ".keys"))
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

int save_to_file(const char* filename, unsigned char* data, size_t bytes_to_write)
{
    FILE* fout = fopen(filename, "wb");
    if(fout == NULL)
    {
        printf("Error opening file \'%s\'\n", filename);
        return 0;
    }

    fwrite(data, sizeof(data[0]), bytes_to_write, fout);
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

    return read_bytes;
}