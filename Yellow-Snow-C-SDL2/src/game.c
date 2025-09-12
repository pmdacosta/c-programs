#include "game.h"
#include "main.h"

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

    game->ground_height = 514;

    // setup player
    game->player.rect.x = (WINDOW_WIDTH - game->player.rect.w) / 2;
    game->player.rect.y = 377;
    game->player.flip = 0;
    game->player.speed = 5;

    // setup flakes
    game->flakes = 0;
    game->flake_count = 0;

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

        if (rand() % 150 == 0) {
            game->flakes = flake_add(game);
        }

        flakes_update(game);

        SDL_RenderClear(game->renderer);

        SDL_RenderCopy(game->renderer, game->background_texture, 0, 0);
        SDL_RenderCopyEx(game->renderer, game->player.texture, 
                0, &game->player.rect, 0, 0, game->player.flip); 

        struct Flake* flake = game->flakes;
        while(flake) {
            SDL_RenderCopy(game->renderer, flake->texture
                    , 0, &flake->rect);
            flake = flake->next;
        }

        SDL_RenderPresent(game->renderer);
        SDL_Delay(16);
    }

    return 0;
}

void game_cleanup(struct Game *game) {
    struct Flake* flake = game->flakes;

    while(flake) {
        struct Flake* next = flake->next;
        free(flake);
        flake = next;
    }

    SDL_DestroyTexture(game->yellow_flake);
    SDL_DestroyTexture(game->white_flake);
    SDL_DestroyTexture(game->player.texture);
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
    player->texture = IMG_LoadTexture(game->renderer, "images/player.png");
    if (!player->texture) {
        fprintf(stderr, "%s:%d: IMG_LoadTexture failed: %s\n",
                __FILE__, __LINE__, IMG_GetError());
        return 1;
    }

    if (SDL_QueryTexture(player->texture, 0, 0,
                &player->rect.w, &player->rect.h)) {
        fprintf(stderr, "%s:%d: SDL_QueryTexture failed: %s\n",
                __FILE__, __LINE__, IMG_GetError());
        return 1;
    }

    game->white_flake = IMG_LoadTexture(game->renderer, "images/white.png");
    if (!game->white_flake) {
        fprintf(stderr, "%s:%d: IMG_LoadTexture failed: %s\n",
                __FILE__, __LINE__, IMG_GetError());
        return 1;
    }

    game->yellow_flake = IMG_LoadTexture(game->renderer, "images/yellow.png");
    if (!game->yellow_flake) {
        fprintf(stderr, "%s:%d: IMG_LoadTexture failed: %s\n",
                __FILE__, __LINE__, IMG_GetError());
        return 1;
    }


    if (SDL_QueryTexture(game->white_flake, 0, 0, 
                &game->flake_rect.w, &game->flake_rect.h)) {
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

// Flake functions
struct Flake* flake_add(struct Game* game) {
    struct Flake* flake = malloc(sizeof(struct Flake));
    if (!flake) {
        fprintf(stderr, "%s:%d: malloc failed: %s\n",
                __FILE__, __LINE__, SDL_GetError());
        return 0;
    }
    flake->next = game->flakes;
    flake->rect = game->flake_rect;

    flake->rect.y = -flake->rect.h;
    flake->rect.x = rand() % (WINDOW_WIDTH - flake->rect.w);

    flake->type = rand() % 2;
    if (flake->type == WHITE_FLAKE) {
        flake->texture = game->white_flake;
    } else {
        flake->texture = game->yellow_flake;
    }

    flake->speed = 1 + rand() % 2;

    return flake;
}

void flakes_update(struct Game* game) {
    struct Flake *flake = game->flakes;
    struct Flake *previous_flake = 0;

    while (flake) {
        flake->rect.y += flake->speed;

        // make the flakes smaller when they hit the ground
        if (flake->rect.y > game->ground_height - flake->speed * 7) {
            flake->rect.h -= flake->speed;
            flake->rect.w -= flake->speed;
            flake->rect.x += 1;
        }

        // make the flakes disappear when they hit the ground
        if (flake->rect.y >= game->ground_height) {
            if (!previous_flake) {
                // we're at the head
                game->flakes = flake->next;
                free(flake);
                if (!game->flakes) {
                    return;
                }
                flake = game->flakes;
            } else {
                previous_flake->next = flake->next;
                free(flake);
                flake = previous_flake;
            }
        }

        previous_flake = flake;
        flake = flake->next;
    }
}
