#include "partition_parser.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <device>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error opening device");
        return 1;
    }

    printf("MBR Partition Table:\n");
    parse_mbr_partition_table(fd, argv[1]);

    printf("\nGPT Partition Table:\n");
    parse_gpt_partition_table(fd, argv[1]);

    close(fd);
    return 0;
}

