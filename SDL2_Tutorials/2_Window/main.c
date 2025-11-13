#include "SDL.h"
#include <stdio.h>

#define print_sdl_error(message) fprintf(stderr, "%s:%d: %s: %s\n", __FILE__, __LINE__, message, SDL_GetError())

int
main(void)
{
    SDL_Window *window;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    if (!window)
    {
        print_sdl_error("SDL_CreateWindow failed");
        return 1;
    }

    SDL_Delay(1000);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
