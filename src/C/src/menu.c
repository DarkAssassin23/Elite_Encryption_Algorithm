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