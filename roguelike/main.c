#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_HEALTH 20
#define MAP_HEIGTH 40
#define MAP_WIDTH 128

typedef struct Position {
    int row;
    int col;
} Position;

typedef struct Monster {

} Monster;

typedef struct Item {
} Item;

typedef struct Room {
    Position position;
    Position doors[4];
    int height;
    int width;
    Monster** monsters;
    Item** items;
} Room ;

typedef struct Tunnel {
    Position position;
    int width;
    int height;
    int tile;
} Tunnel;

typedef struct Player {
    Position position;
    int health;
    char tile;
} Player;

void exitError(char* error) {
    endwin();
    fprintf(stderr, "ERROR: %s\n", error);
    exit(EXIT_FAILURE);
}

Player* createPlayer() {
   Player* player = malloc(sizeof(Player));
   player->position.col = 7;
   player->position.row = 4;
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
        int room_row = room->position.row;
        int room_col = room->position.col;
        for (int col = room_col; col < room_col + room->width; col++) {
            // top wall
            map[room_row][col] = '-';
            // bottom wall
            map[room_row + room->height - 1][col] = '-';
        }

        // middle
        for (int row = room_row + 1; row < room_row + room->height - 1; row++) {
            map[row][room_col] = '|';
            map[row][room_col + room->width - 1] = '|';
            for(int col = room_col + 1; col < room_col + room->width - 1; col++) {
                map[row][col] = '.';
            }
        }

        // doors
        for (int door_index = 0; door_index < 4; door_index++) {
            int door_row = room->doors[door_index].row;
            int door_col = room->doors[door_index].col;
            if (door_row && door_col != -1) {
                map[door_row][door_col] = 'D';
            }
        }
    }

    for (int tunnel_index = 0; tunnel_index < n_tunnels; tunnel_index++) {
        Tunnel* tunnel = tunnels[tunnel_index];
        int tunnel_row = tunnel->position.row;
        int tunnel_col = tunnel->position.col;
        
        if (tunnel->height == 0)  {
            // horizontal tunnel
            for (int col = tunnel_col; col < tunnel_col + tunnel->width; col++) {
                map[tunnel_row][col] = '#';
            }
        } else {
            // vertical tunnel
            for (int row = tunnel_row; row < tunnel_row + tunnel->height; row++) {
                map[row][tunnel_col] = '#';
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
    room->position.row = row;
    room->position.col = col;
    room->height = height;
    room->width = width;
    
    // generate doors
    // we do (width/height - 2) + 1 because we don't want doors in the corners
    int door_chance = 5;

    // top door
    if (rand() % door_chance == 0) {
        room->doors[0].row = row;
        room->doors[0].col = col + rand() % (width - 2) + 1;
    } else {
        room->doors[0].row = -1;
        room->doors[0].col = -1;
    }

    // bottom door
    if (rand() % door_chance == 0) {
        room->doors[1].row = row + height - 1;
        room->doors[1].col = col + rand() % (width - 2) + 1;
    } else {
        room->doors[1].row = -1;
        room->doors[1].col = -1;
    }

    // left door
    if (rand() % door_chance == 0) {
        room->doors[2].row = row + rand() % (height - 2) + 1;
        room->doors[2].col = col;
    } else {
        room->doors[2].row = -1;
        room->doors[2].col = -1;
    }

    // right door
    if (rand() % door_chance == 0) {
        room->doors[3].row = row + rand() % (height - 2) + 1;
        room->doors[3].col = col + width - 1;
    } else {
        room->doors[3].row = -1;
        room->doors[3].col = -1;
    }

    return room;
}

Tunnel* createTunnel(int row, int col, int height, int width) {
    Tunnel* tunnel = malloc(sizeof(Tunnel));
    if (tunnel == NULL) {
        exitError("Malloc failed for room");
    }
    tunnel->position.row = row;
    tunnel->position.col = col;
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
    mvprintw(player->position.row, player->position.col, "%c", player->tile);
    move(player->position.row, player->position.col); // move the cursor to match the player position
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
    int newCol = player->position.col + col;
    int newRow = player->position.row + row;
    if (canMoveTo(newRow, newCol)) {
        player->position.col += col;
        player->position.row += row;
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

    srand(time(NULL)); // seed the rand function
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
