FLAGS='-Wall'
INCLUDE='-Iglad/include -I/usr/include/SDL2'
LIBS='-lSDL2 -ldl'
SRC='glad/src/glad.c main.c'
OUT='main'
clang $FLAGS $INCLUDE $LIBS $SRC -o $OUT
