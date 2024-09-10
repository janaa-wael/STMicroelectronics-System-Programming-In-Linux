# Partition Table Parser

This project provides a tool for reading and parsing both MBR (Master Boot Record) and GPT (GUID Partition Table) partition tables from a specified storage device.

## Project Files

* **`partition_parser.h`**: Header file containing structure definitions, constants, and function prototypes for MBR and GPT partition table parsing.
* **`partition_parser.c`**: Source file implementing the functions to read and parse MBR and GPT partition tables.
* **`main.c`**: Main file that includes the program's entry point and invokes the functions from `partition_parser.c` to display the partition table information.

## Compilation

To compile the project, use the following `gcc` command:

```bash
gcc -g -o partition_parser main.c partition_parser.c

