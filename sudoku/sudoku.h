#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#define ROWS 9
#define COLS 9

typedef struct Box {

} Box;

typedef struct Square {
    int number;
    int possible[ROWS];
    int row;
    int col;
    Box* box;

} Square;

int ctoi(char c);
int** readPuzzle(char* filename);
void printPuzzle(int** puzzle);
