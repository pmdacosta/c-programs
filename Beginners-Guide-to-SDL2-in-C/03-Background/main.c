#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define WINDOW_TITLE "Game"
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

struct Game {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* background;
};

void game_cleanup(struct Game* game, int exit_status);
int load_media(struct Game* game); 
int sdl_initialize(struct Game* game); 

int main() {
    struct Game game = {
        .window = NULL,
        .renderer = NULL,
        .background = NULL
    };

    if (sdl_initialize(&game) != EXIT_SUCCESS) {
        game_cleanup(&game, EXIT_FAILURE);
    }

    if (load_media(&game) != EXIT_SUCCESS) {
        game_cleanup(&game, EXIT_FAILURE);
    }

    while (true) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    game_cleanup(&game, EXIT_SUCCESS);
                } break;

                case SDL_KEYDOWN: {
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_Q: {
                                 game_cleanup(&game, EXIT_SUCCESS);
                            } break;

                        default: 
                            break;
                    }
                } break;

                default:
                    break;
            }
        }

        SDL_RenderClear(game.renderer);
        SDL_RenderCopy(game.renderer, game.background, NULL, NULL);
        SDL_RenderPresent(game.renderer);
        SDL_Delay(16); // approximate 60fps
    }

    game_cleanup(&game, EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

void game_cleanup(struct Game* game, int exit_status) {
    SDL_DestroyTexture(game->background);
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    IMG_Quit();
    SDL_Quit();
    exit (exit_status);
}

int sdl_initialize(struct Game* game) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    int img_init_flags = IMG_Init(IMG_INIT_PNG);
    if ((img_init_flags & IMG_INIT_PNG) != IMG_INIT_PNG) {
        fprintf(stderr, "Error initializing IMG_Init: %s\n", IMG_GetError());
        return EXIT_FAILURE;
    }

    game->window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (game->window == NULL) {
        fprintf(stderr, "Error creating Window: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    game->renderer = SDL_CreateRenderer(game->window, -1, 0);
    if (game->renderer == NULL) {
        fprintf(stderr, "Error creating Rendered: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int load_media(struct Game* game) {
    game->background = IMG_LoadTexture(game->renderer, "../images/background.png");
    if (game->background == NULL) {
        fprintf(stderr, "Error loading texture: %s\n", IMG_GetError());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
