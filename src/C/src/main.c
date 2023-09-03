#include <stdio.h>          // printf
#include <stdlib.h>         // free strol
#include <stdint.h>         // uintx_t
#include <string.h>         // strlen
#include <openssl/sha.h>    // SHA256*, SHA512*

#include "utils.h"
#include "keygen.h"
#include "encrypt.h"
#include "decrypt.h"
#include "file_handling.h"
#include "menu.h"
#include "app_functions.h"

void test_keygen(void)
{
    for(int x = 0; x < 4; x++)
    {
        char *key = generate_key(x);
        printf("key: %s\n", key);
        free(key);
    }
}

void test_multiple_keygen(void)
{
    for(int x=1; x<=5; x++)
    {
        char** keys = generate_keys(HASH_TYPE_SHA512, x);
        printf("Keys:\n");
        for(int k=0; k<x; k++)
        {
            printf("%d.: %s\n", (k+1), keys[k]);
            free(keys[k]);
        }
        free(keys);
    }
}

void test_hashing(void)
{
    unsigned char md[SHA256_DIGEST_LENGTH];
    char hash[(SHA256_DIGEST_LENGTH * 2) + 1];
    char* hexstr = get_random_hexstr(DEFAULT_KEY_BYTES);

    SHA256((const unsigned char*)hexstr, strlen(hexstr), md);
    message_digest_to_hash(md, hash, SHA256_DIGEST_LENGTH);

    unsigned char md512[SHA512_DIGEST_LENGTH];
    char hash512[(SHA512_DIGEST_LENGTH * 2) + 1];

    SHA512((const unsigned char*)hexstr, strlen(hexstr), md512);
    message_digest_to_hash(md512, hash512, SHA512_DIGEST_LENGTH);

    printf("SHA256 hash of %s: %s\n", hexstr, hash);
    printf("SHA512 hash of %s: %s\n", hexstr, hash512);

    free(hexstr);
}

int test_encrypt_decrypt(void)
{
    int num_keys = 3;
    char** keys = generate_keys(HASH_TYPE_SHA256, num_keys);

    unsigned char* data = NULL;
    size_t file_size = read_in_file("file.txt", &data);
    printf("File contents: %s\n", data);
    printf("File size: %zu bytes\n", file_size);

    unsigned char* cipher_text = NULL;
    size_t cipher_text_size = encrypt(data, file_size, &cipher_text, 
                                        (const char**)keys, num_keys);
    if(cipher_text == NULL)
    {
        printf("Error encrypting data\n");
        return -1;
    }
    
    printf("Keys:\n");
    for(int k=0; k<num_keys; k++)
    {
        printf("%d.: %s\n", (k+1), keys[k]);
    }

    printf("Cipher text: %s\n", cipher_text);
    printf("Cipher text size: %zu\n", cipher_text_size);

    unsigned char* plain_text = NULL;
    size_t plain_text_size = decrypt(cipher_text, cipher_text_size, 
                                    &plain_text, (const char**)keys, 
                                    num_keys);
    if(plain_text == NULL)
    {
        printf("Error decrypting data\n");
        return -1;
    }
    printf("Plain text: %s\n", (char*)plain_text);
    printf("Plain text size: %zu\n", plain_text_size);

    // if(!save_to_file("file.txt.eea", cipher_text, cipher_text_size))
    //     printf("Error saving all the data to the file\n");

    free(data);
    for(int k=0; k<num_keys; k++)
        free(keys[k]);

    free(keys);
    free(cipher_text);
    free(plain_text);
    return 0;
}

void test_get_dir_contents(void)
{
    char dir_name[] = "test";
    char* contents = get_dir_contents(dir_name);
    contents[strlen(contents) - 1] = '\0';
    //printf("%s contents:\n%s\n", dir_name, contents);
    int arr_size = 0;
    char** files_list = split_string(contents, "\n", &arr_size);
    free(contents);
    for(int f = 0; f < arr_size; f++)
    {
        printf("file[%d] = %s\n", (f + 1), files_list[f]);
        free(files_list[f]);
    }
    free(files_list);
}

int main (int argc, char** argv)
{
    //test_hashing();
    //test_keygen();
    //test_multiple_keygen();
    //test_encrypt_decrypt();
    // int test = -1;
    // printf("test is %s\n", (test ? "true": "false"));
    // test_get_dir_contents();
    // return 0;

    // JUST FOR TESTING!!!
    const char* keys[] = {
        "d0b791b86f2f849e97ffc695d604565e6a8acdb4eb4d64ec2cf15cede9fa6d08",
        "f71c14e031db65eab8491f35ea6da5029fc39dbf0be7a671450772688dd514e8",
        "a22d4209649c892c4a95887ca33ff5be421d13bf8100ee03a30dfa718b8e5458"
    };
    const int num_keys = sizeof(keys) / sizeof(char*);
    while(1)
    {
        print_main_menu();

        char* line = NULL;
        size_t line_len = 0;
        line_len = getline(&line, &line_len, stdin);
        // Replace new line with null terminator
        line[line_len-1] = '\0';

        if(strcmp(line, "q") == 0 || strcmp(line, "Q") == 0)
        {
            printf("goodbye.\n");
            free(line);
            return 0;
        }
        
        int selection = strtol(line, NULL, 10);
        free(line);
        if(selection <= 0 || selection > num_main_menu_items)
        {
            printf("Invalid selection\n");
            continue;
        }
        
        MAIN_MENU_OPTIONS menu_selection;
        menu_selection = selection - 1;
        // printf("valid\n");
        // printf("item selected = %s\n", menu_items[menu_selection]);
        switch(menu_selection)
        {
            case MAIN_MENU_KEYS:
                manage_keys();
                break;
            case MAIN_MENU_ENCRYPT:
                do_encryption();
                // if(encrypt_file("file.txt", keys, num_keys))
                //     printf("File encrypted successfully\n");
                break;
            case MAIN_MENU_DECRYPT:
                do_decryption();
                // if(decrypt_file("file.txt.eea", keys, num_keys))
                //     printf("File decrypted successfully\n");
                break;
            default:
                printf("Invalid selection\n");
                break;
        }
        printf("\n");
    }

    // options_t options = parse_args(argc, argv);

    // printf("%d, %d, %d, %d, %d, %d, %d\n",
    //     options.key,
    //     options.verbose,
    //     options.encrypt_mode,
    //     options.decrypt_mode,
    //     options.input_file,
    //     options.output_file,
    //     options.help
    // );

    return 0;
}