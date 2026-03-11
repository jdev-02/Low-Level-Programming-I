# Low-Level Programming I — CS3040, NPS

C programming exercises covering systems programming fundamentals: memory management, linked lists, file I/O, signals, and POSIX threads.

## Projects

| Directory | Description |
|-----------|-------------|
| `project1/` | **ASCII Total Calculator** — sums ASCII values of a string and multiplies by a user-provided factor. Demonstrates `strtol`, command-line argument validation, and error handling. |
| `project2/` | **Terminal Dot Display** — clears the screen and renders random dots at terminal coordinates using ANSI escape codes and `ioctl(TIOCGWINSZ)` for terminal size detection. |
| `project3/` | **Random Linked List** — builds a linked list of random values, traverses it to compute the sum, and properly frees all nodes. Focuses on `malloc`, pointer traversal, and memory cleanup. |
| `project4/` | **Overflow Detection** — generates a dynamically allocated array of large random values with `calloc`, displays a running total, and flags unsigned 32-bit integer overflow in red. |
| `project5/` | **Yahtzee** — multi-module terminal Yahtzee game with separate play, screen, and score modules. |
| `project6/` | **Caesar Cipher** — encrypts/decrypts files using a byte-level shift key (0-255). Multi-module design with separate `shift.c` library, buffered file I/O via `fread`/`fwrite`. |
| `project7/` | **Multi-Threaded Clock** — displays a large terminal clock using pthreads, mutex synchronization, and POSIX signals (`SIGINT` cycles color, `SIGQUIT` toggles 12/24-hour format). Includes CPU usage stats via `getrusage`. |

## Other Files

| File | Description |
|------|-------------|
| `exam-prep.c` | Exam preparation exercises |
| `practice_test_final.c` | Final exam practice problems |
| `random_code/` | Miscellaneous C exercises (e.g., filtering even numbers from an array) |

## Technologies

C (C99) · POSIX · pthreads · signals · linked lists · dynamic memory · ANSI escape codes

## Background

Naval Postgraduate School · CS3040 Low-Level Programming I · 2024
