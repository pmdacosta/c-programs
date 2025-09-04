CC = gcc
CFLAGS = -lncurses
SOURCES = $(wildcard src/*.c)
OUT = rogue

rogue: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUT)
	ctags -R .

run:
	./$(OUT)

clean:
	rm $(OUT)
