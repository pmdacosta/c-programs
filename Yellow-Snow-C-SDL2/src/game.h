#ifndef GAME_H
#define GAME_H

#include "main.h"


struct Player {
    SDL_Texture* image;
    SDL_Rect rect; 
    SDL_RendererFlip flip;
};

struct Game {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* background_texture;
    struct Player player;
};

int game_new(struct Game* game);
void game_cleanup(struct Game* game);
int game_run(struct Game* game);
int game_initialize(struct Game* game);
int game_load_media(struct Game* game);

int player_new(struct Player* player);
void player_free(struct Player* player);
void player_update(struct Player *player, int mouse_x, int prev_mouse_x);

#endif
