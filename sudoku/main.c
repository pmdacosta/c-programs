#include "sudoku.h"

int main() {
    int** puzzle;
#ifdef DEBUG
    printf("{DEBUG}: ENTERING createPuzzle()\n");
#endif
    puzzle = createPuzzle();
#ifdef DEBUG
    printf("{DEBUG}: ENTERING printPuzzle()\n");
#endif
    printPuzzle(puzzle);
    return 0;
}
