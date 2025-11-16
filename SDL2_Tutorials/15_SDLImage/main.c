#include "SDL.h"
#include "SDL_image.h"
#include <stdio.h>

#define WIDTH 640
#define HEIGHT 480

#define print_sdl_error(message) fprintf(stderr, "%s:%d: %s: %s\n", __FILE__, __LINE__, message, SDL_GetError())

int
main(void)
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect rect;
    Uint8 running;

    SDL_Init(SDL_INIT_VIDEO);
    if (!IMG_Init(IMG_INIT_PNG))
    {
        print_sdl_error("IMG_Init failed");
        return 1;
    }

    window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
    {
        print_sdl_error("SDL_CreateWindow failed");
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        print_sdl_error("SDL_CreateWindow failed");
        return 1;
    }

    surface = IMG_Load("images/mario.png");
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    rect.x = 10;
    rect.y = 10;
    rect.w = 400;
    rect.h = 200;

    running = 1;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
            {
                running = 0;
                goto quit;
                break;
            }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0xff, 0xff);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, 0, &rect);

        SDL_RenderPresent(renderer);
        /*
        SDL_Delay(20);
        */
    }

quit:
    SDL_DestroyTexture(texture);
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
