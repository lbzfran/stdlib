#!/bin/sh

gcc -o string ./base/main.c ./base/string.c ./base/memory_linux.c ./base/arena.c -Wall -Wpedantic
