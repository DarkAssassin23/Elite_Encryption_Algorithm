#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "globals.h"
#include "menu.h"

void print_main_menu(void)
{
    printf("Select one of the following options: \n");
    for(size_t x = 0; x < num_main_menu_items; x++)
        printf("%s\n", main_menu_items[x]);
    printf("(1-%zu) or 'q' to quit: ", num_main_menu_items);
}

void print_key_gen_menu(void)
{
    printf("Select how large would you like your keys to be: \n");
    for(size_t x = 0; x < num_key_gen_menu_items; x++)
        printf("%s\n", key_gen_menu_items[x]);
    printf("(1-%zu) or 'q' to quit (default: %d): ", 
        num_key_gen_menu_items, DEFAULT_KEY_SELECTION);
}

void print_manage_keys_menu(void)
{
    printf("Select one of the following options: \n");
    for(size_t x = 0; x < num_manage_keys_menu_items; x++)
        printf("%s\n", manage_keys_menu_items[x]);
    printf("(1-%zu) or 'q' to quit: ", num_manage_keys_menu_items);
}

void print_encrypt_decrypt_menu(int encrypting)
{
    printf("Select one of the following options: \n");
    for(size_t x = 0; x < num_encrypt_decrypt_menu_items; x++)
        printf("%lu. %s %s\n", (x + 1), 
            encrypting ? "Encrypt" : "Decrypt", 
            encrypt_decrypt_menu_items[x]);
    printf("(1-%zu) or 'q' to quit (default: 1): ", num_encrypt_decrypt_menu_items);
}

options_t parse_args(int argc, char** argv)
{
    options_t options = {0};
    int opt;
    uint8_t option_selected = 0;
    while ((opt = getopt(argc, argv, "kioedvh")) != -1) 
    {
        switch (opt) 
        {
            // ((!option_selected) ? 1 : 0) = if option_selected
            // is not true return 1, otherwise 0
            case 'k':
                options.key = ((!option_selected) ? 1 : 0);
                option_selected = 1;
                break;
            case 'i': 
                options.input_file = 1; 
                option_selected = 1;
                break;
            case 'o': 
                options.output_file = 1; 
                option_selected = 1;
                break;
            case 'e': 
                options.encrypt_mode = ((!option_selected) ? 1 : 0); 
                option_selected = 1;
                break;
            case 'd': 
                options.decrypt_mode = ((!option_selected) ? 1 : 0); 
                option_selected = 1;
                break;
            case 'v': 
                options.verbose = 1; 
                break;
            case 'h': 
                options.help = ((!option_selected) ? 1 : 0); 
                option_selected = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-kiedvh] [file...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    return options;
}