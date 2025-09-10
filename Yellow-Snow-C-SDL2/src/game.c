#include "game.h"
#include "main.h"
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

struct Game* game_new() {
    struct Game* game = calloc(1, sizeof(struct Game));
    if (!game) {
        fprintf(stderr, "%s:%d: calloc failed in game_new()\n", __FILE__, __LINE__);
        return 0;
    }

    if (game_initialize(game)) {
        return 0;
    }

    return game;
}

int game_run(struct Game *game) {
    return 0;
}

void game_free(struct Game *game) {
    SDL_DestroyWindow(game->window);
    SDL_DestroyRenderer(game->renderer);
    SDL_Quit();
    free(game);
}

int game_initialize(struct Game *game) {
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        fprintf(stderr, "%s:%d: SDL_Init failed: %s\n",
                __FILE__, __LINE__, SDL_GetError());
        return 1;
    }

    game->window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!game->window) {
        fprintf(stderr, "%s:%d: SDL_CreateWindow failed: %s\n",
                __FILE__, __LINE__, SDL_GetError());
        return 1;
    }

    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
    if (!game->renderer) {
        fprintf(stderr, "%s:%d: SDL_CreateRenderer failed: %s\n",
                __FILE__, __LINE__, SDL_GetError());
        return 1;
    }

    return 0; 
}
