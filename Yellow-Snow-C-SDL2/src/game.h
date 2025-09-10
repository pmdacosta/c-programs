#ifndef GAME_H
#define GAME_H

#include "main.h"

struct Game {
    SDL_Window* window;
    SDL_Renderer* renderer;
};

struct Game* game_new();
void game_free(struct Game* game);
int game_run(struct Game* game);
int game_initialize(struct Game* game);

#endif
