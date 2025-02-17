

set "SRC=./src"
set "LAYER_BASE=%SRC%/base/math.c %SRC%/base/string.c %SRC%/base/arena.c %SRC%/os/memory_win32.c %SRC%/base/random.c"
set "LAYER_OS=%SRC%/os/shared_win32.c %SRC%/os/file_win32.c %SRC%/os/dt_win32.c"

set "CC=gcc"
set "INC=-I ./src"
set "CFLAGS=-Wall -g -std=c11"
set "LD=-lm"
set "FLAGS=%CFLAGS% %INC% %LD%"
set "RLIB=-L ./lib/ -lraylib -lopengl32 -lgdi32 -lwinmm -I ../rayfiles"

:: %CC% %FLAGS% -o ./build/base %SRC%/base/base.c %LAYER_BASE%

:: %CC% %FLAGS% -shared -o ./build/test.dll %SRC%/os/dll_main.c
:: %CC% %FLAGS% -o ./build/os %SRC%/os/os.c %LAYER_BASE% %LAYER_OS%

%CC% %FLAGS% %RLIB% -o ./build/weiss %SRC%/editor/weiss.c %LAYER_BASE% %LAYER_OS%
