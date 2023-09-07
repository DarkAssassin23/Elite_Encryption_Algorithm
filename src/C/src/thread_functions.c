#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "globals.h"
#include "utils.h"
#include "file_handling.h"
#include "encrypt.h"
#include "decrypt.h"

/**
* @struct thread_data_t
* @brief Simple struct to hold all the data each thread will need
*/
typedef struct
{
    char** files_list;
    int start;
    int end;
    const char** keys;
    int num_keys;
    int overwrite;
    int encrypting;
} thread_data_t;

static volatile int running_threads = 0;
static pthread_mutex_t running_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
* @brief Encrypt all files in the provided list
* @param[in] files_list The list files to be encrypted
* @param[in] start Index in the files list to start
* @param[in] end Index in the files list to end
* @param[in] keys The keys to be used for encryption
* @param[in] num_keys The number of keys
* @param[in] overwrite Should the files be overwritten
* @note The array of files will be freed
*/
static void encrypt_list_of_files(char** files_list, int start, int end,
                                    const char** keys, int num_keys,
                                    int overwrite)
{
    for(int f = start; f < end; f++)
    {
        int encryption_success = encrypt_file(files_list[f], keys, num_keys);
        if(encryption_success)
            fprintf(stdout, "%sEncryption success:%s %s\n", 
                colors[COLOR_SUCCESS], colors[COLOR_RESET], files_list[f]);
        else
            fprintf(stderr, "%sEncryption failed:%s  %s\n",
                colors[COLOR_ERROR], colors[COLOR_RESET], files_list[f]);
        
        if(encryption_success && overwrite)
            remove(files_list[f]);

        free(files_list[f]);
    }
    pthread_mutex_lock(&running_mutex);
    running_threads--;
    pthread_mutex_unlock(&running_mutex);
}

/**
* @brief Decrypt all files in the provided list
* @param[in] files_list The list files to be decrypted
* @param[in] start Index in the files list to start
* @param[in] end Index in the files list to end
* @param[in] keys The keys to be used for decryption
* @param[in] num_keys The number of keys
* @param[in] overwrite Should the files be overwritten
* @note The array of files will be freed
*/
static void decrypt_list_of_files(char** files_list, int start, int end,
                                    const char** keys, int num_keys,
                                    int overwrite)
{
    for(int f = start; f < end; f++)
    {
        int decryption_success = 0;
        if(is_of_filetype(files_list[f], EEA_FILE_EXTENTION))
            decryption_success = decrypt_file(files_list[f], 
                                                keys, 
                                                num_keys);
        else
        {
            free(files_list[f]);
            continue;
        }

        if(decryption_success)
            fprintf(stdout, "%sDecryption success:%s %s\n", 
                colors[COLOR_SUCCESS], colors[COLOR_RESET], files_list[f]);
        else
            fprintf(stderr, "%sDecryption failed:%s  %s\n",
                colors[COLOR_ERROR], colors[COLOR_RESET], files_list[f]);
        
        if(decryption_success && overwrite)
            remove(files_list[f]);

        free(files_list[f]);
    }
    pthread_mutex_lock(&running_mutex);
    running_threads--;
    pthread_mutex_unlock(&running_mutex);
}

/**
* @brief Function called by pthread_create to start each thread
* @param[in] args A thread_data_t struct to house the data for the thread
*/
static void* start_thread(void *args)
{
    thread_data_t* data = (thread_data_t*)args;
    pthread_mutex_lock(&running_mutex);
    running_threads++;
    pthread_mutex_unlock(&running_mutex);
    if(data->encrypting)
        encrypt_list_of_files(data->files_list, 
                                data->start,
                                data->end,
                                data->keys,
                                data->num_keys,
                                data->overwrite); 

    else
        decrypt_list_of_files(data->files_list, 
                                data->start,
                                data->end,
                                data->keys,
                                data->num_keys,
                                data->overwrite); 
    return NULL;
}

/**
* @brief Function to initialize the threads and set the thread_data_t data
* @param[in] files_list The list files to be decrypted
* @param[in] num_files The number of files to be decrypted
* @param[in] keys The keys to be used for decryption
* @param[in] num_keys The number of keys
* @param[in] overwrite Should the files be overwritten
* @param[in] threads The number of threads to use
* @param[in] encrypting Are we encrypting the files
*/
void init_threads(char** files_list, int num_files,
                    const char** keys, int num_keys, 
                    int overwrite, int threads, int encrypting)
{
    int files_per_thread = num_files / threads;
    int leftover = num_files % threads;
    int start = 0;

    thread_data_t data[threads];
    pthread_t threadPool[threads];
    for(int t = 0; t < threads; t++)
    {
        int end = start + files_per_thread;
        if(leftover > 0)
        {
            end++;
            leftover--;
        }

        data[t].files_list = files_list;
        data[t].start = start;
        data[t].end = end;
        data[t].keys = keys;
        data[t].num_keys = num_keys;
        data[t].overwrite = overwrite;
        data[t].encrypting = encrypting;

        pthread_create(&threadPool[t], NULL, start_thread, &data[t]);
        start = end;
    }

    // Make sure all threads finish running before returning
    // Fixes issue on Windows where the function returns, and frees 
    // the file_list before the threads finish causing them to crash
    struct timespec ts;

    // Sleep for a quarter second
    ts.tv_sec = 250 / 1000;
    ts.tv_nsec = (250 % 1000) * 1000000;

    while(running_threads > 0)
        nanosleep(&ts, &ts);

    for(int t = 0; t < threads; t++)
        pthread_join(threadPool[t], NULL);

}

void start_dir_encrypt_threads(char** files_list, int num_files,
                                const char** keys, int num_keys, 
                                int overwrite, int threads)
{
    if(threads > num_files)
        threads = num_files;

    if(threads <= 1)
    {
        encrypt_list_of_files(files_list, 0, num_files, 
                                keys, num_keys, overwrite);
        free(files_list);
        return;
    }
    init_threads(files_list, num_files, keys, num_keys, overwrite,
                    threads, 1);
    free(files_list);
}

void start_dir_decrypt_threads(char** files_list, int num_files,
                                const char** keys, int num_keys, 
                                int overwrite, int threads)
{
    if(threads > num_files)
        threads = num_files;

    if(threads <= 1)
    {
        decrypt_list_of_files(files_list, 0, num_files, 
                                keys, num_keys, overwrite);
        free(files_list);
        return;
    }

    init_threads(files_list, num_files, keys, num_keys, overwrite,
                    threads, 0);
    free(files_list);
}