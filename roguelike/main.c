#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_HEALTH 20
#define BUF_SIZE 256

typedef struct Player {
    int x;
    int y;
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
   player->x = 7;
   player->y = 4;
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

void drawMap(char** map) {
    for (int row = 0; row < BUF_SIZE; row++) {
        mvprintw(row, 0,  "%s", map[row]);
    }
    refresh();
}

void drawPlayer(Player* player) {
    mvprintw(player->y, player->x, "%c", player->tile);
    move(player->y, player->x);
    refresh();
}

// returns true if a player/npc can move into that position
int canMoveTo(int x, int y) {
    return mvinch(y, x) == '.';
}

// Tries to move player to new postion
void movePlayer(Player* player, int x, int y) {
    int newX = player->x + x;
    int newY = player->y + y;
    if (canMoveTo(newX, newY)) {
        player->x += x;
        player->y += y;
    }
}

void handleInput(int key, Player* player) {
    switch (key) {
        case 'w':
            movePlayer(player, 0, -1);
            break;
        case 'a':
            movePlayer(player, -1, 0);
            break;
        case 's':
            movePlayer(player, 0, 1);
            break;
        case 'd':
            movePlayer(player, 1, 0);
            break;
    }

}

int main() {
    Player* player;
    char **map;
    int key;

    map = readMap();
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
