# Custom_C-Shell

A custom shell implementation in C that supports basic shell functionalities, custom commands, I/O redirection, process management, and networking features. This shell is designed to provide a user-friendly interface with additional features like command history, process monitoring, and custom commands.


## Assumptions

- All absolute paths start with `/`, whereas relative paths do not.

- Commands will have at most 20 arguments.

- Erroneous commands will print `Error :<user_prompt> is not a valid command`.

- Precedence order for operators: `;` > `&`.

## Features

### BASIC SYS CALLS

#### INPUT:
- Commands are tokenized and executed based on the precedence of operators (`;` > `&`).

- Erroneous commands are identified and handled gracefully.

#### HOP:
- Changes the current working directory.

- Supports `~`, `.`, `..`, and `-` for navigation.

- Maintains a `prev_dir` variable to track the previous directory.

- Handles multiple arguments in a single command, printing errors for invalid arguments.

- **Assumptions:**
    - Initially, the `prev_pwd` (previous working directory) is set to `NULL`.

    - If the command is `hop dir1 -`, the `-` is interpreted as the directory that was the working directory before the execution of the entire `hop` command. This ensures that the `-` flag always refers to the directory prior to the current command's execution, maintaining consistency in navigation.

    - When multiple hop parameters are executed in a single user prompt, the shell processes each parameter sequentially. If any parameter is invalid or causes an error, the shell prints a descriptive error message for that specific parameter and continues processing the remaining valid parameters without terminating the command execution. 

#### REVEAL:
- Lists files and directories in the specified path.

- Supports flags:
  - `-a`: Show hidden files.
  - `-l`: Show detailed information (permissions, size, owner, etc.).

- Assumes a maximum of 1024 entries in a directory.

- Handles errors for invalid paths or flags.

- **Assumptions:**
- It is assumed that no path name starts with a `-` symbol.

- If `reveal -` is executed but there are no previous directories set, the shell will print:  
  `ERROR: previous_directory not set`.

- If `reveal -l ; reveal -` is performed as the first input command, the `prev_dir` is not updated by the `reveal` command. The `prev_dir` is only updated by the `hop` command. As a result, the same error message `ERROR: previous_directory not set` will be printed for the `reveal -` command. This ensures that the `prev_dir` is managed exclusively by the `hop` command for consistency.


#### LOG:
- Maintains a history of commands in `~/history.txt`.

- Supports:
  - `log`: Displays the command history.
  - `log purge`: Clears the command history.
  - `log execute <index>`: Executes a command from history by index.

- Erroneous `log` commands are not stored in history.

- **Assumptions:**
    - The command executed by `log execute` is not stored in the command history to avoid redundancy.

    - Erroneous commands are stored in the history for reference, except for erroneous `log` commands. For example, if an invalid command like `log execuuuute 1` is entered, it will not be stored in the history. 


#### SYS COMMANDS:
- Supports running system commands in the foreground or background.

- Prints the PID of background processes and their exit status.

- Handles invalid commands gracefully.

- **Assumptions:**
    - Background processes are limited to 4096.

    - In the foreground, if a user prompt contains multiple commands, the shell prints the entire user prompt and then displays the `total time taken` (if it exceeds 2 seconds) in the next display prompt. This approach avoids cluttering the output with individual execution times for each command, focusing instead on the overall execution time.

    - If a user-defined command (e.g., `hop ..`) is executed in the background, the shell prints an error message such as `(hop .. is not a valid command in background)`. Similarly, for erroneous commands (e.g., `slee`), the shell prints `(slee is not a valid command)`. In both cases, a child process is created, and the shell prints its PID followed by `exited normal` to indicate the process's termination.

    - When executing a foreground process like `vim`, Linux sends signal `22` to the child process, causing it to stop. 

#### PROCLORE:
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
- Supports input (`<`), output (`>`), and append (`>>`) redirection.

- Handles combined input and output redirection (`<` and `>`).



- **Assumptions:**
    - Redirected errors are printed to the specified file.

    - It is assumed that I/O redirection is used only at the end of the command and all its parameters. If a command like `echo i>newfile.txt text` is given as input, the shell will process the redirection up to the point where it encounters the redirection operator (`>` in this case). As a result, `i` will be written to `newfile.txt`, and the remaining parameters (`text`) will be ignored. This ensures consistent and predictable behavior for I/O redirection.



#### ACTIVITIES:
- Lists all background processes spawned by the shell.

- Displays process status (`Running` or `Stopped`).

- **Assumptions:**
    - The shell only stores the **command name** in the activities list, excluding any flags or arguments associated with the command. 

    - Erroneous commands spawned by the shell are also added to the activities list if they are currently running. This ensures that all processes, including those resulting from invalid commands, are tracked consistently.

    - The activities list is sorted in **lexicographic order** based on the process names, providing an organized and predictable view of background processes.

    - It is assumed that the maximum number of activities (background processes) that can be tracked in a single shell session is **4096**. This limit ensures efficient management of resources.

    - A **status code** of `'T'` is interpreted as the process being **stopped**, while any other status code is assumed to indicate that the process is **running**. This assumption simplifies the handling of process states.

#### SIGNALS:
- Handles signals:
  - `SIGCHLD`: Cleans up terminated background processes.
  - `SIGINT` (Ctrl+C): Sends `SIGINT` to the foreground process.
  - `SIGTSTP` (Ctrl+Z): Sends `SIGTSTP` to the foreground process.

- Supports sending custom signals to processes using the `ping` command:
  - `ping <pid> <signal_number>`: Sends the specified signal to the process.

- **Assumptions:**
    - When `Ctrl+D` is pressed, the shell gracefully handles the termination of background processes. For each background process that exits, the shell prints its **exit status**, ensuring the user is informed about the completion of those processes.

    - If a foreground process like `sleep x` is currently running when `Ctrl+D` is pressed, the shell waits for the completion of the foreground process (`sleep x` in this case) before logging out. This ensures that the shell does not terminate abruptly and allows the foreground process to complete execution properly.

#### FOREGROUND(FG) AND BACKGROUND(BG):
- `fg <pid>`: Brings a background process to the foreground.

- `bg <pid>`: Resumes a stopped background process.

- Handles errors for invalid or non-existent PIDs.

- When the fg command is used to bring a background process to the foreground, the shell monitors its execution time. If the process takes more than 2 seconds to execute, the shell prints the message fg pid : time in the display prompt, where pid is the process ID and time is the total execution time of the process. This provides users with clear feedback on the execution duration of foreground processes brought from the background.

#### NEONATE:
- Monitors the most recently created process.
- Flags:
  - `-n <time_arg>`: Prints the PID of the most recent process at intervals of `time_arg` seconds.
- Exits monitoring when the user presses `x`.

### NETWORKING

#### iMAN:
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
- `1.h` and `1.c`: Display the shell prompt.
- `3.h` and `3.c`: Implements the `hop` command.
- `4.h` and `4.c`: Implements the `reveal` command.
- `5.h` and `5.c`: Implements the `log` command.
- `6.h` and `6.c`: Handles foreground and background process execution.
- `7.h` and `7.c`: Implements the `proclore` command.
- `8.h` and `8.c`: Implements the `seek` command.
- `10_redirection.h`, `10_redirection.c`, and `10_redirection_tokenise.c`: Handles I/O redirection.
- `13_activities.h` and `13_activities.c`: Implements the `activities` command.
- `14_signals.h` and `14_signals.c`: Handles signal management.
- `15_fgandbg.h` and `15_fgandbg.c`: Implements `fg` and `bg` commands.
- `16_neonate.h` and `16_neonate.c`: Implements the `neonate` command.
- `17_man.h` and `17_man.c`: Implements the `iMan` command.

---