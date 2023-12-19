#!/usr/bin/sh

rm -rf main *.o core
gcc -g main.c -Wall -Wextra -Werror -pedantic -o main
./main

