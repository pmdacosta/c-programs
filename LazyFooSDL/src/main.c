#include "main.h"

SDL_Window* GlobalWindow = 0;
SDL_Surface* GlobalScreenSurface = 0;
SDL_Surface* GlobalCurrentSurface = 0;
SDL_Surface* GlobalKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];

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
    GlobalKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = LoadSurface("images/press.bmp");
    GlobalKeyPressSurfaces[KEY_PRESS_SURFACE_UP] = LoadSurface("images/up.bmp");
    GlobalKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = LoadSurface("images/down.bmp");
    GlobalKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = LoadSurface("images/left.bmp");
    GlobalKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = LoadSurface("images/right.bmp");

    for (int i = 0; i < KEY_PRESS_SURFACE_TOTAL; ++i) {
        if (!GlobalKeyPressSurfaces[i]) {
            fprintf(stderr, "%s:%d: LoadSurface failed: %s\n", __FILE__, __LINE__, SDL_GetError());
            return 1;
        }
    }

    return 0;
}

void Cleanup(void) {
    SDL_DestroyWindow(GlobalWindow);
    SDL_Quit();
}

/* returns pointer to a loaded surface or 0 on error */
SDL_Surface* LoadSurface(const char* file) {
    SDL_Surface* LoadedSurface = SDL_LoadBMP(file);
    if (!LoadedSurface) {
        fprintf(stderr, "%s:%d: SDL_LoadBMP failed: %s\n", __FILE__, __LINE__, SDL_GetError());
        return 0;
    }

    return LoadedSurface;
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

    GlobalCurrentSurface = GlobalKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
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
                            GlobalCurrentSurface = GlobalKeyPressSurfaces[KEY_PRESS_SURFACE_UP];
                        } break;
                    case SDLK_DOWN:
                        {
                            GlobalCurrentSurface = GlobalKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN];
                        } break;
                    case SDLK_LEFT:
                        {
                            GlobalCurrentSurface = GlobalKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT];
                        } break;
                    case SDLK_RIGHT:
                        {
                            GlobalCurrentSurface = GlobalKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT];
                        } break;
                    default:
                        {
                            GlobalCurrentSurface = GlobalKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
                        } break;
                }
            }
        }

        SDL_BlitSurface(GlobalCurrentSurface, 0, GlobalScreenSurface, 0);
        SDL_UpdateWindowSurface(GlobalWindow);
        SDL_Delay(16);
    }

    Cleanup();
    return 0;
}
