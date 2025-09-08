TARGETS = close-window
SOURCE = close-window.c
CFLAGS = -Wall -lSDL2 

$(TARGETS): $(SOURCE)
	gcc $< $(CFLAGS) -o $@

run:
	./$(TARGETS)

clean:
	rm $(TARGETS)
