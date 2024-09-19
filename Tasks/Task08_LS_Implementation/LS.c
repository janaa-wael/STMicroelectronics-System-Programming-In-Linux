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


int global_options; // To hold the global options value


int filter_hidden(const struct dirent *entry) {
    // Exclude hidden files (those starting with a dot) unless -a option is set
    if (!(global_options & 2) && entry->d_name[0] == '.') {
        return 0; // Exclude this entry
    }
    return 1; // Include this entry
}

void print_long_format(const char *path, int show_inode) {
    struct stat st;
    if (stat(path, &st) == -1) {
        perror("stat failed");
        return;
    }

    if (show_inode) {
        printf("%lu ", (unsigned long)st.st_ino);
    }

    printf("%c", S_ISDIR(st.st_mode) ? 'd' : '-');
    printf("%c", (st.st_mode & S_IRUSR) ? 'r' : '-');
    printf("%c", (st.st_mode & S_IWUSR) ? 'w' : '-');
    printf("%c", (st.st_mode & S_IXUSR) ? 'x' : '-');
    printf("%c", (st.st_mode & S_IRGRP) ? 'r' : '-');
    printf("%c", (st.st_mode & S_IWGRP) ? 'w' : '-');
    printf("%c", (st.st_mode & S_IXGRP) ? 'x' : '-');
    printf("%c", (st.st_mode & S_IROTH) ? 'r' : '-');
    printf("%c", (st.st_mode & S_IWOTH) ? 'w' : '-');
    printf("%c", (st.st_mode & S_IXOTH) ? 'x' : '-');
    printf(" %ld ", (long)st.st_nlink);

    struct passwd *pw = getpwuid(st.st_uid);
    struct group *gr = getgrgid(st.st_gid);
    printf("%s %s ", pw ? pw->pw_name : "unknown", gr ? gr->gr_name : "unknown");

    printf("%ld ", (long)st.st_size);
    char time_str[13];
    strftime(time_str, sizeof(time_str), "%b %d %H:%M", localtime(&st.st_mtime));
    printf("%s ", time_str);

    printf("%s\n", path);
}

int compare_time(const struct dirent **a, const struct dirent **b) {
    struct stat stat1, stat2;
    char path1[MAX_PATH], path2[MAX_PATH];

    snprintf(path1, sizeof(path1), "%s/%s", ".", (*a)->d_name);
    snprintf(path2, sizeof(path2), "%s/%s", ".", (*b)->d_name);

    if (stat(path1, &stat1) == -1 || stat(path2, &stat2) == -1) {
        perror("stat failed");
        exit(1);
    }

    struct timespec time1, time2;

    if (global_options & 8) { // -u option: Access time
        time1 = stat1.st_atim;
        time2 = stat2.st_atim;
    } else if (global_options & 16) { // -c option: Change time
        time1 = stat1.st_ctim;
        time2 = stat2.st_ctim;
    } else { // Default: Modification time
        time1 = stat1.st_mtim;
        time2 = stat2.st_mtim;
    }

    if (time1.tv_sec > time2.tv_sec) {
        return -1;
    } else if (time1.tv_sec < time2.tv_sec) {
        return 1;
    }

    if (time1.tv_nsec > time2.tv_nsec) {
        return -1;
    } else if (time1.tv_nsec < time2.tv_nsec) {
        return 1;
    }

    return 0;
}

int compare_alpha(const void *a, const void *b) {
    const struct dirent *const *da = a;
    const struct dirent *const *db = b;
    return strcmp((*da)->d_name, (*db)->d_name);
}

void print_table(struct dirent **entries, int count, int options) {
    int width = 0;
    for (int i = 0; i < count; i++) {
        int len = strlen(entries[i]->d_name);
        if (len > width) {
            width = len;
        }
    }

    for (int i = 0; i < count; i++) {
        if (options & 256) { // -1 option
            printf("%s\n", entries[i]->d_name);
        } else {
            // Print inode number if -i option is set
            if (options & 32) {
                struct stat st;
                char path[MAX_PATH];
                snprintf(path, sizeof(path), "%s/%s", ".", entries[i]->d_name);
                if (stat(path, &st) == -1) {
                    perror("stat failed");
                    continue;
                }
                printf("%lu ", (unsigned long)st.st_ino);
            }

            printf("%-*s", width + 2, entries[i]->d_name);
            if ((i + 1) % 4 == 0 || i == count - 1) {
                printf("\n");
            }
        }
    }

    if (!(options & 256)) {
        printf("\n");
    }
}

void print_error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void print_entry(const char *dir, const char *filename, int options, int show_inode) {
    struct stat file_stat;
    char full_path[MAX_PATH];

    // Construct the full path for the stat call
    snprintf(full_path, sizeof(full_path), "%s/%s", dir, filename);

    // Retrieve file information
    if (stat(full_path, &file_stat) == -1) {
        perror("stat failed");
        return;
    }

    // Print the inode number if -i option is set
    if (show_inode) {
        printf("%ld ", (long)file_stat.st_ino);
    }

    // Determine color based on file type
    const char *color = REG_COLOR; // Default color for regular files
    if (S_ISDIR(file_stat.st_mode)) {
        color = DIR_COLOR; // Directory color
    } else if (S_ISLNK(file_stat.st_mode)) {
        color = SYMLINK_COLOR; // Symlink color
    } else if (file_stat.st_mode & S_IXUSR) {
        color = EXEC_COLOR; // Executable color
    }
    
    
    // Print the file name with color
    printf("%s%s%s\n", color, filename, RESET_COLOR);
}


void sort_entries(struct dirent **entries, int count, int options) {
    if (options & 64) { // -f option: No sorting
        return;
    }

    if (options & 4) { // -t option: Sort by time
        qsort(entries, count, sizeof(struct dirent *), compare_time);
    } else { // Default: Sort alphabetically
        qsort(entries, count, sizeof(struct dirent *), compare_alpha);
    }
}

void list_directory(const char *dir, int options) {

    if (options & 128) { // -d option: Treat directory as a file
        if (options & 1) { // -l option
            print_long_format(dir, options & 32); // Show inode if -i is set
        } else {
            // Print directory name
            if (options & 256) { // -1 option
                printf("%s\n", dir);
            } else {
                printf("%s  ", dir);
            }
        }
        printf("\n");
        return;
    }
    
    DIR *d;
    struct dirent **entries;
    int count;

    d = opendir(dir);
    if (d == NULL) {
        print_error("opendir failed");
        return;
    }

    // Read directory entries
    count = scandir(dir, &entries, filter_hidden, NULL);
    if (count < 0) {
        print_error("scandir failed");
    }

    // Sort directory entries by time (-t) if -f option is not set
    if (!(options & 64)) {
        if (options & 4 || options & 8 || options & 16) {
            qsort(entries, count, sizeof(struct dirent *), compare_time);
        } else {
            qsort(entries, count, sizeof(struct dirent *), compare_alpha);
        }
    }
    else
    {
    	 qsort(entries, count, sizeof(struct dirent *), compare_alpha);
    	//count = scandir(dir, &entries, NULL, NULL); // No filter, no sort
    }


    //******
    // Print entries
    if (options & 1) { // -l option
        for (int i = 0; i < count; i++) {
            char full_path[MAX_PATH];
            snprintf(full_path, sizeof(full_path), "%s/%s", dir, entries[i]->d_name);
            print_long_format(full_path, options & 32); // Show inode if -i is set
        }
    } else {
    	if (options & 512) { // --color option
            for (int i = 0; i < count; i++) {
                print_entry(dir, entries[i]->d_name, options, options & 32);
            }
        }
        else {
            print_table(entries, count, options);
        }
    }

    // Free memory
    for (int i = 0; i < count; i++) {
        free(entries[i]);
    }
    free(entries);

    closedir(d);
}

