#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_HEALTH 20
#define BUF_SIZE 256

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
    char buffer[BUF_SIZE];
    char **map;

    map = malloc(BUF_SIZE * sizeof(char *));
    if (map == NULL) {
        exitError("Could not allocate memory for map. Exiting.");
    }

    for (int i = 0; i < BUF_SIZE; i++) {
        map[i] = malloc(BUF_SIZE * sizeof(char));
        if (map[i] == NULL) {
            exitError("Could not allocate memory for map row. Exiting.");
        }
    }

    fp = fopen("map", "r");
    if (fp == NULL) {
        exitError("Could not read map. Exiting.");
    }

    int row = 0;
    while (fgets(map[row], BUF_SIZE, fp) && row < BUF_SIZE) {
        row++;
    }
    fclose(fp);
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

Room** generateRooms(int n_rooms) {
    Room** rooms = malloc(n_rooms * sizeof(Room*));
    if (rooms == NULL) {
        exitError("Malloc failed for rooms");
    }

    rooms[0] = createRoom(3,5,9,12);
    rooms[1] = createRoom(5,24,6,16);
    return rooms;
}

void drawMap(char** map) {
    for (int row = 0; row < BUF_SIZE; row++) {
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
    return mvinch(row, col) == '.';
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
    char **map;
    int key;
    int n_rooms = 2;

    map = readMap();
    rooms = generateRooms(n_rooms);
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
