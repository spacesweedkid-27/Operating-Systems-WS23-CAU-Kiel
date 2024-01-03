#!/bin/sh
gcc -o triangle.o triangle.c -c
gcc -o patric.o patric.c -c -pthread -Wall
gcc -o patric triangle.o patric.o -pthread -Wall 
