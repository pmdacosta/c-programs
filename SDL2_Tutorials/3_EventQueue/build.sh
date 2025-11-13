FLAGS='-std=c89 -pedantic -Wall'
INCLUDE='-I/usr/include/SDL2'
LIBS='-lSDL2 -ldl'
SRC='main.c'
OUT='prog'
clang $FLAGS $INCLUDE $LIBS $SRC -o $OUT
