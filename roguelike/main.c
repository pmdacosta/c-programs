#include <ncurses.h>
#include <stdlib.h>

#define MAX_HEALTH 20

typedef struct Player {
    int x;
    int y;
    int health;
    char tile;
} Player;

Player* createPlayer() {
   Player* player = malloc(sizeof(Player));
   player->x = 14;
   player->y = 14;
   player->health = MAX_HEALTH;
   player->tile = '@';
   return player;
}

void mapSetup() {
    mvprintw(13, 13, "--------");
    mvprintw(14, 13, "|......|");
    mvprintw(15, 13, "|......|");
    mvprintw(16, 13, "|......|");
    mvprintw(17, 13, "|......|");
    mvprintw(18, 13, "--------");

    mvprintw(2, 40, "--------");
    mvprintw(3, 40, "|......|");
    mvprintw(4, 40, "|......|");
    mvprintw(5, 40, "|......|");
    mvprintw(6, 40, "|......|");
    mvprintw(7, 40, "--------");

    mvprintw(10, 40, "------------");
    mvprintw(11, 40, "|..........|");
    mvprintw(12, 40, "|..........|");
    mvprintw(13, 40, "|..........|");
    mvprintw(14, 40, "|..........|");
    mvprintw(15, 40, "------------");
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
