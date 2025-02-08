#!/bin/sh


SRC="./src"
LAYER_BASE="$SRC/base/math.c $SRC/base/string.c $SRC/base/arena.c $SRC/os/memory_linux.c $SRC/base/random.c"
LAYER_OS="$SRC/os/shared_linux.c $SRC/os/file_linux.c"

CC=clang
INC="-I./src"
CFLAGS="-Wall -g"
LD="-lm"
FLAGS="${CFLAGS} ${INC} ${LD}"

mkdir -p ./build

$CC $FLAGS -o ./build/base $SRC/base/base.c $LAYER_BASE

$CC $FLAGS -fPIC -shared -o ./build/libtest.so $SRC/os/dll_main.c
$CC $FLAGS -o ./build/os $SRC/os/os.c $LAYER_BASE $LAYER_OS
