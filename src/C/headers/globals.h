#pragma once 

static const unsigned char PADDING = 0;
static const int ROUNDS = 5;
static const char DEFAULT_KEYS_FILE[] = "keys.keys";
static const char EEA_FILE_EXTENTION[] = ".eea";
// Selection 2 (512-bits) in the menu
static const int DEFAULT_KEY_SELECTION = 2;
static const int DEFAULT_NUM_KEYS = 3;
static const char* colors[] = {
    "\x1B[0m",
    "\x1B[33m",
    "\x1B[31m",
};

/**
* @enum COLOR_OPTIONS
* @brief Index for the color options in the colors array
*/
typedef enum
{
    COLOR_RESET = 0,
    COLOR_WARNING = 1,
    COLOR_ERROR = 2
} COLOR_OPTIONS;