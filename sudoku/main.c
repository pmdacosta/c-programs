#include "sudoku.h"
#include <libgen.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    int** puzzle;               // 2D array to hold the sudoku puzzle

    // check for command line arguments
    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename\n", basename(argv[0]));
        exit (EXIT_FAILURE);
    }

    puzzle = readPuzzle(argv[1]);
    printPuzzle(puzzle);

    // cleanup
    free(puzzle);
    return 0;
}
