#!/bin/sh

SRC="./src"
BUILD_PLATFORM="${1:-"linux"}"
BUILD_CORES=()

LAYER_BASE_LINUX="$SRC/base/math.c $SRC/base/string.c $SRC/base/arena.c $SRC/os/memory_linux.c $SRC/base/random.c"
LAYER_OS_LINUX="$SRC/os/shared_linux.c $SRC/os/file_linux.c $SRC/os/dt_linux.c"

LAYER_BASE_WIN32="$SRC/base/math.c $SRC/base/string.c $SRC/base/arena.c $SRC/os/memory_win32.c $SRC/base/random.c"
LAYER_OS_WIN32="$SRC/os/shared_win32.c $SRC/os/file_win32.c $SRC/os/dt_win32.c"

LAYER_GRAPHICS_LINUX="$SRC/graphics/graphics_linux.c"



CC=gcc
INC="-I./src"
CFLAGS="-Wall -g"
LD="-lm"
FLAGS="${CFLAGS} ${INC} ${LD}"
#RFLAGS="-I ./include -L ./lib -lraylib -lopengl32 -lgdi32 -lwinmm"

mkdir -p ./build

if [ "$BUILD_PLATFORM" = "win32" ]; then
    if [[ $BUILD_CORES == *"base"* ]]; then
        $CC $FLAGS -o ./build/base $SRC/base/base.c $LAYER_BASE_WIN32
    fi

    if [[ $BUILD_CORES == *"os"* ]]; then
	$CC $FLAGS -fPIC -shared -o ./build/test.dll $SRC/os/dll_main.c
	$CC $FLAGS -o ./build/os $SRC/os/os.c $LAYER_BASE_WIN32 $LAYER_OS_WIN32
    fi

    $CC $FLAGS -o ./build/piece $SRC/piece.c $LAYER_BASE_WIN32 $LAYER_OS_WIN32
elif [ "$BUILD_PLATFORM" = "linux" ]; then
    if [[ $BUILD_CORES == *"base"* ]]; then
        $CC $FLAGS -o ./build/base $SRC/base/base.c $LAYER_BASE_LINUX
    fi

    if [[ $BUILD_CORES == *"os"* ]]; then
        $CC $FLAGS -fPIC -shared -o ./build/libtest.so $SRC/os/dll_main.c
        $CC $FLAGS -o ./build/os $SRC/os/os.c $LAYER_BASE_LINUX $LAYER_OS_LINUX
    fi

    # $CC $FLAGS -lxcb -o ./build/gp $SRC/graphics/graphics.c $LAYER_BASE_LINUX $LAYER_OS_LINUX $LAYER_GRAPHICS_LINUX
    $CC $FLAGS -o ./build/piece $SRC/piece.c $LAYER_BASE_LINUX $LAYER_OS_LINUX
fi
