#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "base64.h"
#include "decrypt.h"
#include "file_handling.h"
#include "globals.h"
#include "utils.h"

char *get_output_filename(const char *filename, int encrypting)
{
    char *output_filename = NULL;

    if (encrypting)
    {
        output_filename = malloc(strlen(filename) + sizeof(EEA_FILE_EXTENTION)
                                 + 1);
        if (output_filename == NULL)
            return NULL;

        sprintf(output_filename, "%s%s", filename, EEA_FILE_EXTENTION);
        return output_filename;
    }

    size_t output_filename_len = strlen(filename) - sizeof(EEA_FILE_EXTENTION)
                                 + 1;

    output_filename = malloc(output_filename_len + 1);
    if (output_filename == NULL)
        return NULL;

    strncpy(output_filename, filename, output_filename_len);
    output_filename[output_filename_len] = '\0';

    return output_filename;
}

char *get_input_filename(int encrypting)
{
    char *input_filename = NULL;

    printf("Enter the name of the file you would like to %s: ",
           (encrypting) ? "encrypt" : "decrypt");
    char *line = NULL;
    size_t line_len = 0;
    line_len = getline(&line, &line_len, stdin);
    // Replace new line with null terminator
    line[line_len - 1] = '\0';
    line_len--;
    if (!file_exists(line))
    {
        fprintf(stderr, "%sError:%s The file, \'%s\', does not exist\n",
                colors[COLOR_ERROR], colors[COLOR_RESET], line);
        free(line);
        return NULL;
    }

    if (!encrypting)
    {
        if (!is_of_filetype(line, EEA_FILE_EXTENTION))
        {
            fprintf(stderr,
                    "%sError:%s The file needs to be a \'%s\' file "
                    "in order to be decrypted\n",
                    colors[COLOR_ERROR], colors[COLOR_RESET],
                    EEA_FILE_EXTENTION);
            free(line);
            return NULL;
        }
    }

    input_filename = strdup(line);
    free(line);

    return input_filename;
}

char *get_input_dir_name(int encrypting)
{
    char *input_dir = NULL;

    printf("Enter the name of the directory you would like to %s: ",
           (encrypting) ? "encrypt" : "decrypt");
    char *line = NULL;
    size_t line_len = 0;
    line_len = getline(&line, &line_len, stdin);
    // Replace new line with null terminator
    line[line_len - 1] = '\0';
    line_len--;

    FILE_TYPE f_type = get_file_type(line);
    if (f_type != FILE_TYPE_DIR)
    {
        fprintf(stderr, "%sError:%s The \'%s\', is not a directory\n",
                colors[COLOR_ERROR], colors[COLOR_RESET], line);
        free(line);
        return NULL;
    }

    input_dir = strdup(line);
    free(line);
    return input_dir;
}

char **load_keys_from_file(const char *filename, int *total_keys, size_t *len)
{
    // Make sure the file exists and it is a keys file
    if (!file_exists(filename) || !is_of_filetype(filename, ".keys"))
        return NULL;

    unsigned char *encrypted_keys_string = NULL;
    size_t file_size = read_in_file(filename, &encrypted_keys_string);
    if (encrypted_keys_string == NULL)
        return NULL;

    char *keys_string = NULL;
    size_t key_string_len = decrypt_keys(encrypted_keys_string, file_size,
                                         &keys_string);
    free(encrypted_keys_string);
    if (keys_string == NULL)
        return NULL;

    size_t key_len = find_key_len(keys_string);
    if (key_string_len < key_len)
    {
        fprintf(stderr,
                "%sError:%s Failed to decrypt your keys.\n"
                "Make sure the keys in this file are valid "
                "and you entered the correct password\n",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
        free(keys_string);
        return NULL;
    }

    int num_keys = (key_string_len / key_len);
    size_t index = 0;
    char **keys = malloc(sizeof(char *) * num_keys);
    if (keys == NULL)
        return NULL;

    for (int k = 0; k < num_keys; k++)
    {
        char *key = malloc(key_len + 1);
        if (key == NULL)
        {
            fprintf(stderr,
                    "%sError:%s Mallocing memory for keys failed. "
                    "Aborting...",
                    colors[COLOR_ERROR], colors[COLOR_RESET]);
            free(keys_string);
            for (int x = k - 1; x >= 0; x--)
                free(keys[x]);
            free(keys);
            return NULL;
        }
        strncpy(key, &keys_string[index], key_len);
        key[key_len] = '\0';
        keys[k] = key;
        index += key_len + 1;
    }
    free(keys_string);
    *len = key_len;
    *total_keys = num_keys;
    if (!validate_keys((const char **) keys, num_keys))
    {
        fprintf(stderr,
                "%sError:%s Invalid keys detected\n"
                "Make sure you entered your password correctly\n",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
        for (int k = 0; k < num_keys; k++)
            free(keys[k]);
        free(keys);
        return NULL;
        // exit(EXIT_FAILURE);
    }
    return keys;
}

int file_exists(const char *filename)
{
    return (access(filename, F_OK) == 0);
}

int get_file_type(const char *path)
{
    if (!file_exists(path))
        return FILE_TYPE_NA;

    struct stat path_stat;
    stat(path, &path_stat);
    if (S_ISREG(path_stat.st_mode))
        return FILE_TYPE_REG;

    if (S_ISDIR(path_stat.st_mode))
        return FILE_TYPE_DIR;

    return FILE_TYPE_OTHER;
}

int is_of_filetype(const char *filename, const char *extention)
{
    size_t filename_len = strlen(filename);
    size_t extention_len = strlen(extention);
    if (filename_len < extention_len)
        return 0;

    char *cur_file_extention = strrchr(filename, '.');
    if (cur_file_extention == NULL)
        return 0;

    return (strcmp(cur_file_extention, extention) == 0);
}

int keys_file_exists(void)
{
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (!d)
        return 0;

    int success = 0;
    while ((dir = readdir(d)) != NULL)
    {
        if (is_of_filetype(dir->d_name, ".keys"))
        {
            success = 1;
            break;
        }
    }
    closedir(d);

    return success;
}

char **get_all_keys_files(size_t *key_files_count)
{
    char **file_list = calloc(1, sizeof(char *));
    if (file_list == NULL)
    {
        *key_files_count = 0;
        return NULL;
    }

    size_t count = *key_files_count;
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (!d)
        return 0;

    while ((dir = readdir(d)) != NULL)
    {
        if (is_of_filetype(dir->d_name, ".keys"))
        {
            char **tmp = realloc(file_list,
                                 (sizeof(file_list) * count) + sizeof(char *));
            if (tmp == NULL)
            {
                *key_files_count = 0;
                free(file_list);
                return NULL;
            }
            file_list = tmp;
            file_list[count] = strdup(dir->d_name);
            count++;
        }
    }
    closedir(d);
    *key_files_count = count;

    return file_list;
}

char *get_dir_contents(char *basePath)
{
    size_t files_list_max_len = 64;
    size_t files_list_curr_len = 0;
    char *files_list = calloc((files_list_max_len + 1), sizeof(char));
    if (files_list == NULL)
        return NULL;

    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir)
        return 0;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            size_t path_len = (strlen(basePath) + strlen(dp->d_name) + 2);
            char path[path_len];

            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            int path_file_type = get_file_type(path);
            if (path_file_type == FILE_TYPE_DIR)
            {
                char *temp = get_dir_contents(path);
                size_t new_len = strlen(temp) + files_list_curr_len;

                if (new_len >= files_list_max_len)
                {
                    while (new_len >= files_list_max_len)
                        files_list_max_len *= 2;

                    char *tmp = realloc(files_list, files_list_max_len + 1);
                    if (tmp == NULL)
                    {
                        free(files_list);
                        free(temp);
                        closedir(dir);
                        return NULL;
                    }
                    files_list = tmp;
                }

                strcat(files_list, temp);
                files_list_curr_len = new_len;
                free(temp);
            }
            else
            {
                strcat(path, "\n");
                size_t new_len = path_len + files_list_curr_len;
                if (new_len >= files_list_max_len)
                {
                    while (new_len >= files_list_max_len)
                        files_list_max_len *= 2;

                    char *tmp = realloc(files_list, files_list_max_len + 1);
                    if (tmp == NULL)
                    {
                        free(files_list);
                        closedir(dir);
                        return NULL;
                    }
                    files_list = tmp;
                }

                strcat(files_list, path);
                files_list_curr_len = new_len;
            }
        }
    }
    char *tmp = realloc(files_list, files_list_curr_len + 1);
    if (tmp == NULL)
    {
        free(files_list);
        closedir(dir);
        return NULL;
    }
    files_list = tmp;
    closedir(dir);
    return files_list;
}

int save_to_file(const char *filename, unsigned char *data,
                 size_t bytes_to_write)
{
    FILE *fout = fopen(filename, "wb");
    if (fout == NULL)
    {
        fprintf(stderr, "%sError:%s Failed to open the file \'%s\'\n",
                colors[COLOR_ERROR], colors[COLOR_RESET], filename);
        return 0;
    }

    fwrite(data, sizeof(data[0]), bytes_to_write, fout);
    fclose(fout);
    return 1;
}

size_t read_in_file(const char *filename, unsigned char **buffer)
{
    FILE *fin = fopen(filename, "rb");
    if (fin == NULL)
    {
        fprintf(stderr, "%sError:%s Failed to open the file \'%s\'\n",
                colors[COLOR_ERROR], colors[COLOR_RESET], filename);
        return -1;
    }

    fseek(fin, 0, SEEK_END);
    size_t file_size = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    *buffer = calloc(file_size + 1, sizeof(char));
    if (*buffer == NULL)
        return -1;

    size_t read_bytes = fread(*buffer, sizeof(unsigned char), file_size, fin);
    fclose(fin);

    return read_bytes;
}
