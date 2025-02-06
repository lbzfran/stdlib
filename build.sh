#!/bin/sh

SRC="./src"

gcc -I./src -o ./build/base $SRC/base/base.c $SRC/base/string.c $SRC/base/math.c $SRC/os/memory_linux.c $SRC/base/arena.c -Wall -g -lm
gcc -I./src -o ./build/os $SRC/os/os.c $SRC/base/string.c $SRC/base/math.c $SRC/os/memory_linux.c $SRC/base/arena.c -Wall -g -lm
