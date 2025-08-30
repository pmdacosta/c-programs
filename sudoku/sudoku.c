#include "sudoku.h"

int** createPuzzle() {
    int** puzzle;
    int init_puzzle[ROWS][COLS] = {
        {0,1,9,0,0,2,0,0,0},
        {4,7,0,6,9,0,0,0,1},
        {0,0,0,4,0,0,0,9,0},
        {8,9,4,5,0,7,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,2,0,1,9,5,8},
        {0,5,0,0,0,6,0,0,0},
        {0,5,0,0,0,6,0,0,0},
        {0,0,0,1,0,0,8,6,0},
    };

    puzzle = malloc(ROWS * sizeof(int*));
    for (int i = 0; i < ROWS; i++) {
        puzzle[i] = malloc(COLS * sizeof(int));
        memcpy(puzzle[i], init_puzzle[i], COLS * sizeof(int));
    }
    return puzzle;
}

void printPuzzle(int** puzzle) {
    for(int i = 0; i < ROWS; i++) {
        for(int j = 0; j < COLS; j++) {
            printf("%d", puzzle[i][j]);
            if ((j+1) % 3 == 0) {
                printf("  ");
            } else if (j != COLS - 1) {
                printf(" ");
            }
        }
        if ((i+1) % 3 == 0 && i != ROWS - 1) {
            printf("\n\n");
        } else {
            printf("\n");
        }
    }
}
