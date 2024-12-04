# Tetris

This is a simple implementation of the popular puzzle game **Tetris** written in C. The game involves manipulating falling tetrominoes to create complete lines on the grid, which will then be cleared. The objective is to score as many points as possible before the screen fills up.

## To Compile Project
Reminder: Only available for Linux
### (32-bit)
Note: I wish it will be supported by all systems
```bash
nasm -f elf32 Scripts/rawchar.s -o build/rawcharlinux.o
gcc -m32 -c Scripts/main.c -o build/main.o
gcc -m32 -no-pie build/* -o program
```
