#include "main.h"
#include <SDL2/SDL_image.h>

SDL_Window* GlobalWindow = 0;
SDL_Surface* GlobalScreenSurface = 0;
SDL_Surface* GlobalStretchedSurface = 0;

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

    if (!IMG_Init(IMG_INIT_PNG)) {
        fprintf(stderr, "%s:%d: IMG_Init failed: %s\n", __FILE__, __LINE__, IMG_GetError());
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
    GlobalStretchedSurface = LoadSurface("images/loaded.png");
    if (!GlobalStretchedSurface) {
        fprintf(stderr, "%s:%d: LoadSurface failed: %s\n", __FILE__, __LINE__, SDL_GetError());
        return 1;
    }

    return 0;
}

void Cleanup(void) {
    SDL_DestroyWindow(GlobalWindow);
    IMG_Quit();
    SDL_Quit();
}

/* returns pointer to a loaded surface or 0 on error */
SDL_Surface* LoadSurface(const char* file) {
    SDL_Surface* LoadedSurface = IMG_Load(file);
    if (!LoadedSurface) {
        fprintf(stderr, "%s:%d: IMG_Load failed: %s\n", __FILE__, __LINE__, IMG_GetError());
        return 0;
    }

    SDL_Surface* OptimizedSurface = SDL_ConvertSurface(LoadedSurface, LoadedSurface->format, 0);
    if (!OptimizedSurface) {
        fprintf(stderr, "%s:%d: SDL_ConvertSurface failed: %s\n", __FILE__, __LINE__, SDL_GetError());
        return 0;
    }

    SDL_FreeSurface(LoadedSurface);

    return OptimizedSurface;
}

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

    SDL_Rect StretchRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    SDL_Event event;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        {
                        } break;
                    case SDLK_DOWN:
                        {
                        } break;
                    case SDLK_LEFT:
                        {
                        } break;
                    case SDLK_RIGHT:
                        {
                        } break;
                    default:
                        {
                            running = 0;
                        } break;
                }
            }
        }

        SDL_BlitScaled(GlobalStretchedSurface, 0, GlobalScreenSurface, &StretchRect);
        SDL_UpdateWindowSurface(GlobalWindow);
        SDL_Delay(16);
    }

    Cleanup();
    return 0;
}
