sudoku: sudoku.c main.c
	gcc sudoku.c main.c -o sudoku
	ctags -R .

debug: sudoku.c main.c
	gcc sudoku.c main.c -o sudoku -DDEBUG
	ctags -R .

tags: sudoku.c main.c
	ctags -R .

run:
	./sudoku puzzle-input

clean:
	rm sudoku
