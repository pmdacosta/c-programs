#!/bin/sh

CFLAGS='-std=c99 
-Wextra
-Werror
-Wpedantic
-Wwrite-strings
-Wshadow
-Wunused-parameter
-Wunused-value
-Wunused-variable
-Wmissing-prototypes
-Wmissing-declarations
-Wfloat-equal
-Wsign-compare
-Wundef
-Wcast-align
-Wstrict-prototypes
-Wswitch-default
-Wold-style-definition
-Wmissing-include-dirs
-g'
LIBS='-lSDL2 -lSDL2_image'

gcc $CFLAGS src/main.c -o game $LIBS


# -fsanitize=address
# -fsanitize-address-use-after-scope
# -Wconversion
