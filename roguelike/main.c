#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HEALTH 20
#define MAP_HEIGTH 40
#define MAP_WIDTH 128

typedef struct Tiles {
    char tunnel;
    char room_wall_top;
    char room_wall_side;
    char room_floor;
} Tiles;

Tiles* TILES;

typedef struct Monster {

} Monster;

typedef struct Item {
} Item;

typedef struct Room {
    int row;        // top left corner
    int col;        // top left corner
    int height;
    int width;
    Monster** monsters;
    Item** items;
} Room ;

typedef struct Tunnel {
    int row;
    int col;
    int width;
    int height;
    int tile;
} Tunnel;

typedef struct Player {
    int col;
    int row;
    int health;
    char tile;
} Player;

void exitError(char* error) {
    endwin();
    fprintf(stderr, "ERROR: %s\n", error);
    exit(EXIT_FAILURE);
}

void setupTiles() {
    TILES = malloc(sizeof(Tiles));
    TILES->tunnel = '#';
    TILES->room_floor = '.';
    TILES->room_wall_side = '|';
    TILES->room_wall_top = '-';
}

Player* createPlayer() {
   Player* player = malloc(sizeof(Player));
   player->col = 7;
   player->row = 4;
   player->health = MAX_HEALTH;
   player->tile = '@';
   return player;
}

char** readMap() {
    FILE *fp;
    char buffer[MAP_WIDTH];
    char **map;

    map = malloc(MAP_HEIGTH * sizeof(char *));
    if (map == NULL) {
        exitError("Could not allocate memory for map. Exiting.");
    }

    for (int i = 0; i < MAP_HEIGTH; i++) {
        map[i] = malloc(MAP_WIDTH * sizeof(char));
        if (map[i] == NULL) {
            exitError("Could not allocate memory for map row. Exiting.");
        }
    }

    fp = fopen("map", "r");
    if (fp == NULL) {
        exitError("Could not read map. Exiting.");
    }

    int row = 0;
    while (fgets(map[row], MAP_HEIGTH, fp) && row < MAP_HEIGTH) {
        row++;
    }
    fclose(fp);
    return map;
}

char** generateMap(Room** rooms, int n_rooms, Tunnel** tunnels, int n_tunnels) {
    char **map;

    map = malloc(MAP_HEIGTH * sizeof(char *));
    if (map == NULL) {
        exitError("Could not allocate memory for map. Exiting.");
    }

    for (int i = 0; i < MAP_HEIGTH; i++) {
        map[i] = malloc(MAP_WIDTH * sizeof(char));
        if (map[i] == NULL) {
            exitError("Could not allocate memory for map row. Exiting.");
        }
        memset(map[i], ' ', MAP_WIDTH);
        map[i][MAP_WIDTH - 1] = '\n';
    }

    for (int room_index = 0; room_index < n_rooms; room_index++) {
        Room* room = rooms[room_index];
        // top wall
        for(int col = room->col; col < room->col + room->width; col++) {
            map[room->row][col] = TILES->room_wall_top;
        }
        // bottom wall
        for(int col = room->col; col < room->col + room->width; col++) {
            map[room->row + room->height - 1][col] = TILES->room_wall_top;
        }

        // middle
        for(int row = room->row + 1; row < room->row + room->height - 1; row++) {
            map[row][room->col] = TILES->room_wall_side;
            map[row][room->col + room->width - 1] = TILES->room_wall_side;
            for(int col = room->col+1; col < room->col + room->width - 1; col++) {
                map[row][col] = '.';
            }
        }
    }

    for (int tunnel_index = 0; tunnel_index < n_tunnels; tunnel_index++) {
        Tunnel* tunnel = tunnels[tunnel_index];
        
        if (tunnel->height == 0)  {
            // horizontal tunnel
            for(int col = tunnel->col; col < tunnel->col + tunnel->width; col++) {
                map[tunnel->row][col] = TILES->tunnel;
            }
        } else {
            // vertical tunnel
            for(int row = tunnel->row; row < tunnel->row + tunnel->height; row++) {
                map[row][tunnel->col] = TILES->tunnel;
            }
        }
    }

    return map;
}

Room* createRoom(int row, int col, int height, int width) {
    Room* room = malloc(sizeof(Room));
    if (room == NULL) {
        exitError("Malloc failed for room");
    }
    room->row = row;
    room->col = col;
    room->height = height;
    room->width = width;
    return room;
}

Tunnel* createTunnel(int row, int col, int height, int width) {
    Tunnel* tunnel = malloc(sizeof(Tunnel));
    if (tunnel == NULL) {
        exitError("Malloc failed for room");
    }
    tunnel->row = row;
    tunnel->col = col;
    tunnel->height = height;
    tunnel->width = width;
    return tunnel;
}

Room** generateRooms(int n_rooms) {
    Room** rooms = malloc(n_rooms * sizeof(Room*));
    if (rooms == NULL) {
        exitError("Malloc failed for rooms");
    }

    rooms[0] = createRoom(3,5,9,12);
    rooms[1] = createRoom(5,24,6,16);
    return rooms;
}

Tunnel** generateTunnels(int n_tunnels) {
    Tunnel** tunnels = malloc(n_tunnels * sizeof(Tunnel*));
    if (tunnels == NULL) {
        exitError("Malloc failed for rooms");
    }

    tunnels[0] = createTunnel(7,16,0,9);
    return tunnels;
}

void drawMap(char** map) {
    for (int row = 0; row < MAP_HEIGTH; row++) {
        mvprintw(row, 0,  "%s", map[row]);
    }
    refresh();
}

void drawPlayer(Player* player) {
    mvprintw(player->row, player->col, "%c", player->tile);
    move(player->row, player->col);
    refresh();
}

// returns true if a player/npc can move into that position
int canMoveTo(int row, int col) {
    // mvinch returns the character a y,x coords in the screen
    int tile = mvinch(row, col);
    switch(tile) {
        case '.':
        case '#':
            return TRUE;
    }
    return FALSE;
}

// Tries to move player to new postion
void movePlayer(Player* player, int row, int col) {
    int newCol = player->col + col;
    int newRow = player->row + row;
    if (canMoveTo(newRow, newCol)) {
        player->col += col;
        player->row += row;
    }
}

void handleInput(int key, Player* player) {
    switch (key) {
        case 'w':
            movePlayer(player, -1, 0);
            break;
        case 'a':
            movePlayer(player, 0, -1);
            break;
        case 's':
            movePlayer(player, 1, 0);
            break;
        case 'd':
            movePlayer(player, 0, 1);
            break;
    }

}

int main() {
    Player* player;
    Room** rooms;
    Tunnel** tunnels;
    char **map;
    int key;
    int n_rooms = 2;
    int n_tunnels = 1;

    setupTiles();
    rooms = generateRooms(n_rooms);
    tunnels = generateTunnels(n_tunnels);
    // map = readMap();
    map = generateMap(rooms, n_rooms, tunnels, n_tunnels);
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

    // 
    endwin();
    return 0;
}
