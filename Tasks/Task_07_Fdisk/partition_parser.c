#include "partition_parser.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define MBR_SIZE 512
#define GPT_HEADER_SIZE 92
#define GPT_PARTITION_ENTRY_SIZE 128
#define GPT_PARTITION_TABLE_SIZE 128

void parse_mbr_partition_table(int fd, const char *device_name) {
    char buf[MBR_SIZE];

    // Read MBR sector
    read(fd, buf, MBR_SIZE);

    // MBR partition table starts at byte 446
    PartitionEntry *table_entry_ptr = (PartitionEntry *) &buf[446];

    printf("%-12s %-6s %-10s %-10s %-10s %-8s %-4s\n", 
           "Device", "Boot", "Start", "End", "Sectors", "Size (GB)", "Id");

    for (int i = 0; i < 4; i++) {
        printf("%-12s %-6c %-10u %-10u %-10u %-8u %-4X\n",
               device_name,
               table_entry_ptr[i].status == 0x80 ? '*' : ' ',
               table_entry_ptr[i].lba,
               table_entry_ptr[i].lba + table_entry_ptr[i].sector_count - 1,
               table_entry_ptr[i].sector_count,
               (uint32_t)(((uint64_t)table_entry_ptr[i].sector_count * 512) / (1024 * 1024 * 1024)),
               table_entry_ptr[i].partition_type);
    }
}

void parse_gpt_partition_table(int fd, const char *device_name) {
    char gpt_header[GPT_HEADER_SIZE];
    char partition_entries[GPT_PARTITION_TABLE_SIZE * GPT_PARTITION_ENTRY_SIZE];

    // Seek to LBA 1 (sector 1) for GPT header
    lseek(fd, 512, SEEK_SET);
    read(fd, gpt_header, GPT_HEADER_SIZE);

    // Seek to the partition entry array
    lseek(fd, 2 * 512, SEEK_SET);
    read(fd, partition_entries, GPT_PARTITION_TABLE_SIZE * GPT_PARTITION_ENTRY_SIZE);

    GPTPartitionEntry *gpt_entry = (GPTPartitionEntry *)partition_entries;

    printf("%-12s %-10s %-10s %-10s %-10s\n", 
           "Device", "Start (LBA)", "End (LBA)", "Sectors", "Partition Type");

    for (int i = 0; i < GPT_PARTITION_TABLE_SIZE; i++) {
        if (gpt_entry[i].first_lba == 0 && gpt_entry[i].last_lba == 0) {
            continue; // Skip unused entries
        }

        printf("%-12s %-10lu %-10lu %-10lu %-10X\n",
               device_name,
               gpt_entry[i].first_lba,
               gpt_entry[i].last_lba,
               gpt_entry[i].last_lba - gpt_entry[i].first_lba + 1,
               *((uint32_t *)gpt_entry[i].partition_type_guid));
    }
}

