

set "cc=C:/Users/029163106/Downloads/env/w64devkit/bin/gcc"
%cc% -o string ./base/main.c ./base/string.c ./base/math.c ./base/memory_win32.c ./base/arena.c -Wall -g -lm
