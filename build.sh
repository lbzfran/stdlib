#!/bin/sh

SRC="./src"

BUILD_DEBUG="1"
# add the test cases to build here (separated by layers)
# i.e. BUILD_CORES=("base" "os")
BUILD_CORES=("graphics")

BUILD_PLATFORM="${1:-"linux"}"

LAYER_BASE_LINUX="$SRC/base/string.c $SRC/base/arena.c $SRC/os/memory_linux.c $SRC/base/random.c"

LAYER_OS_LINUX="$SRC/os/shared_linux.c $SRC/os/file_linux.c $SRC/os/dt_linux.c $SRC/os/term_linux.c $SRC/os/shell_linux.c"
LAYER_GRAPHICS_LINUX="$SRC/graphics/graphics_linux.c"

LAYER_BASE_WIN32="$SRC/base/string.c $SRC/base/arena.c $SRC/os/memory_win32.c $SRC/base/random.c"
LAYER_OS_WIN32="$SRC/os/shared_win32.c $SRC/os/file_win32.c $SRC/os/dt_win32.c $SRC/os/term_win32.c $SRC/os/unistd_win32.c"

LAYER_MATH="$SRC/math/vector.c $SRC/math/op.c"

CC=gcc
INC="-I./src"
C11FLAGS="-D_DEFAULT_SOURCE -D_BSD_SOURCE -D_GNU_SOURCE -std=c11"
CFLAGS="-Wall -g -fanalyzer"
LD="-lm"
FLAGS="${CFLAGS} ${C11FLAGS} ${INC} ${LD}"

mkdir -p ./build
[ "${BUILD_DEBUG}" = "1" ] && ctags -R ./src

if [ "$BUILD_PLATFORM" = "win32" ]; then
    if [[ $BUILD_CORES == *"base"* ]]; then
        $CC $FLAGS -o ./build/base $SRC/base/base.c $LAYER_BASE_WIN32 $LAYER_MATH
    fi

    if [[ $BUILD_CORES == *"os"* ]]; then
	$CC $FLAGS -fPIC -shared -o ./build/test.dll $SRC/os/dll_main.c
	$CC $FLAGS -o ./build/os $SRC/os/os.c $LAYER_BASE_WIN32 $LAYER_OS_WIN32 $LAYER_MATH
    fi

    if [[ $BUILD_CORES == *"term"* ]]; then
        $CC $FLAGS -DOS_WINDOWS -o ./build/view $SRC/test.c $LAYER_BASE_WIN32 $LAYER_OS_WIN32 $LAYER_MATH
    fi

    # $CC $FLAGS -o ./build/ed $SRC/editor.c $SRC/gap.c $LAYER_BASE_WIN32 $LAYER_OS_WIN32
elif [ "$BUILD_PLATFORM" = "linux" ]; then
    if [[ $BUILD_CORES == *"base"* ]]; then
        $CC $FLAGS -o ./build/base $SRC/base/base.c $LAYER_BASE_LINUX $LAYER_MATH
    fi

    if [[ $BUILD_CORES == *"os"* ]]; then
        $CC $FLAGS -fPIC -shared -o ./build/libtest.so $SRC/os/dll_main.c
        $CC $FLAGS -o ./build/os $SRC/os/os.c $LAYER_BASE_LINUX $LAYER_OS_LINUX $LAYER_MATH
    fi

    if [[ $BUILD_CORES == *"graphics"* ]]; then
        $CC $FLAGS -fsanitize=address -lX11 -o ./build/gp $SRC/graphics/graphics.c $LAYER_BASE_LINUX $LAYER_OS_LINUX $LAYER_GRAPHICS_LINUX $LAYER_MATH
    fi

    if [[ $BUILD_CORES == *"term"* ]]; then
        $CC $FLAGS -o ./build/view $SRC/test.c $LAYER_BASE_LINUX $LAYER_OS_LINUX $LAYER_MATH
    fi
    # $CC $FLAGS -o ./build/piece $SRC/piece.c $LAYER_BASE_LINUX $LAYER_OS_LINUX $LAYER_MATH
fi
