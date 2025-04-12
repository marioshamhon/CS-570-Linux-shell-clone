# Linux Shell Clone (C)

## Overview

- A fully functional clone of the Linux shell written entirely in C.
- Implements key features such as command execution, I/O redirection, pipeline commands, and background execution.
- Includes custom built-in functionalities like directory changing and comprehensive error checking.
- Leverages low-level system calls (e.g., `fork()`, `exec()`, `wait()`) to ensure correct and efficient shell operation.

## Key Features

- **Command Execution:**  
  Executes external commands and handles process creation using standard Unix system calls.
- **Input/Output Redirection:**  
  Supports redirection of input and output streams, allowing for complex command chaining.
- **Pipelines:**  
  Implements pipelines to connect multiple commands, passing output from one command as input to the next.
- **Background Execution:**  
  Allows commands to run in the background, enabling multitasking within the shell.
- **Custom Built-in Functions:**  
  Provides functionalities for directory navigation, custom error checking, and other essential built-in commands.
- **System Call Integration:**  
  Uses system calls to manage process creation, control, and synchronization, ensuring robust shell functionality.

## Purpose & Learning

This project was an intensive learning experience that deepened my understanding of:
- Unix system internals and process management in C.
- The intricacies of I/O redirection and pipeline handling.
- Building a complex application from scratch using low-level programming techniques.