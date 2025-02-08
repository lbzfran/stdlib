

set "SRC=./src"
set "LAYER_BASE=%SRC%/base/math.c %SRC%/base/string.c %SRC%/base/arena.c %SRC%/os/memory_win32.c %SRC%/base/random.c"
set "LAYER_OS=%SRC%/os/shared_win32.c %SRC%/os/file_win32.c"

set "CC=gcc"
set "INC=-I./src"
set "CFLAGS=-Wall -g"
set "LD=-lm"
set "FLAGS=%CFLAGS% %INC% %LD%"

%CC% %FLAGS% -o ./build/base %SRC%/base/base.c %LAYER_BASE%

%CC% %FLAGS% -shared -o ./build/test.dll %SRC%/os/dll_main.c
%CC% %FLAGS% -o ./build/os %SRC%/os/os.c %LAYER_BASE% %LAYER_OS%
