#include "rogue.h"

int main() {
    Player* player;
    Level* level;
    int key;

    srand(time(NULL)); // seed the rand function

    player = createPlayer();
    level = generateLevel();

    // init screen
    initscr();
    noecho();   // don't show typed characters

    drawMap(level->map);
    drawPlayer(player);

    // main loop
    while ((key = getch()) != 'q') {
        handleInput(key, player);
        drawMap(level->map);
        drawPlayer(player);
    }

    // cleanup
    endwin();
    return 0;
}
