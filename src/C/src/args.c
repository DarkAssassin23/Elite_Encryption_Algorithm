#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "args.h"

void print_menu(void)
{
    printf("Select one of the following options: \n");
    for(size_t x = 0; x < num_menu_items; x++)
        printf("%s\n", menu_items[x]);
    printf("(1-%zu) or 'q' to quit: ", num_menu_items);
}

void print_key_gen_menu(void)
{
    printf("Select how large would you like your keys to be: \n");
    for(size_t x = 0; x < num_key_gen_menu_items; x++)
        printf("%s\n", key_gen_menu_items[x]);
    printf("(1-%zu) or 'q' to quit: ", num_key_gen_menu_items);
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