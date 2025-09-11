#ifndef GAME_H
#define GAME_H

#include "main.h"

#define WHITE_FLAKE 0
#define YELLOW_FLAKE 1

struct Flake {
    int type;
    SDL_Rect rect;
    struct Flake* next;
    SDL_Texture* texture;
};

struct Player {
    SDL_Texture* texture;
    SDL_Rect rect; 
    SDL_RendererFlip flip;
    Uint8 speed;
};

struct Game {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* background_texture;
    SDL_Texture* yellow_flake;
    SDL_Texture* white_flake;
    SDL_Rect flake_rect;
    struct Player player;
    struct Flake* flakes;
    int flake_count;
};

int game_new(struct Game* game);
void game_cleanup(struct Game* game);
int game_run(struct Game* game);
int game_initialize(struct Game* game);
int game_load_media(struct Game* game);

void player_update(struct Player *player);

struct Flake* flake_add(struct Game* game);
void flakes_update(struct Game *game);

#endif
