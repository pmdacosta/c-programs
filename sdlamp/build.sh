#!/bin/sh

CFLAGS='-std=c99 -Wall -O0 -ggdb3 -Wno-unused-but-set-variable'
INCLUDE='-Isrc -Isrc/SDL_sound'
SRC='src/sdlamp.c src/physfs/*.c src/SDL_sound/*.c src/libmodplug/*.c src/timidity/*.c'

clang -D_POSIX_C_SOURCE=200809L $CFLAGS $INCLUDE $SRC -o sdlamp `sdl2-config --cflags --libs`

