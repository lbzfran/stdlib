#!/bin/sh

gcc -o ./build/string ./base/main.c ./base/string.c ./base/math.c ./base/memory_linux.c ./base/arena.c -Wall -g -lm
