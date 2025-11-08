#!/bin/sh

CFLAGS='-std=c99 -Wall -O0 -ggdb3'

clang -D_POSIX_C_SOURCE=200809L $CFLAGS src/sdlamp.c src/physfs/*.c -o sdlamp `sdl2-config --cflags --libs`


# -fsanitize=address
# -fsanitize-address-use-after-scope
# -Wconversion
# -Wall
# -Wextra
# -Werror
# -Wpedantic
# -Wwrite-strings
# -Wshadow
# -Wunused-parameter
# -Wunused-value
# -Wunused-variable
# -Wmissing-prototypes
# -Wmissing-declarations
# -Wfloat-equal
# -Wsign-compare
# -Wundef
# -Wcast-align
# -Wstrict-prototypes
# -Wswitch-default
# -Wold-style-definition
# -Wmissing-include-dirs
# -g
# -Wfatal-errors
