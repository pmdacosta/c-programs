CC = gcc
CFLAGS = -Wall -Wextra -I src
LIBS = -lSDL2

SRC = $(wildcard src/*.c)
BIN = yellow-snow

all: $(BIN)

$(BIN): $(SRC) src/*.h
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LIBS)

clean:
	rm $(BIN)

