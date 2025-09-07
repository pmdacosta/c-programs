#ifndef ROGUE_H
#define ROGUE_H

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
    char tile;
    int health;
    int attack;
    Position position;
} Monster;

typedef struct Item {
} Item;

typedef struct Room {
    Position position;
    Position doors[4];
    int height;
    int width;
    Item** items;
} Room ;

typedef struct Player {
    Position position;
    int health;
    char tile;
} Player;

typedef struct Level {
    char**map;
    Room** rooms;
    Monster** monsters;
    Player* player;
    int n_rooms;
    int n_tunnels;
    int n_monsters;
} Level;

void exitError(char* error);
void drawMap(char** map);
char** readMap();
Level* generateLevel();
char** generateMap(Room** rooms, int n_rooms);
Room** generateRooms(int n_rooms);
void generateTunnels(char** map, Room** rooms, int n_rooms);
void generateMonsters(Level* level);
Room* createRoom(int row, int col, int height, int width);
void connectDoors(char**map, const Position* door1, const Position* door2);
void drawPlayer(Player* player);
void drawMonsters(Monster** monsters, int n_monsters);

/* returns true if a player/npc can move into that position */
int canMoveTo(int row, int col);

/* Tries to move player to new postion */
void movePlayer(Player* player, int row, int col);
void handleInput(int key, Player* player);

#endif
