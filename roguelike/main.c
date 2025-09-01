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

void mapSetup() {
    FILE *fp;
    char buffer[BUF_SIZE];
    fp = fopen("map", "r");
    if (fp == NULL) {
        exitError("Could not read map. Exiting.");
    }

    int row = 0;
    while (fgets(buffer, BUF_SIZE, fp)) {
        mvprintw(row, 0, "%s", buffer);
        row++;
    }
    fclose(fp);
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
    int key;

    // init screen
    initscr();
    noecho();   // don't show typed characters

    mapSetup();
    player = createPlayer();
    drawPlayer(player);

    // main loop
    while ((key = getch()) != 'q') {
        handleInput(key, player);
        mapSetup();
        drawPlayer(player);
    }

    // 
    endwin();
    return 0;
}
