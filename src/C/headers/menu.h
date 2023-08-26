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
    MAIN_MENU_KEYS = 0,
    MAIN_MENU_ENCRYPT = 1,
    MAIN_MENU_DECRYPT = 2
} MAIN_MENU_OPTIONS;

typedef enum
{
    MANAGE_KEYS_MENU_ADD = 0,
    MANAGE_KEYS_MENU_DELETE = 1,
    MANAGE_KEYS_MENU_VIEW = 2
} MANAGE_KEYS_MENU_OPTIONS;

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

static const char* manage_keys_menu_items[] = {
    "1. Add Keys",
    "2. Delete Keys",
    "3. View Keys"
};
static const size_t num_manage_keys_menu_items = sizeof(manage_keys_menu_items) / sizeof(char*);

/**
* @brief Prints out the main menu
*/
void print_main_menu(void);

/**
* @brief Prints out the key generation menu
*/
void print_key_gen_menu(void);

/**
* @brief Prints out the key management menu
*/
void print_manage_keys_menu(void);
options_t parse_args(int argc, char** argv);