#include "SDL.h"
#include "SDL_ttf.h"
#include <stdio.h>

#define WIDTH 640
#define HEIGHT 480

#define print_sdl_error(message) fprintf(stderr, "%s:%d: %s: %s\n", __FILE__, __LINE__, message, SDL_GetError())

int
main(void)
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *surface_text;
    SDL_Texture *texture_text;
    SDL_Color color;
    SDL_Rect rect;
    TTF_Font *font;
    Uint8 running;

    SDL_Init(SDL_INIT_VIDEO);
    if (TTF_Init() == -1)
    {
        print_sdl_error("TTF_Init failed");
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

    font = TTF_OpenFont("fonts/8bitOperatorPlus8-Regular.ttf", 32);
    if (!font)
    {
        print_sdl_error("TTF_OpenFont failed");
    }
    color.r = 0xff; 
    color.g = 0xff; 
    color.b = 0xff; 
    surface_text = TTF_RenderText_Solid(font, "Hello World", color);

    texture_text = SDL_CreateTextureFromSurface(renderer, surface_text);
    TTF_CloseFont(font);
    SDL_FreeSurface(surface_text);

    rect.x = 10;
    rect.y = 10;
    rect.w = 400;
    rect.h = 100;

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

        SDL_RenderCopy(renderer, texture_text, 0, &rect);
        SDL_RenderPresent(renderer);
        /*
        SDL_Delay(20);
        */
    }

quit:
    SDL_DestroyTexture(texture_text);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
