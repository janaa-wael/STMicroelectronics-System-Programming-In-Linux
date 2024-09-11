#ifndef PARTITION_PARSER_H
#define PARTITION_PARSER_H

#include <stdint.h>

// Structure to represent an MBR partition entry
typedef struct {
    uint8_t status;
    uint8_t first_chs[3];
    uint8_t partition_type;
    uint8_t last_chs[3];
    uint32_t lba;
    uint32_t sector_count;
} PartitionEntry;

// Structure to represent a GPT partition entry
typedef struct {
    uint8_t partition_type_guid[16];
    uint8_t unique_partition_guid[16];
    uint64_t first_lba;
    uint64_t last_lba;
    uint64_t attributes;
    uint16_t partition_name[36];
} GPTPartitionEntry;

// Function prototypes
void parse_mbr_partition_table(int fd, const char *device_name);
void parse_gpt_partition_table(int fd, const char *device_name);

#endif // PARTITION_PARSER_H

