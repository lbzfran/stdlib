#!/bin/sh


SRC="./src"
LAYER_BASE="$SRC/base/math.c $SRC/base/string.c $SRC/base/arena.c $SRC/os/memory_linux.c"
LAYER_OS=""

CC=gcc
INC="-I./src"
CFLAGS="-Wall -g"
LD="-lm"
FLAGS="${CFLAGS} ${INC} ${LD}"

mkdir -p ./build

$CC $FLAGS -o ./build/base $SRC/base/base.c $LAYER_BASE
$CC $FLAGS -o ./build/os $SRC/os/os.c $LAYER_BASE $LAYER_OS
