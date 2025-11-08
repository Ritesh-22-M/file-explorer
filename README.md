# 🧭 Console File Explorer

A beginner-friendly, console-based **File Explorer** written in **C++17** for Linux.  
This project allows users to navigate and manage files and directories directly from the terminal — similar to common Linux shell commands.

---

## 🚀 Features
- `ls`, `cd`, `pwd` — Basic directory navigation  
- `mkdir`, `touch` — Create folders and files  
- `cp`, `mv`, `rm (-r)` — Copy, move, and delete (with recursion)  
- `chmod` — Change permissions using octal input (e.g., 755)  
- `search` — Find files/folders recursively by name  
- `info` — Display file size, type, and permissions  
- `clear`, `help`, `exit` — Utility commands  
- Simple, interactive command prompt:  

---

## 🛠️ Requirements
- Linux environment (or WSL on Windows)
- **g++** supporting C++17 or higher (e.g., g++ 8+)
- Basic command-line knowledge

---

## ⚙️ Build Instructions

Clone or download this repository, then open a terminal in the project root folder:

```bash
make
```

This will compile and create an executable file named: file-explorer

To clean build artifacts:

```bash
make clean
```

▶️ Run the Application
Run the explorer from your terminal:

```bash
./file-explorer
```

You’ll see:

```bash
Console File Explorer (C++ / Linux)
Type 'help' for commands.
[fe] /home/ritesh $
```
💡 Example Usage
```bash
[fe] /home/ritesh $ ls
[fe] /home/ritesh $ mkdir test
[fe] /home/ritesh $ cd test
[fe] /home/ritesh/test $ touch hello.txt
[fe] /home/ritesh/test $ ls
[fe] /home/ritesh/test $ info hello.txt
[fe] /home/ritesh/test $ cd ..
[fe] /home/ritesh $ rm test -r
[fe] /home/ritesh $ exit
```

🧱 Project Structure :
```
src/
  main.cpp
  explorer.hpp
  explorer.cpp
Makefile
.gitignore
README.md
LICENSE
📄 License
This project is licensed under the MIT License — see the LICENSE file for details.
```

```
👨‍💻 Author
Ritesh Mohanty
Capstone Project - Linux System Programming
https://github.com/Ritesh-22-M/file-explorer
```
