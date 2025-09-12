#include "main.h"
#include <SDL2/SDL_surface.h>

SDL_Window* GlobalWindow = 0;
SDL_Surface* GlobalScreenSurface = 0;
SDL_Surface* GlobalHelloWorld = 0;

int main(void) {

    if (Init()) {
        fprintf(stderr, "%s:%d: Init() failed: %s\n", __FILE__, __LINE__, SDL_GetError());
        Cleanup();
        return 1;
    }

    if (LoadMedia()) {
        fprintf(stderr, "%s:%d: LoadMedia() failed: %s\n", __FILE__, __LINE__, SDL_GetError());
        Cleanup();
        return 1;
    }

    SDL_BlitSurface(GlobalHelloWorld, 0, GlobalScreenSurface, 0);
    SDL_UpdateWindowSurface(GlobalWindow);

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

    Cleanup();
    return 0;
}

/* Initliaze game
 * returns 0 on success or 1 otherwise */
int Init(void) {
    if (SDL_Init( SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "%s:%d: SDL_Init failed: %s\n", __FILE__, __LINE__, SDL_GetError());
        return 1;
    }

    GlobalWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if (!GlobalWindow) {
        fprintf(stderr, "%s:%d: SDL_CreateWindow failed: %s\n", __FILE__, __LINE__, SDL_GetError());
        return 1;
    }

    GlobalScreenSurface = SDL_GetWindowSurface(GlobalWindow);
    if (!GlobalScreenSurface) {
        fprintf(stderr, "%s:%d: SDL_GetWindowSurface failed: %s\n", __FILE__, __LINE__, SDL_GetError());
        return 1;
    }

    return 0;
}

int LoadMedia(void) {
    GlobalHelloWorld = SDL_LoadBMP("images/hello_world.bmp");
    if (!GlobalHelloWorld) {
        fprintf(stderr, "%s:%d: SDL_LoadBMP failed: %s\n", __FILE__, __LINE__, SDL_GetError());
        return 1;
    }

    return 0;
}

void Cleanup(void) {
    SDL_FreeSurface(GlobalHelloWorld);
    SDL_DestroyWindow(GlobalWindow);
    SDL_Quit();
}

