#include "rogue.h"

int main() {
    Level* level;
    int key;

    srand(time(NULL)); // seed the rand function

    level = generateLevel();

    // init screen
    initscr();
    noecho();   // don't show typed characters

    drawMap(level->map);
    drawMonsters(level->monsters, level->n_monsters);
    drawPlayer(level->player);

    // main loop
    while ((key = getch()) != 'q') {
        handleInput(key, level->player);
        drawMap(level->map);
        drawMonsters(level->monsters, level->n_monsters);
        drawPlayer(level->player);
    }

    // cleanup
    endwin();
    return 0;
}
