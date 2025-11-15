#include "SDL.h"
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
    Uint8 move_mode;
    Uint8 running;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
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
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    SDL_FreeSurface(surface);

    rect.x = 0;
    rect.y = 0;
    rect.w = 100;
    rect.h = 100;

    move_mode = 0;

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
                if (move_mode) {
                    rect.x = event.motion.x - rect.w / 2;
                    rect.y = event.motion.y - rect.h / 2;
                    if (rect.x < 0) rect.x = 0;
                    if (rect.y < 0) rect.y = 0;
                    if (rect.x > WIDTH - rect.w) rect.x = WIDTH - rect.w;
                    if (rect.y > HEIGHT - rect.h) rect.y = HEIGHT - rect.h;

                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                SDL_Point p;
                p.x = event.button.x;
                p.y = event.button.y;
                if (SDL_PointInRect(&p, &rect) && event.button.button == SDL_BUTTON_LEFT)
                {
                    move_mode = 1;
                }
                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    move_mode = 0;
                }
                break;
            }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0xff, 0xff);
        SDL_RenderClear(renderer);

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
