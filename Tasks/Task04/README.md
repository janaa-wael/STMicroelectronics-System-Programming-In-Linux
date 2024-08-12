# Simple Shell

## Overview

This project is a simple Unix-like shell implemented in C. It supports basic shell functionalities, including command execution, output and error redirection, file manipulation, and piping between processes. The shell can handle both internal commands and external commands.

## Features

- **Internal Commands:**
  - `mypwd`: Prints the current working directory.
  - `myecho`: Prints arguments passed to it.
  - `myexit`: Exits the shell.
  - `myhelp`: Displays help information about supported commands.
  - `mymv`: Moves a file from one location to another.
  - `mycp`: Copies a file to another location with options for overwriting or appending.
  - `mycd`: Changes the current working directory.
  - `mytype`: Identifies if a command is internal or external.
  - `myenv`: Prints all environment variables.
  - `free`: Prints memory usage information.
  - `uptime`: Displays system uptime and idle time.

- **Redirection:**
  - Output Redirection (`>`): Redirects standard output to a file.
  - Input Redirection (`<`): Redirects standard input from a file.
  - Error Redirection (`2>`): Redirects standard error to a file.

- **Piping:**
  - Supports piping between processes (e.g., using `grep` to filter output).

## Getting Started

### Prerequisites

- C Compiler (e.g., `gcc`)
- Make (optional, if using a Makefile)

### Compilation

To compile the shell, use the following command:

```sh
gcc -o simple_shell main.c SimpleShell.c
```

### Running the Shell

To run the shell, execute the compiled binary:

```sh
./simple_shell
```

You will be prompted with a custom shell message: `O2mor Ya Ghaly >>`. Enter commands and interact with the shell as needed.

### Example Usage

- **Print Working Directory:**

  ```
  O2mor Ya Ghaly >> mypwd
  ```

- **Echo Arguments:**

  ```
  O2mor Ya Ghaly >> myecho Hello World
  ```

- **Move File:**

  ```
  O2mor Ya Ghaly >> mymv source.txt destination.txt
  ```

- **Copy File with Appending:**

  ```
  O2mor Ya Ghaly >> mycp source.txt destination.txt -a
  ```

- **Change Directory:**

  ```
  O2mor Ya Ghaly >> mycd /path/to/directory
  ```

- **Redirect Output to File:**

  ```
  O2mor Ya Ghaly >> mypwd > output.txt
  ```

- **Pipe Output to `grep`:**

  ```
  O2mor Ya Ghaly >> mycommand | grep search_term
  ```

## Functions

- **`pwd()`**: Prints the current working directory.
- **`echo(char* args[])`**: Prints the provided arguments.
- **`exit_shell()`**: Exits the shell.
- **`help()`**: Displays information about supported commands.
- **`copy(char * sourcepath, char * targetpath, char flag)`**: Copies or appends a file.
- **`move(char* sourcepath, char* targetpath)`**: Moves a file.
- **`cd(char* new_dir)`**: Changes the current directory.
- **`type(char * command)`**: Identifies if a command is internal or external.
- **`print_env_var()`**: Prints all environment variables.
- **`print_memory_info()`**: Prints memory usage information.
- **`print_uptime_info()`**: Prints system uptime and idle time.
- **`output_redirection(char ** args,char arg_count)`**: Handles output redirection.
- **`error_redirection(char *args[], char *command)`**: Handles error redirection.
- **`input_redirection(char *args[], char *command)`**: (Not yet implemented)
- **`pipe_with_grep(const char *cmd1, const char *cmd2, const char *search_term)`**: Pipes output between commands and filters with `grep`.

## Contributing

Feel free to fork the repository, make improvements, and submit pull requests. Ensure that your code adheres to the existing style and includes appropriate comments and documentation.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

Feel free to adjust the README according to your preferences or add any additional information that might be useful for users or contributors.
