#include "SDL.h"
#include <stdio.h>

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

    window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    if (!window)
    {
        print_sdl_error("SDL_CreateWindow failed");
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        print_sdl_error("SDL_CreateWindow failed");
        return 1;
    }

    surface = SDL_LoadBMP("images/kong0.bmp");
    SDL_SetColorKey(surface, 1, SDL_MapRGB(surface->format, 0xff, 0x00, 0xff));
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    rect.x = 10;
    rect.y = 10;
    rect.w = 200;
    rect.h = 200;

    running = 1;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            Sint32 x, y;

            SDL_GetMouseState(&x, &y);
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

        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderDrawLine(renderer, 5, 5, 300, 240);

        SDL_RenderCopy(renderer, texture, 0, &rect);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

quit:
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
