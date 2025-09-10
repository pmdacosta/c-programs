#include "game.h"
#include "main.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>

int game_new(struct Game* game) {
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

    if (game_load_media(game)) {
        fprintf(stderr, "%s:%d: game_load_media failed\n",
                __FILE__, __LINE__);
        return 1;
    }

    game->player.rect.x = (WINDOW_WIDTH - game->player.rect.w) / 2;
    game->player.rect.y = 377;
    game->player.flip = 0;
    game->player.speed = 5;

    return 0;
}

int game_run(struct Game* game) {
    SDL_Event event;

    for (;;) {
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: 
                    {
                        return 0;
                    } break;
                case SDL_KEYDOWN:
                    {
                        switch(event.key.keysym.scancode) {
                            case SDL_SCANCODE_Q: 
                                {
                                    return 0;
                                } break;
                            default:
                                break;
                        }
                    } break;
                default:
                    break;
            }
        }

        player_update(&game->player);

        SDL_RenderClear(game->renderer);

        SDL_RenderCopy(game->renderer, game->background_texture, 0, 0);
        SDL_RenderCopyEx(game->renderer, game->player.image, 
                0, &game->player.rect, 0, 0, game->player.flip); 

        SDL_RenderPresent(game->renderer);
        SDL_Delay(16);
    }

    return 0;
}

void game_cleanup(struct Game *game) {
    SDL_DestroyTexture(game->player.image);
    SDL_DestroyTexture(game->background_texture);
    SDL_DestroyWindow(game->window);
    SDL_DestroyRenderer(game->renderer);
    IMG_Quit();
    SDL_Quit();
}

int game_load_media(struct Game *game) {
    if (!IMG_Init(IMG_INIT_PNG)) {
        fprintf(stderr, "%s:%d: IMG_Init failed: %s\n",
                __FILE__, __LINE__, IMG_GetError());
        return 1;
    }

    SDL_Surface* icon = IMG_Load("images/yellow.png");
    if (!icon) {
        fprintf(stderr, "%s:%d: IMG_Load failed: %s\n",
                __FILE__, __LINE__, IMG_GetError());
        return 1;
    }
    SDL_SetWindowIcon(game->window, icon);
    SDL_FreeSurface(icon);

    game->background_texture = IMG_LoadTexture(game->renderer, "images/background.png");
    if (!game->background_texture) {
        fprintf(stderr, "%s:%d: IMG_LoadTexture failed: %s\n",
                __FILE__, __LINE__, IMG_GetError());
        return 1;
    }

    struct Player* player = &game->player;
    player->image = IMG_LoadTexture(game->renderer, "images/player.png");
    if (!player->image) {
        fprintf(stderr, "%s:%d: IMG_LoadTexture failed: %s\n",
                __FILE__, __LINE__, IMG_GetError());
        return 1;
    }

    if (SDL_QueryTexture(player->image, 0, 0,
                &player->rect.w, &player->rect.h)) {
        fprintf(stderr, "%s:%d: SDL_QueryTexture failed: %s\n",
                __FILE__, __LINE__, IMG_GetError());
        return 1;
    }

    return 0;
}

// Player functions
void player_update(struct Player *player) {
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    int x = player->rect.x;
    int w = player->rect.w;
    if (keystate[SDL_SCANCODE_RIGHT]) {
        x += player->speed;
        if (x + w >= WINDOW_WIDTH - 1) {
            x = WINDOW_WIDTH - w - 1;
        }
        player->flip = 0;
    } 
    if (keystate[SDL_SCANCODE_LEFT]) {
        x -= player->speed;
        if (x <= 0) {
            x = 0;
        }
        player->flip = SDL_FLIP_HORIZONTAL;
    }
    player->rect.x = x;
}

