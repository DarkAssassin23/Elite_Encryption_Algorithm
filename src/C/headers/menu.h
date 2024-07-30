#pragma once

/**
 * @enum MAIN_MENU_OPTIONS
 * @brief Menu options for the main menu
 */
typedef enum
{
    MAIN_MENU_KEYS = 0,
    MAIN_MENU_ENCRYPT = 1,
    MAIN_MENU_DECRYPT = 2
} MAIN_MENU_OPTIONS;

/**
 * @enum MANAGE_KEYS_MENU_OPTIONS
 * @brief Menu options for the key management menu
 */
typedef enum
{
    MANAGE_KEYS_MENU_ADD = 0,
    MANAGE_KEYS_MENU_DELETE = 1,
    MANAGE_KEYS_MENU_VIEW = 2
} MANAGE_KEYS_MENU_OPTIONS;

/**
 * @enum ENCRYPT_DECRYPT_MENU_OPTIONS
 * @brief Menu options for the encryption menu
 */
typedef enum
{
    ENCRYPT_DECRYPT_MENU_FILE = 0,
    ENCRYPT_DECRYPT_MENU_DIR = 1,
    ENCRYPT_DECRYPT_MENU_TEXT = 2
} ENCRYPT_DECRYPT_MENU_OPTIONS;

static const char *MAIN_MENU_ITEMS[] = { "1. Manage Keys", "2. Encrypt",
                                         "3. Decrypt" };
static const size_t NUM_MAIN_MENU_ITEMS = sizeof(MAIN_MENU_ITEMS)
                                          / sizeof(char *);

static const char *KEY_GEN_MENU_ITEMS[] = { "1. 256-bits", "2. 512-bits",
                                            "3. 1024-bits", "4. 2048-bits",
                                            "5. Other" };

static const unsigned short KEY_BIT_SIZES[] = { 256, 512, 1024, 2048 };
static const size_t NUM_KEY_GEN_MENU_ITEMS = sizeof(KEY_GEN_MENU_ITEMS)
                                             / sizeof(char *);

static const char *MANAGE_KEYS_MENU_ITEMS[] = { "1. Add Keys",
                                                "2. Delete Keys",
                                                "3. View Keys" };
static const size_t NUM_MANAGE_KEYS_MENU_ITEMS = sizeof(MANAGE_KEYS_MENU_ITEMS)
                                                 / sizeof(char *);

static const char *ENCRYPT_DECRYPT_MENU_ITEMS[] = { "single file", "directory",
                                                    "text" };
static const size_t
    NUM_ENCRYPT_DECRYPT_MENU_ITEMS = sizeof(ENCRYPT_DECRYPT_MENU_ITEMS)
                                     / sizeof(char *);

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

/**
 * @brief Prints out the encrypt/decrypt menu
 * @param[in] encrypting If the menu is for encrypting
 */
void print_encrypt_decrypt_menu(int encrypting);
