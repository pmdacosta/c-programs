#!/bin/sh

CFLAGS='-std=c11 
-Wall
-Wextra
-Werror
-Wpedantic
-pedantic-errors
-Wno-unused-function
-Wimplicit-int-conversion
-Wint-conversion
-Wshorten-64-to-32
-Wimplicit-float-conversion
-Wimplicit-int-float-conversion
-Wfloat-conversion
-Wfloat-overflow-conversion
-Wfloat-zero-conversion
-Wbool-conversion
-Wliteral-conversion
-Wimplicit-conversion-floating-point-to-bool
-Wnon-literal-null-conversion
-Wno-incompatible-pointer-types-discards-qualifiers
-Wnull-conversion
-g'
LIBS='-lSDL2 -lSDL2_image -lm'

clang $CFLAGS src/tetris.c -o game $LIBS


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
