# Console File Explorer

A beginner-friendly, console-based file explorer written in C++ (C++17) for Linux.  
Provides common file operations: list, cd, pwd, cp, mv, rm, mkdir, touch, chmod, search, info.

## Features
- `ls`, `cd`, `pwd`
- Recursive `cp` for directories
- `mv`, `rm` (with `-r`), `mkdir`, `touch`
- `chmod` with octal permission
- `search` for filenames by substring (recursive)
- `info` for file metadata
- Simple, interactive prompt

## Requirements
- Linux
- g++ supporting C++17 (e.g., g++ 8 or later)

## Build
```bash
make

