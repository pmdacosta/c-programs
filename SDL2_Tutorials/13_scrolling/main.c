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
    SDL_Texture *texture2;
    SDL_Rect rect[4];
    Uint8 running;

    SDL_Init(SDL_INIT_VIDEO);

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

    surface = SDL_LoadBMP("images/pool.bmp");
    SDL_SetColorKey(surface, 1, SDL_MapRGB(surface->format, 0xff, 0x00, 0xff));
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    texture2 = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);

    rect[0].x = 0;
    rect[0].y = 0;
    rect[0].w = WIDTH;
    rect[0].h = HEIGHT;

    rect[1].x = -WIDTH;
    rect[1].y = 0;
    rect[1].w = WIDTH;
    rect[1].h = HEIGHT;

    rect[2].x = 0;
    rect[2].y = 0;
    rect[2].w = WIDTH;
    rect[2].h = HEIGHT;

    rect[3].x = 0;
    rect[3].y = -HEIGHT;
    rect[3].w = WIDTH;
    rect[3].h = HEIGHT;

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
            case SDL_MOUSEBUTTONDOWN:
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    SDL_SetTextureBlendMode(texture2, SDL_BLENDMODE_ADD);
                }
                else if (event.button.button == SDL_BUTTON_MIDDLE)
                {
                    SDL_SetTextureBlendMode(texture2, SDL_BLENDMODE_BLEND);
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    SDL_SetTextureBlendMode(texture2, SDL_BLENDMODE_MOD);
                }
                break;
            }
            }
        }

        rect[0].x++;
        rect[1].x++;
        rect[2].y++;
        rect[3].y++;

        if (rect[0].x >= WIDTH) rect[0].x = -WIDTH;
        if (rect[1].x >= WIDTH) rect[1].x = -WIDTH;
        if (rect[2].y >= HEIGHT) rect[2].y = -HEIGHT;
        if (rect[3].y >= HEIGHT) rect[3].y = -HEIGHT;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0xff, 0xff);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, texture, 0, &rect[0]);
        SDL_RenderCopy(renderer, texture, 0, &rect[1]);
        SDL_RenderCopy(renderer, texture2, 0, &rect[2]);
        SDL_RenderCopy(renderer, texture2, 0, &rect[3]);

        SDL_RenderPresent(renderer);
        /*
        SDL_Delay(20);
        */
    }

quit:
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
