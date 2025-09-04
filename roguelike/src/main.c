#include "rogue.h"

int main() {
    Player* player;
    Room** rooms;
    char **map;
    int key;
    int n_rooms = 3;
    int n_tunnels = 2;

    srand(time(NULL)); // seed the rand function

    // init screen
    initscr();
    noecho();   // don't show typed characters

    rooms = generateRooms(n_rooms);
    // map = readMap();
    map = generateMap(rooms, n_rooms);
    generateTunnels(map, rooms, n_rooms);
    player = createPlayer();

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
