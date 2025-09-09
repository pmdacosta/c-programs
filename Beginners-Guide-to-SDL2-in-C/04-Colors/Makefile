TARGETS = game
SOURCE = main.c
CFLAGS = -Wall -lSDL2 -lSDL2_image

$(TARGETS): $(SOURCE)
	gcc $< $(CFLAGS) -o $@

run:
	./$(TARGETS)

clean:
	rm $(TARGETS)
