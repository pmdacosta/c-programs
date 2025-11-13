#include "SDL.h"
#include <stdio.h>

#define print_sdl_error(message) fprintf(stderr, "%s:%d: %s: %s\n", __FILE__, __LINE__, message, SDL_GetError())

int
main(void)
{
    SDL_Window *window;
    Uint8 running;
    SDL_Surface *screen;
    SDL_Surface *image;

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

    image = SDL_LoadBMP("images/blender.bmp");
    SDL_BlitSurface(image, 0, screen, 0);
    SDL_FreeSurface(image);
    SDL_UpdateWindowSurface(window);

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
            case SDL_MOUSEMOTION:
            {
                SDL_Point p;
                p.x = event.motion.x;
                p.y = event.motion.y;
                printf("mouse potion: %d, %d\n", p.x, p.y);
                break;
            }
            case SDL_KEYDOWN:
            {
                Sint32 keycode;
                keycode = event.key.keysym.sym;
                printf("key pressed: %x\n", keycode); 

                if (keycode == SDLK_SPACE) {
                    printf("    space bar pressed\n"); 
                }

                break;
            }
            default:
            {
                break;
            }
            }
        }

        SDL_Delay(16);
    }

quit:
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
