.SILENT:

# --- Compiler and flags ---
CC      := clang
CFLAGS  := -std=c99 -Wall -O0 -ggdb3 -Wno-unused-but-set-variable -D_POSIX_C_SOURCE=200809L
INCLUDE := -Isrc -Isrc/SDL_sound $(shell sdl2-config --cflags)
LIBS    := $(shell sdl2-config --libs)

# --- Source and build setup ---
SRC := $(wildcard src/*.c src/physfs/*.c src/SDL_sound/*.c src/libmodplug/*.c src/timidity/*.c)
OBJ := $(SRC:.c=.o)
TARGET := sdlamp

# --- Rules ---
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

