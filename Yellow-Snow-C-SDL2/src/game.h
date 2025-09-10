#ifndef GAME_H
#define GAME_H

#include "main.h"

struct Game {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* background_texture;
};

int game_new(struct Game* game);
void game_cleanup(struct Game* game);
int game_run(struct Game* game);
int game_initialize(struct Game* game);
int game_load_media(struct Game* game);

#endif
