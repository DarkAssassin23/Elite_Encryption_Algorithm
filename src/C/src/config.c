#include <limits.h>
#include <stdio.h>
#include <string.h>

#if defined(WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

#if defined(__APPLE__)
#include <libproc.h>
#include <sys/types.h>
#endif

#include "config.h"
#include "file_handling.h"
#include "globals.h"
#include "utils.h"

static char CONFIG_FILE[] = "eea.conf";

/**
 * @brief Generate the default config file.
 * @param[in] path Path to the default config file
 */
static void write_default_config(const char *path)
{
    const char *cfg =
        "# Elite Encryption Algorithm (EEA) config file\n\n"
        "# The directory to search for your '.keys' files in.\n"
        "# NOTE: The default is the same directory as the EEA executable\n"
        "# keysDir: ~/.eeaKeys\n";
    if (!save_to_file(path, (unsigned char *) cfg, strlen(cfg)))
    {
        fprintf(stderr, "%sError:%s, Failed to open default config\n",
                colors[COLOR_ERROR], colors[COLOR_RESET]);
        return;
    }
}

/**
 * @brief Return the full path to the EEA executable
 * @param[out] path_len The length of the path to the executable
 * @return Path to EEA executable
 * @note Return value must be freed
 */
static char *exe_path(size_t *path_len)
{
    char *path = calloc(PATH_MAX + 1, sizeof(char));
    if (path == NULL)
        return NULL;

#if defined(WIN32)
    if (GetModuleFileName(NULL, path, PATH_MAX) == 0)
    {
        free(path);
        return NULL;
    }
#elif defined(__APPLE__)
    pid_t pid = getpid();
    if (proc_pidpath(pid, path, PATH_MAX) != 0)
    {
        free(path);
        return NULL;
    }
#else
    if (readlink("/proc/self/exe", path, PATH_MAX) == -1)
    {
        free(path);
        return NULL;
    }
#endif

    char *pch = strrchr(path, SLASH_CH) + 1;
    *pch = 0;
    *path_len = strnlen(path, PATH_MAX);
    return path;
}

/**
 * @brief Get the absolute path to the users home directory
 * @return Path to users home directory
 * @note Return value must be freed
 */
static char *get_home(void)
{
    char home[PATH_MAX + 1] = { 0 };
#ifdef WIN32
    snprintf(home, PATH_MAX, "%s%s", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
#else
    snprintf(home, PATH_MAX, "%s", getenv("HOME"));
#endif
    return strdup(home);
}

/**
 * @brief Upate the path to ensure its slashes are all going the right way
 * @param[in] path The path to updated the slashes for
 * @return The path with the correct slashes
 */
static char *fix_slashes(char *path)
{
#if WIN32
    char bad_slash = '/';
#else
    char bad_slash = '\\';
#endif
    char *p = path;
    while (*p)
    {
        if (*p == bad_slash)
            *p = SLASH_CH;
        p++;
    }
    return path;
}

/**
 * @brief Parse the config file and set the requisite variables
 * @param[in] cfg Path to the config file
 */
static void parse_cfg(const char *cfg)
{
    FILE *config = fopen(cfg, "r");
    if (config == NULL)
        return;

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, config)) != -1)
    {
        // Ignore comments (#, //) and new lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
            continue;
        if (len > 2)
            if (line[0] == '/' && line[1] == '/')
                continue;

        char *result = trim(line);
        char *value = strchr(result, ':') + 1;
        char key[(value - result)];
        memset(key, 0, sizeof(key));
        strncpy(key, result, sizeof(key) - 1);

        if (strcmp(key, "keysDir") == 0)
        {
            char actual[PATH_MAX + 1] = { 0 };
            char *path = trim(value);
            if (starts_with(path, "~/"))
            {
                char *home = get_home();
                if (home == NULL)
                {
                    fprintf(stderr,
                            "%sError:%s Failed to allocate memory for home\n",
                            colors[COLOR_ERROR], colors[COLOR_RESET]);
                    continue;
                }
                snprintf(actual, PATH_MAX, "%s%c%s", home, SLASH_CH, path + 2);
                free(home);
            }
            else
                strncpy(actual, path, PATH_MAX);

            size_t actual_len = strlen(actual);
            if (actual[actual_len] != SLASH_CH)
            {
                actual[actual_len] = SLASH_CH;
                actual_len++;
            }

            keys_dir = calloc(actual_len + 1, sizeof(char));
            if (keys_dir != NULL)
            {
                strncpy(keys_dir, actual, actual_len);
                keys_dir = fix_slashes(keys_dir);
            }
        }
    }
    free(line);
    fclose(config);
}

/**
 * @brief Ask the user if they would like to create the keys dir
 * @param[in] path The path that will be created
 * @return Whether to proceed with creating the dir
 */
int confirm_key_dir(const char *path)
{
    char *line = NULL;
    int ret = 1;
    size_t line_len = 0;

    printf("The keys directory specified, \'%s\', does not exist\n"
           "Would you like to create it? (y/n) (default: y): ",
           path);
    line_len = getline(&line, &line_len, stdin);
    // Replace new line with null terminator
    line[line_len - 1] = '\0';
    if (strcmp(line, "n") == 0 || strcmp(line, "N") == 0)
        ret = 0;

    free(line);
    return ret;
}

void load_config(void)
{
    size_t len = 0;
    char *cfg = exe_path(&len);
    if (cfg == NULL || len + sizeof(CONFIG_FILE) > PATH_MAX)
        return;
    strcat(cfg, CONFIG_FILE);

    if (!file_exists(cfg))
    {
        write_default_config(cfg);
        goto load_config_end;
    }
    parse_cfg(cfg);

    if (keys_dir == NULL)
        goto load_config_end;

    if (!file_exists(keys_dir))
    {
        if (!confirm_key_dir(keys_dir))
        {
            printf("Did not create \'%s\', using default.\n", keys_dir);
            free(keys_dir);
            keys_dir = NULL;
            goto load_config_end;
        }
        printf("Trying to create: %s\n", keys_dir);

        // Try to make the path to the keys directory
        if (!mkdir_path(keys_dir))
        {
            fprintf(stderr,
                    "%sError:%s Failed to create the directory \'%s\'\n",
                    colors[COLOR_ERROR], colors[COLOR_RESET], keys_dir);
            free(keys_dir);
            keys_dir = NULL;
        }
        printf("Success!\n");
    }

load_config_end:
    free(cfg);
    return;
}
