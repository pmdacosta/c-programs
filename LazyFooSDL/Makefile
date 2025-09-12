CC = gcc
CFLAGS = -std=c11 -O0 -ggdb3 -Wall -Wextra -Werror -Wpedantic -Wwrite-strings -Wconversion \
				  -Wshadow -Wmissing-prototypes -Wmissing-declarations -Wfloat-equal \
				  -Wsign-compare -Wundef -Wcast-align -Wstrict-prototypes -Wswitch-default \
				  -Wold-style-definition -Wmissing-include-dirs \
				  -fsanitize=address -fsanitize-address-use-after-scope -I src
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

SRC = $(wildcard src/*.c)
BIN = game

all: $(BIN)

$(BIN): $(SRC) src/*.h
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LIBS)

run:
	./$(BIN)

clean:
	rm $(BIN)

