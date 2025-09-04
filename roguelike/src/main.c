#include "rogue.h"

int main() {
    Player* player;
    Level** level;
    Room** rooms;
    char **map;
    int key;
    int n_rooms = 3;
    int n_tunnels = 2;

    srand(time(NULL)); // seed the rand function

    rooms = generateRooms(n_rooms);
    // map = readMap();
    map = generateMap(rooms, n_rooms);
    generateTunnels(map, rooms, n_rooms);
    player = createPlayer();

    // init screen
    initscr();
    noecho();   // don't show typed characters

    drawMap(map);
    drawPlayer(player);

    // main loop
    while ((key = getch()) != 'q') {
        handleInput(key, player);
        drawMap(map);
        drawPlayer(player);
    }

    // cleanup
    endwin();
    return 0;
}
