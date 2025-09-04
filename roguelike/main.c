#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_HEALTH 20
#define MAP_HEIGTH 40
#define MAP_WIDTH 128
#define DOOR_TOP    0
#define DOOR_RIGHT  1
#define DOOR_BOTTOM 2
#define DOOR_LEFT   3

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

void drawMap(char** map) {
    for (int row = 0; row < MAP_HEIGTH; row++) {
        mvprintw(row, 0,  "%s", map[row]);
    }
    refresh();
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

char** generateMap(Room** rooms, int n_rooms) {
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
            if (room_index == 0 && door_index == DOOR_RIGHT) {
                if (door_row && door_col != -1) {
                    map[door_row][door_col] = '+';
                }
            }
            if (room_index == 1 && door_index == DOOR_LEFT) {
                if (door_row && door_col != -1) {
                    map[door_row][door_col] = '+';
                }
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
    int door_chance = 1; // chance of door being generated

    // we do (width/height - 2) + 1 because we don't want doors in the corners
    // top door
    room->doors[DOOR_TOP].row = row;
    room->doors[DOOR_TOP].col = col + rand() % (width - 2) + 1;

    // right door
    room->doors[DOOR_RIGHT].row = row + rand() % (height - 2) + 1;
    room->doors[DOOR_RIGHT].col = col + width - 1;

    // bottom door
    room->doors[DOOR_BOTTOM].row = row + height - 1;
    room->doors[DOOR_BOTTOM].col = col + rand() % (width - 2) + 1;

    // left door
    room->doors[DOOR_LEFT].row = row + rand() % (height - 2) + 1;
    room->doors[DOOR_LEFT].col = col;

    return room;
}

void connectDoors(char**map, const Position* door1, const Position* door2) {
    Position current;
    Position previous;
    Position backtrack;
    current.row = door1->row;
    current.col = door1->col;
    previous.row = current.row;
    previous.col = current.col;
    int step = 0;

    while ( 1 ) {
        if (map[current.row][current.col - 1] == ' ' && 
                abs((current.col - 1) - door2->col) < abs(current.col - door2->col)) {
            previous.col = current.col;
            current.col = current.col - 1;
        }
        else if (map[current.row][current.col + 1] == ' ' &&
                (abs((current.col + 1) - door2->col) < abs(current.col - door2->col))) {
            previous.col = current.col;
            current.col = current.col + 1;
        }
        else if (map[current.row - 1][current.col] == ' ' &&
                (abs((current.row - 1) - door2->row) < abs(current.row - door2->row))) {
            previous.row = current.row;
            current.row = current.row - 1;
        }
        else if (map[current.row + 1][current.col] == ' ' &&
                (abs((current.row + 1) - door2->row) < abs(current.row - door2->row))) {
            previous.row = current.row;
            current.row = current.row + 1;
        } else {
            if (current.row == door2->row && abs(current.col - door2->col) == 1) {
                return;
            }

            if (current.col == door2->col && abs(current.row - door2->row) == 1) {
                return;
            }

            if (step == 0) {
                current = previous;
                step++;
                continue;
            } else {
                return;
            }
        }
        map[current.row][current.col] = '#';
    }

}

Room** generateRooms(int n_rooms) {
    Room** rooms = malloc(n_rooms * sizeof(Room*));
    if (rooms == NULL) {
        exitError("Malloc failed for rooms");
    }

    rooms[0] = createRoom(1,5,9,12);
    rooms[1] = createRoom(5,40,6,16);

    return rooms;
}

void generateTunnels(char** map, Room** rooms, int n_rooms) {
    connectDoors(map, &rooms[0]->doors[DOOR_RIGHT], &rooms[1]->doors[DOOR_LEFT]);
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
        case '+':
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
    char **map;
    int key;
    int n_rooms = 2;
    int n_tunnels = 1;

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

    // 
    endwin();
    return 0;
}
