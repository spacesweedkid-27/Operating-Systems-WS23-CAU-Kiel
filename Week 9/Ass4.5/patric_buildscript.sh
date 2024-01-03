#!/bin/sh
gcc -o triangle.o triangle.c -c
gcc -o patric.o patric.c -c -pthread -Wall -Werror
gcc -o patric.out triangle.o patric.o -pthread -Wall -Werror
