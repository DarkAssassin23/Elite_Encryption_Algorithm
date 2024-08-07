#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app_functions.h"
#include "config.h"
#include "menu.h"

char *keys_dir = NULL;
int main(int argc, char **argv)
{
    load_config();
    while (1)
    {
        print_main_menu();

        char *line = NULL;
        size_t line_len = 0;
        line_len = getline(&line, &line_len, stdin);
        // Replace new line with null terminator
        line[line_len - 1] = '\0';

        if (strcmp(line, "q") == 0 || strcmp(line, "Q") == 0)
        {
            printf("goodbye.\n");
            free(line);
            if (keys_dir != NULL)
                free(keys_dir);
            return 0;
        }

        int selection = strtol(line, NULL, 10);
        free(line);
        if (selection <= 0 || selection > NUM_MAIN_MENU_ITEMS)
        {
            printf("Invalid selection\n");
            continue;
        }

        MainMenuOptions menu_selection;
        menu_selection = selection - 1;

        switch (menu_selection)
        {
            case MAIN_MENU_KEYS:
                manage_keys();
                break;
            case MAIN_MENU_ENCRYPT:
                do_encryption();
                break;
            case MAIN_MENU_DECRYPT:
                do_decryption();
                break;
            default:
                printf("Invalid selection\n");
                break;
        }
        printf("\n");
    }

    return 0;
}
