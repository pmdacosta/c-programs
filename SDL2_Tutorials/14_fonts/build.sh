FLAGS='-std=c89 -pedantic -Wall -Wextra'
INCLUDE='-I/usr/include/SDL2'
LIBS='-lSDL2 -lSDL2_ttf -ldl'
SRC='main.c'
OUT='prog'
clang $FLAGS $INCLUDE $LIBS $SRC -o $OUT
