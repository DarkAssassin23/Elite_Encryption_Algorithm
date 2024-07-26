#pragma once
#define MAJOR_VERSION 2
#define MINOR_VERSION 0
#define PATCH_VERSION 0
#define BUILD_VERSION 0

#define COPYRIGHT_YEAR 2024

#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)

// clang-format off
#define APP_VERSION_STRING       \
    STRINGIZE(MAJOR_VERSION) "." \
    STRINGIZE(MINOR_VERSION) "." \
    STRINGIZE(PATCH_VERSION) "." \
    STRINGIZE(BUILD_VERSION) "\0"
// clang-format on

#define COPYRIGHT_STRING "Dark Assassins Inc. " STRINGIZE(COPYRIGHT_YEAR)

#define WIN_FILENAME "eea.exe"
