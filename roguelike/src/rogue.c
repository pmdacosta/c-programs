#include "rogue.h"
#include <stdlib.h>

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
        if (room == NULL) {
            continue;               // sanity check
                                    // there was an error creating room
                                    // so it's null
        }

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
            map[door_row][door_col] = '+';
            map[door_row][door_col] = '+';
        }
    }

    return map;
}

Room* createRoom(int row, int col, int height, int width) {
    // sanity check
    // checking in the room is inside the map limits
    if (row < 0 || row >= MAP_HEIGTH) return NULL;
    if (col < 0 || col >= MAP_WIDTH) return NULL;
    if (row + height - 1 < 0 || row + height - 1 >= MAP_HEIGTH) return NULL;
    if (col + width - 1 < 0 || col + width - 1 >= MAP_WIDTH) return NULL;

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
    rooms[2] = createRoom(18,30,12,12);

    return rooms;
}

void generateTunnels(char** map, Room** rooms, int n_rooms) {
    connectDoors(map, &rooms[0]->doors[DOOR_RIGHT], &rooms[1]->doors[DOOR_LEFT]);
    if (rooms[2] != NULL) {
        connectDoors(map, &rooms[1]->doors[DOOR_BOTTOM], &rooms[2]->doors[DOOR_TOP]);
    }
}

void generateMonsters(Level* level) {
    level->monsters = malloc(level->n_monsters * sizeof(Monster*));
    if (level->monsters == NULL) {
        exitError("Could not allocate memory for monsters. Exiting");
    }
    
    int monster_index = 0;
    for (int room_i = 0; room_i < level->n_rooms; room_i++) { 
        Monster* monster = malloc(sizeof(Monster));
        if (monster == NULL) {
            exitError("Could not allocate memory for monster. Exiting");
        }

        monster->attack = 1;
        monster->health = 2;
        monster->tile = 'X';

        Room* room = level->rooms[room_i];
        int monster_row = room->position.row + rand() % (room->height - 2) + 1;
        int monster_col = room->position.col + rand() % (room->width - 2) + 1;
        monster->position.row = monster_row;
        monster->position.col = monster_col;
        level->monsters[monster_index] = monster;

        monster_index++;
        if (monster_index == level->n_monsters) break;
    }
}

Level* generateLevel() {
    Level* level = malloc(sizeof(Level));
    if (level == NULL) {
        exitError("Could not allocate memory for Level. Exiting");
    }

    level->n_rooms = 3;
    level->n_tunnels = 2;
    level->n_monsters = 3;

    level->rooms = generateRooms(level->n_rooms);
    level->map = generateMap(level->rooms, level->n_rooms);
    generateTunnels(level->map, level->rooms, level->n_rooms);
    generateMonsters(level);

    level->player = malloc(sizeof(Player));
    if (level->player == NULL) {
        exitError("Could not allocate memory for player. Exiting.");
    }
    int starting_room = rand() % level->n_rooms;
    int starting_room_height = level->rooms[starting_room]->height;
    int starting_room_width = level->rooms[starting_room]->width;
    int starting_room_row = level->rooms[starting_room]->position.row;
    int starting_room_col = level->rooms[starting_room]->position.col;
    while (1) {
        int player_row = starting_room_row + rand() % (starting_room_height - 2) + 1;
        int player_col = starting_room_col + rand() % (starting_room_width - 2) + 1;
        if (player_row != level->monsters[starting_room]->position.row ||
                player_col != level->monsters[starting_room]->position.col) {
            level->player->position.row = player_row;
            level->player->position.col = player_col;
            break;
        }
    }
    level->player->health = MAX_HEALTH;
    level->player->tile = '@';

    return level;
}


void drawPlayer(Player* player) {
    mvprintw(player->position.row, player->position.col, "%c", player->tile);
    move(player->position.row, player->position.col); // move the cursor to match the player position
    refresh();
}

void drawMonsters(Monster** monsters, int n_monsters) {
    for (int i = 0; i < n_monsters; i++) {
        if (monsters[i] == NULL) continue;
        mvprintw(monsters[i]->position.row, monsters[i]->position.col, "%c", monsters[i]->tile);
    }
    refresh();
}

// returns true if a player/npc can move into that position
int canMoveTo(Level* level, int row, int col) {
    // mvinch returns the character a y,x coords in the screen
    char tile = level->map[row][col];
    switch(tile) {
        case '.':
        case '#':
        case '+':
            return TRUE;
    }
    return FALSE;
}

// Tries to move player to new position
void movePlayer(Level* level, int row, int col) {
    Player* player = level->player;
    int newCol = player->position.col + col;
    int newRow = player->position.row + row;
    if (canMoveTo(level, newRow, newCol)) {
        player->position.col += col;
        player->position.row += row;
    }
}

void handleInput(Level* level, int key) {
    switch (key) {
        case 'w':
            movePlayer(level, -1, 0);
            break;
        case 'a':
            movePlayer(level, 0, -1);
            break;
        case 's':
            movePlayer(level, 1, 0);
            break;
        case 'd':
            movePlayer(level, 0, 1);
            break;
    }

}

void updateMonsters(Level *level) {
    for (int i = 0; i < level->n_monsters; i++) {
        Monster* monster = level->monsters[i];
        if (monster->health == 0) continue;

        int newRow = monster->position.row;
        int newCol = monster->position.col;
        int move_horizontal = rand() % 2;
        if (move_horizontal) {
            int right = rand() % 2 ;
            if (right) {
                newCol++; // move right
            } else {
                newCol--; // move left
            }
        } else {
            int up = rand() % 2 ;
            if (up) {
                newRow--;
            } else {
                newRow++;
            }
        }

        if (canMoveTo(level, newRow, newCol)) {
            monster->position.row = newRow;
            monster->position.col = newCol;
        }
    }
}

