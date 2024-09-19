#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>
#include "LS.h"

extern int global_options; // To hold the global options value

int main(int argc, char *argv[]) {
    int options = 0;
    int i = 1;
    
    // Parse options
    while (i < argc && argv[i][0] == '-') {
    if (strcmp(argv[i], "--color") == 0) {
            options |= 512; // Enable color output
    } 
    else 
    {
    // Print normally without color


        for (int j = 1; argv[i][j] != '\0'; j++) {
            switch (argv[i][j]) {
                case 'l': options |= 1; break; // Long format
                case 'a': options |= 2; break; // All files
                case 't': options |= 4; break; // Sort by time
                case 'u': options |= 8; break; // Access time
                case 'c': options |= 16; break; // Change time (inode change)
                case 'i': options |= 32; break; // Inode number
                case 'f': options |= 64; break; // No sort
                case 'd': options |= 128; break; // Directory
                case '1': options |= 256; break; // One per line
                default:
                    fprintf(stderr, "Unknown option: -%c\n", argv[i][j]);
                    exit(1);
            }
        }
     }
        i++;
    }
    
    global_options = options; // Store the parsed options in global_options
    if (i == argc) {
        list_directory(".", options);
    } else {
        for (; i < argc; i++) {
            printf("Directory listing of %s:\n", argv[i]);
            list_directory(argv[i], options);
        }
    }
    
    return 0;
}
