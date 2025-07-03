# Custom_C-Shell

A custom shell implementation in C that supports basic shell functionalities, custom commands, I/O redirection, process management, and networking features. This shell is designed to provide a user-friendly interface with additional features like command history, process monitoring, and custom commands.


## Assumptions

- All absolute paths start with `/`, whereas relative paths do not.

- Commands will have at most 20 arguments.

- Erroneous commands will print `Error :<user_prompt> is not a valid command`.

- The operator precedence is: **`;` > `&` > `|`**

---

## Features

### BASIC SYS CALLS

#### INPUT:
- Commands are tokenized and executed based on the precedence of operators (`;` > `&`).

- Erroneous commands are identified and handled gracefully.

#### HOP:

- **Syntax:**
  - `hop arg` : In case of single arguments.
  - `hop arg1 arg2 arg3` : In case of multiple arguments.
  
- Changes the current working directory.

- Supports `~`, `.`, `..`, and `-` for navigation.

- Maintains a `prev_dir` variable to track the previous directory.

- Handles multiple arguments in a single command, printing errors for invalid arguments.

- If no argument is present, then hops into the home directory.

- **Assumptions:**
    - Initially, the `prev_pwd` (previous working directory) is set to `NULL`.

    - If the command is `hop dir1 -`, the `-` is interpreted as the directory that was the working directory before the execution of the entire `hop` command. This ensures that the `-` flag always refers to the directory prior to the current command's execution, maintaining consistency in navigation.

    - When multiple hop parameters are executed in a single user prompt, the shell processes each parameter sequentially. If any parameter is invalid or causes an error, the shell prints a descriptive error message for that specific parameter and continues processing the remaining valid parameters without terminating the command execution. 

#### REVEAL:

- **Syntax:**
  - `reveal <flags> <path/name>`.


- Lists files and directories in the specified path.

- Supports flags:
  - `-a`: Show hidden files.
  - `-l`: Show detailed information (permissions, size, owner, etc.).

  - Following cases are handled in case of flags :

    - reveal -a <path/name>

    - reveal -l <path/name>

    - reveal -a -l <path/name>

    - reveal -l -a <path/name>

    - reveal -la <path/name>

    - reveal -al <path/name>

  - Handled the cases where there are multiple l & a like:-

    - reveal -lala <path/name>

- Assumes a maximum of 1024 entries in a directory.

- Handles errors for invalid paths or flags.

- **Assumptions:**
    - You can assume that the paths/names will not contain any whitespace characters.

    - It is assumed that no path name starts with a `-` symbol.

    - If `reveal -` is executed but there are no previous directories set, the shell will print:  
    `ERROR: previous_directory not set`.

    - If `reveal -l ; reveal -` is performed as the first input command, the `prev_dir` is not updated by the `reveal` command. The `prev_dir` is only updated by the `hop` command. As a result, the same error message `ERROR: previous_directory not set` will be printed for the `reveal -` command. This ensures that the `prev_dir` is managed exclusively by the `hop` command for consistency.


#### LOG:

- **Syntax:**
  - `log`: Displays the command history.
  - `log purge`: Clears the command history.
  - `log execute <index>`: Executes a command from history by index (ordered from most recent to oldest).
  
- Maintains a history of commands in `~/history.txt`.

- Erroneous `log` commands are not stored in history.

- **Assumptions:**
    - The command executed by `log execute` is not stored in the command history to avoid redundancy.

    - Erroneous commands are stored in the history for reference, except for erroneous `log` commands. For example, if an invalid command like `log execuuuute 1` is entered, it will not be stored in the history. 


#### SYS COMMANDS:

- **Syntax:**
  - `<sys_cmd> arg` : In case of single argument.
  - `<sys_cmd> arg1 arg2 arg3` : In case of multiple arguments.

- Supports running system commands in the foreground or background.

- Prints the PID of background processes and their exit status.

- Handles invalid commands gracefully.

- **Assumptions:**
    - Background processes are limited to 4096.

    - In the foreground, if a user prompt contains multiple commands, the shell prints the entire user prompt and then displays the `total time taken` (if it exceeds 2 seconds) in the next display prompt. This approach avoids cluttering the output with individual execution times for each command, focusing instead on the overall execution time.

    - If a user-defined command (e.g., `hop ..`) is executed in the background, the shell prints an error message such as `(hop .. is not a valid command in background)`. Similarly, for erroneous commands (e.g., `slee`), the shell prints `(slee is not a valid command)`. In both cases, a child process is created, and the shell prints its PID followed by `exited normal` to indicate the process's termination.

    - When executing a foreground process like `vim`, Linux sends signal `22` to the child process, causing it to stop. 

#### PROCLORE:

- **Syntax:**
  - `proclore` : prints the information of the shell itself.
  - `proclore <pid>` : prints the information for a given PID.

- Displays process information for a given PID.

- Information includes:
  - Process status.
  - Process group.
  - Virtual memory usage (in KB).
  - Executable path.

- Handles errors for invalid or inaccessible PIDs.


- **Assumptions:**
    - For some processes, if Linux does not grant permission to obtain details, the corresponding `perror` function is used to print descriptive error messages. In such cases:
        - The **process status** is left blank (nothing is printed).
        - The **process group** is displayed as `0`.
        - The **virtual memory usage** is displayed as `1`.
        - The **executable path** is displayed as `Not Accessible`.

    - When attempting to execute a command like `sudo`, Linux may stop the foreground process by sending signal `22`. 

#### SEEK:

- **Syntax:**
  - `seek <flags> <search> <target_directory>`

- Searches for files or directories matching a given name.
- Flags:
  - `-d`: Search for directories only.
  - `-f`: Search for files only.
  - `-e`: Execute the file or change to the directory if a single match is found.
- Handles errors for invalid flags or search names.

- **Assumptions:**
    - When `-` is provided as the search name, the shell will search specifically for folders or files named `-`, as it is possible for directories or files to have `-` as their name. This ensures that the shell treats `-` as a valid search term rather than interpreting it as a flag.
---

### PROCESSES, FILES AND MISC

#### I/O REDIRECTION:

- **Syntax:**
  - `command < file.txt`:  command reads input from file.txt.
  - `command > file.txt`: command writes output to file.txt.
  - `command >> file.txt` : Similar to “>” but appends instead of overwriting if the file.txt already exists.
  - `command < input.txt > output.txt` : command reads input from input.txt and writes output to output.txt.

- Supports input (`<`), output (`>`), and append (`>>`) redirection.

- Handles combined input and output redirection (`<` and `>`) also.

- **Assumptions:**
    - Redirected errors are printed to the specified file.

    - It is assumed that I/O redirection is used only at the end of the command and all its parameters. If a command like `echo i>newfile.txt text` is given as input, the shell will process the redirection up to the point where it encounters the redirection operator (`>` in this case). As a result, `i` will be written to `newfile.txt`, and the remaining parameters (`text`) will be ignored. This ensures consistent and predictable behavior for I/O redirection.

#### PIPES:

- **Syntax:**
  - `command1 | command2`: Passes output of command1 as input to command2
  - `command1 | command2 | command3`: Supports multiple pipes in sequence

- Pipes are used to pass information between commands, taking the output from the command on the left and passing it as standard input to the command on the right.

- The shell supports any number of pipes in a single command line.

- Commands are executed sequentially from left to right when pipes are present.

- Supports both built-in commands (hop, reveal, proclore, etc.) and external system commands in pipe chains.

- **Assumptions:**
  - The shell supports up to **20 pipes** in a single command chain.
  - Background execution with pipes is **not supported**. If attempted, the shell prints: `Background execution with pipes not supported`.
  - Pipes have lower precedence than the `&` operator but higher than the `;` operator.
  - Invalid pipe usage (pipes at the beginning/end of command or consecutive pipes) results in the error: `Error: Invalid use of pipe`.
  - Empty commands between pipes are not allowed and will result in an error.
  - All processes in a pipe chain are executed as foreground processes and the shell waits for all of them to complete.
  - Built-in commands like `hop`, `reveal`, `proclore`, etc., work within pipe chains but their output behavior may differ from standalone execution.

#### REDIRECTION ALONG WITH PIPES:

- **Syntax:**
  - `command < input_file | command2`: Input redirection with first command
  - `command1 | command2 > output_file`: Output redirection with last command  
  - `command1 | command2 >> output_file`: Append redirection with last command
  - `command < input_file | command2 | command3 > output_file`: Combined input and output redirection

- The shell supports combining I/O redirection with pipes, allowing complex command chains with file input/output.

- Input redirection (`<`) can be used with the **first command** in a pipe chain.

- Output redirection (`>` and `>>`) can be used with the **last command** in a pipe chain.

- Multiple inputs and outputs from I/O redirection are **not supported** within pipe chains.

- **Assumptions:**
  - Input redirection (`<`) is **only allowed on the first command** in a pipe chain. If used elsewhere, the behavior is undefined.
  - Output redirection (`>` or `>>`) is **only allowed on the last command** in a pipe chain. If used elsewhere, the behavior is undefined.
  - **Multiple input or output redirections** within a single pipe chain are not supported (e.g., `cmd1 < file1 | cmd2 < file2` is invalid).
  - The redirection operators within pipe chains follow the same syntax and error handling as standalone redirection commands.
  - File permissions and accessibility are checked at runtime, and appropriate error messages are displayed if files cannot be opened.
  - Background execution with pipes and redirection is **not supported**. If attempted, the shell prints: `Background execution with pipes and redirection not supported`.
  - Redirection parsing within pipe chains handles whitespace around operators and filenames appropriately.
  - Error handling for redirection within pipes includes checking for missing filenames after redirection operators.
  - The shell creates necessary output files with permissions `0644` (readable/writable by owner, readable by group and others).

#### ACTIVITIES:

- **Syntax:**
  - `activities`

- Lists all background processes spawned by the shell.

- Displays process status (`Running` or `Stopped`).

- **Assumptions:**
    - The shell only stores the **command name** in the activities list, excluding any flags or arguments associated with the command. 

    - Erroneous commands spawned by the shell are also added to the activities list if they are currently running. This ensures that all processes, including those resulting from invalid commands, are tracked consistently.

    - The activities list is sorted in **lexicographic order** based on the process names, providing an organized and predictable view of background processes.

    - It is assumed that the maximum number of activities (background processes) that can be tracked in a single shell session is **4096**. This limit ensures efficient management of resources.

    - A **status code** of `'T'` is interpreted as the process being **stopped**, while any other status code is assumed to indicate that the process is **running**. This assumption simplifies the handling of process states.

#### SIGNALS:

- **Syntax:**
  - `ping <pid> <signal_number>`: Sends the specified signal to the process with corresponding PID.

- Handles signals:
  - `SIGCHLD`: Cleans up terminated background processes.
  - `Ctrl+D` : Logs out of myshell (after killing all processes) while having no effect on the actual terminal.
  - `SIGINT` (Ctrl+C): Sends `SIGINT` to the foreground process.
  - `SIGTSTP` (Ctrl+Z): Sends `SIGTSTP` to the foreground process.

- Supports sending custom signals to processes using the `ping` command:

- **Assumptions:**
    - When `Ctrl+D` is pressed, the shell gracefully handles the termination of background processes. For each background process that exits, the shell prints its **exit status**, ensuring the user is informed about the completion of those processes.

    - If a foreground process like `sleep x` is currently running when `Ctrl+D` is pressed, the shell waits for the completion of the foreground process (`sleep x` in this case) before logging out. This ensures that the shell does not terminate abruptly and allows the foreground process to complete execution properly.

#### FOREGROUND(FG) AND BACKGROUND(BG):

- **Syntax:**
  - `fg <pid>`: Brings a background process with corresponding PID to the foreground.
  - `bg <pid>`: Resumes a stopped background process with corresponding PID.

- Handles errors for invalid or non-existent PIDs.

- When the fg command is used to bring a background process to the foreground, the shell monitors its execution time. If the process takes more than 2 seconds to execute, the shell prints the message fg pid : time in the display prompt, where pid is the process ID and time is the total execution time of the process. This provides users with clear feedback on the execution duration of foreground processes brought from the background.

#### NEONATE:

- **Syntax:**
  - `neonate -n [time_arg]`

- Monitors the most recently created process.
- Flags:
  - `-n <time_arg>`: Prints the PID of the most recent process at intervals of `time_arg` seconds.
- Exits monitoring when the user presses `x`.

### NETWORKING

#### iMAN:

- **Syntax**
  - `iMan <command_name>`

- Fetches and displays man pages from `man.he.net`.
- Removes HTML tags from the output for better readability.
- Handles errors for invalid or non-existent commands.

---
## OTHER INSTRUCTIONS

### To exit the shell:
- Type `exit`.

### To compile:
```bash
make
```

### To run:
```bash
./myprogram
```

### To clean:
```bash
make clean
```

---

## File Correspondence

- `headers.h`: Includes all header files, standard C headers, and macros.
- `main.c`: Handles input tokenization, global variables, and function calls for different features.
- `prompt_display.h` and `prompt_display.c`: Display the shell prompt.
- `hop.h` and `hop.c`: Implements the `hop` command.
- `reveal.h` and `reveal.c`: Implements the `reveal` command.
- `log.h` and `log.c`: Implements the `log` command.
- `execute_commands.h` and `execute_commands.c`: Handles foreground and background process execution.
- `proclore.h` and `proclore.c`: Implements the `proclore` command.
- `seek.h` and `seek.c`: Implements the `seek` command.
- `redirection.h`, `redirection.c`, and `redirection_tokenise.c`: Handles I/O redirection.
- `activities.h` and `activities.c`: Implements the `activities` command.
- `signals.h` and `signals.c`: Handles signal management.
- `fg_and_bg.h` and `fg_and_bg.c`: Implements `fg` and `bg` commands.
- `neonate.h` and `neonate.c`: Implements the `neonate` command.
- `man.h` and `man.c`: Implements the `iMan` command.
- `pipes.h` and `pipes.c`: Implements pipe functionality and redirection with pipes.

---