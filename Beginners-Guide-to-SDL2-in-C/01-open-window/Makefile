TARGETS = open-window
SOURCE = open-window.c
CFLAGS = -Wall -lSDL2 

$(TARGETS): $(SOURCE)
	gcc $< $(CFLAGS) -o $@

run:
	./$(TARGETS)

clean:
	rm $(TARGETS)
