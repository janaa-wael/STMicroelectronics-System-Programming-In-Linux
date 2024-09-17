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

#define MAX_PATH 1024

// Function prototypes
void list_directory(const char *dir, int options);
void print_long_format(const char *path, int show_inode);
void sort_entries(struct dirent **entries, int count, int options);
int compare_time(const struct dirent **a, const struct dirent **b);
void print_table(struct dirent **entries, int count, int options);
void print_entry(const char *dir, const char *filename, int options, int show_inode);
void print_error(const char *msg);
int compare_alpha(const void *a, const void *b);
int compare_access_time(const void *a, const void *b, void *dir);
int global_options; // To hold the global options value

int main(int argc, char *argv[]) {
    int options = 0;
    int i = 1;
    
    // Parse options
    while (i < argc && argv[i][0] == '-') {
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
        i++;
    }

    if (i == argc) {
        printf("Usage: %s [options] <directory>...\n", argv[0]);
        exit(1);
    }

    global_options = options;

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

    // Print the file name (or other attributes based on options)
    printf("%s\n", filename);
}

void list_directory(const char *dir, int options) {
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
    if (count == -1) {
        print_error("scandir failed");
        closedir(d);
        return;
    }

    // Sort entries if required
    if (!(options & 64)) { // Not -f option
        if (options & 4) { // -t option
            if (options & 8) { // -u option: sort by access time
                qsort(entries, count, sizeof(struct dirent *), compare_time);
            } else if (options & 16) { // -c option: sort by change time
                // You might want to update compare_time function to handle change time here
                qsort(entries, count, sizeof(struct dirent *), compare_time);
            } else { // Default: sort by modification time
                qsort(entries, count, sizeof(struct dirent *), compare_time);
            }
        } else {
            qsort(entries, count, sizeof(struct dirent *), compare_alpha);
        }
    }

    if (options & 1) { // -l option
        for (int i = 0; i < count; i++) {
            char path[MAX_PATH];
            snprintf(path, sizeof(path), "%s/%s", dir, entries[i]->d_name);
            print_long_format(path, options & 32); // Show inode if -i is set
        }
    } else {
        if (options & 256) { // -1 option
            for (int i = 0; i < count; i++) {
                printf("%s\n", entries[i]->d_name);
            }
        } else {
            print_table(entries, count, options);
        }
    }

    // Free directory entries
    for (int i = 0; i < count; i++) {
        free(entries[i]);
    }
    free(entries);

    closedir(d);
}
