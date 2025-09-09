#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define WINDOW_TITLE "Game"
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

struct Game {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* background;
    SDL_Texture* text;
    SDL_Rect text_rect;
    SDL_Color text_color;
};

void game_cleanup(struct Game* game, int exit_status);
int load_media(struct Game* game); 
int sdl_initialize(struct Game* game); 

int main() {
    struct Game game = {
        .window = NULL,
        .renderer = NULL,
        .background = NULL,
        .text = NULL,
        .text_rect = {300, 400},
        .text_color = {0, 255, 100, 255}
    };

    // initialize random number generator
    srand(time(NULL));

    if (sdl_initialize(&game) != EXIT_SUCCESS) {
        game_cleanup(&game, EXIT_FAILURE);
    }

    if (load_media(&game) != EXIT_SUCCESS) {
        game_cleanup(&game, EXIT_FAILURE);
    }

    int red = rand() % 255;
    int green = rand() % 255;
    int blue = rand() % 255;

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
                        case SDL_SCANCODE_SPACE: {
                                red = rand() % 255;
                                green = rand() % 255;
                                blue = rand() % 255;
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
        SDL_SetRenderDrawColor(game.renderer, red, green, blue, 255);
        // SDL_RenderCopy(game.renderer, game.background, NULL, NULL);
        SDL_RenderCopy(game.renderer, game.text, NULL, &game.text_rect);
        SDL_RenderPresent(game.renderer);
        SDL_Delay(16); // approximate 60fps
    }

    game_cleanup(&game, EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

void game_cleanup(struct Game* game, int exit_status) {
    SDL_DestroyTexture(game->text);
    SDL_DestroyTexture(game->background);
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    exit (exit_status);
}

int sdl_initialize(struct Game* game) {
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    int img_init_flags = IMG_Init(IMG_INIT_PNG);
    if ((img_init_flags & IMG_INIT_PNG) != IMG_INIT_PNG) {
        fprintf(stderr, "Error initializing IMG_Init: %s\n", IMG_GetError());
        return EXIT_FAILURE;
    }

    if (TTF_Init()) {
        fprintf(stderr, "Error initializing TTF_Init: %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }

    game->window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!game->window) {
        fprintf(stderr, "Error creating Window: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    game->renderer = SDL_CreateRenderer(game->window, -1, 0);
    if (!game->renderer) {
        fprintf(stderr, "Error creating Rendered: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int load_media(struct Game* game) {
    game->background = IMG_LoadTexture(game->renderer, "../images/background.png");
    if (!game->background) {
        fprintf(stderr, "Error loading texture: %s\n", IMG_GetError());
        return EXIT_FAILURE;
    }

    TTF_Font* font = TTF_OpenFont("../fonts/freesansbold.ttf", 80);
    if (!font) {
        fprintf(stderr, "Error creating TTF Font: %s\n", TTF_GetError());
        TTF_CloseFont(font);
        return EXIT_FAILURE;
    }

    SDL_Surface* text_surface = TTF_RenderText_Blended(font, "Hello World", game->text_color);
    TTF_CloseFont(font);
    if (!text_surface) {
        fprintf(stderr, "Error creating Text Surface: %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }

    game->text_rect.w = text_surface->w;
    game->text_rect.h = text_surface->h;

    game->text = SDL_CreateTextureFromSurface(game->renderer, text_surface);
    SDL_FreeSurface(text_surface);
    if (!game->text) {
        fprintf(stderr, "Error creating texture from surface: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}
