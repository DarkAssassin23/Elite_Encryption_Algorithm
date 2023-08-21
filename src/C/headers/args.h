#pragma once
#include <stdint.h> // uintx_t

typedef struct
{
    uint8_t key;
    uint8_t verbose;
    uint8_t encrypt_mode;
    uint8_t decrypt_mode;
    uint8_t input_file;
    uint8_t output_file;
    uint8_t help;
} options_t;

typedef enum
{
    MENU_KEYS = 0,
    MENU_ENCRYPT = 1,
    MENU_DECRYPT = 2
} MENU_OPTIONS;

static const char* main_menu_items[] = {
    "1. Manage Keys",
    "2. Encrypt",
    "3. Decrypt"
};
static const size_t num_main_menu_items = sizeof(main_menu_items) / sizeof(char*);

static const char* key_gen_menu_items[] = {
    "1. 256-bits",
    "2. 512-bits",
    "3. 1024-bits",
    "4. 2048-bits",
};
static const size_t num_key_gen_menu_items = sizeof(key_gen_menu_items) / sizeof(char*);

void print_main_menu(void);
void print_key_gen_menu(void);
options_t parse_args(int argc, char** argv);