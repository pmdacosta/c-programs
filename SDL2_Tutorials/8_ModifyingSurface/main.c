#include "SDL.h"
#include <stdio.h>

#define print_sdl_error(message) fprintf(stderr, "%s:%d: %s: %s\n", __FILE__, __LINE__, message, SDL_GetError())

int
main(void)
{
    SDL_Window *window;
    Uint8 running;
    SDL_Surface *screen;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    if (!window)
    {
        print_sdl_error("SDL_CreateWindow failed");
        return 1;
    }

    screen = SDL_GetWindowSurface(window);
    if (!screen)
    {
        print_sdl_error("SDL_CreateWindow failed");
        return 1;
    }

    SDL_UpdateWindowSurface(window);
    printf("bytes: %d\n", screen->format->BytesPerPixel);

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
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEMOTION:
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    Uint8 *pixels;
                    Uint32 *pixel;
                    Uint32 color;
                    x = event.button.x;
                    y = event.button.y;
                    pixels = (Uint8*)(screen->pixels) + y * screen->pitch + x * screen->format->BytesPerPixel;
                    pixel = (Uint32*)pixels;
                    color = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
                    SDL_LockSurface(screen);
                    *pixel = color;
                    SDL_UnlockSurface(screen);
                }
            }
            }
        }

        SDL_UpdateWindowSurface(window);
        SDL_Delay(16);
    }

quit:
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
