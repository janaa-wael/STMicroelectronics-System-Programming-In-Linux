# Custom `ls` Implementation

This task contains a custom implementation of the `ls` command in C. The implementation supports various options similar to the standard `ls` command found in Unix-like operating systems.

## Features

- **List files and directories**: Display contents of a directory.
- **Support for multiple options**: Handle combinations of options like `-l`, `-a`, `-t`, `-u`, `-c`, and others.
- **Long format**: Show detailed information for each file (permissions, size, timestamps).
- **Sorting**: Sort files by name or modification time.
- **Hidden files**: Option to include or exclude hidden files (those starting with a dot).
- **Color output**: Optional color-coding for different file types.

## Supported Options

The following options are supported:

- `-a`: Show all files, including hidden ones.
- `-l`: Use long listing format.
- `-t`: Sort by modification time (most recent first).
- `-u`: Sort by access time.
- `-c`: Sort by change time.
- `-d`: Treat directories as files.
- `-1`: List one file per line.
- `-i`: Print the inode number of each file.
- `--color`: Enable colorized output.

## Usage

### Compilation

To compile the code, use the provided `Makefile`. Run the following command in the terminal:

```bash
make

