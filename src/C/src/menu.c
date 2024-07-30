#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "globals.h"
#include "menu.h"

void print_main_menu(void)
{
    printf("Select one of the following options: \n");
    for (size_t x = 0; x < NUM_MAIN_MENU_ITEMS; x++)
        printf("%s\n", MAIN_MENU_ITEMS[x]);
    printf("(1-%zu) or 'q' to quit: ", NUM_MAIN_MENU_ITEMS);
}

void print_key_gen_menu(void)
{
    printf("Select how large would you like your keys to be: \n");
    for (size_t x = 0; x < NUM_KEY_GEN_MENU_ITEMS; x++)
        printf("%s\n", KEY_GEN_MENU_ITEMS[x]);
    printf("(1-%zu) or 'q' to quit (default: %d): ", NUM_KEY_GEN_MENU_ITEMS,
           DEFAULT_KEY_SELECTION);
}

void print_manage_keys_menu(void)
{
    printf("Select one of the following options: \n");
    for (size_t x = 0; x < NUM_MANAGE_KEYS_MENU_ITEMS; x++)
        printf("%s\n", MANAGE_KEYS_MENU_ITEMS[x]);
    printf("(1-%zu) or 'q' to quit: ", NUM_MANAGE_KEYS_MENU_ITEMS);
}

void print_encrypt_decrypt_menu(int encrypting)
{
    printf("Select one of the following options: \n");
    for (size_t x = 0; x < NUM_ENCRYPT_DECRYPT_MENU_ITEMS; x++)
        printf("%lu. %s %s\n", (x + 1), encrypting ? "Encrypt" : "Decrypt",
               ENCRYPT_DECRYPT_MENU_ITEMS[x]);
    printf("(1-%zu) or 'q' to quit (default: 1): ",
           NUM_ENCRYPT_DECRYPT_MENU_ITEMS);
}
