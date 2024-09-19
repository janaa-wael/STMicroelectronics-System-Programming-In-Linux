#ifndef LS_H
#define LS_H

#define MAX_PATH 1024


#define RESET_COLOR "\033[0m"
#define DIR_COLOR "\033[1;34m"
#define EXEC_COLOR "\033[1;32m"
#define SYMLINK_COLOR "\033[1;36m"
#define REG_COLOR "\033[0m" // Regular file color

// Function prototypes
void list_directory(const char *dir, int options);
void print_long_format(const char *path, int show_inode);
void sort_entries(struct dirent **entries, int count, int options);
int compare_time(const struct dirent **a, const struct dirent **b);
void print_table(struct dirent **entries, int count, int options);
void print_entry(const char *dir, const char *filename, int options, int show_inode);
void print_error(const char *msg);
int compare_alpha(const void *a, const void *b);
int filter_hidden(const struct dirent *entry);

#endif
