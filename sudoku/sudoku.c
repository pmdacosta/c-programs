#include "sudoku.h"

#define BUF_SIZE 64

// get integer value of a digit
int ctoi(char c) {
    return c - '0';
}

int** readPuzzle(char *filename) {
    int** puzzle;
    FILE* fp;
    char buffer[BUF_SIZE];

    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Can't open %s\n", filename);
        exit (EXIT_FAILURE);
    }

    // Allocate memory for the puzzle matrix
    puzzle = malloc(ROWS * sizeof(int*));
    for (int i = 0; i < ROWS; i++) {
        puzzle[i] = malloc(COLS * sizeof(int));
    }

    int row = 0;
    while (fgets(buffer, BUF_SIZE, fp)) {
        for(int col = 0; col < COLS; col++) {
            puzzle[row][col] = ctoi(buffer[col]);
        }
        row++;
    }

    return puzzle;
}

void printPuzzle(int** puzzle) {
    printf("+-------+-------+-------+\n");
    for(int i = 0; i < ROWS; i++) {
        printf("| ");
        for(int j = 0; j < COLS; j++) {
            printf("%d", puzzle[i][j]);
            if ((j+1) % 3 == 0) {
                printf(" | ");
            } else if (j != COLS - 1) {
                printf(" ");
            }
        }
        if ((i+1) % 3 == 0 && i != ROWS - 1) {
            printf("\n+-------+-------+-------+\n");
        } else {
            printf("\n");
        }
    }
    printf("+-------+-------+-------+\n");
}
