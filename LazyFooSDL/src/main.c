#include "main.h"

SDL_Window* Window = 0;
SDL_Surface* ScreenSurface = 0;

int main(void) {

    if (SDL_Init( SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "%s:%d: SDL_Init failed: %s\n", __FILE__, __LINE__, SDL_GetError());
        cleanup();
        return 1;
    }

    Window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if (!Window) {
        fprintf(stderr, "%s:%d: SDL_CreateWindow failed: %s\n", __FILE__, __LINE__, SDL_GetError());
        cleanup();
        return 1;
    }

    ScreenSurface = SDL_GetWindowSurface(Window);
    if (!ScreenSurface) {
        fprintf(stderr, "%s:%d: SDL_GetWindowSurface failed: %s\n", __FILE__, __LINE__, SDL_GetError());
        cleanup();
        return 1;
    }

    Uint32 color = SDL_MapRGB(ScreenSurface->format, 0xFF, 0x00, 0x00);
    printf("%x", color);
    if (SDL_FillRect(ScreenSurface, NULL, color)) {
        fprintf(stderr, "%s:%d: SDL_FillRect failed: %s\n", __FILE__, __LINE__, SDL_GetError());
        cleanup();
        return 1;
    }

    SDL_UpdateWindowSurface(Window);

    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }
        SDL_Delay(16);
    }

    cleanup();
    return 0;
}

void cleanup(void) {
    SDL_FreeSurface(ScreenSurface);
    SDL_DestroyWindow(Window);
    SDL_Quit();
}

