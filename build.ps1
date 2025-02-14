# Create the build directory if it doesn't exist
New-Item -ItemType Directory -Force -Path "./build"

# Define source paths
$SRC = "./src"
$LAYER_BASE = @(
    "$SRC/base/math.c",
    "$SRC/base/string.c",
    "$SRC/base/arena.c",
    "$SRC/os/memory_win32.c",
    "$SRC/base/random.c"
)
$LAYER_OS = @(
    "$SRC/os/shared_win32.c",
    "$SRC/os/file_win32.c",
    "$SRC/os/dt_win32.c"
)

# Define compiler and flags
$CC = "gcc"
$INC = "-I./src"
$CFLAGS = @("-Wall", "-g")
$LD = "-lm"

# Compile base
& $CC $CFLAGS $LD $INC -o ./build/base $SRC/base/base.c $LAYER_BASE

# Compile test DLL
& $CC $CFLAGS $LD $INC -shared -o ./build/test.dll $SRC/os/dll_main.c

# Compile OS
& $CC $CFLAGS $LD $INC -o ./build/os $SRC/os/os.c $LAYER_BASE $LAYER_OS

