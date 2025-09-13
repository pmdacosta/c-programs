#include "main.h"

global_variable SDL_Window* GlobalWindow;
global_variable SDL_Renderer* GlobalRenderer;
global_variable SDL_Texture* GlobalScreen;
global_variable u32* GlobalPixels;
global_variable int GlobalPitch = SCREEN_WIDTH * sizeof(u32);

void Cleanup(void) {
    free(GlobalPixels);
    SDL_DestroyTexture(GlobalScreen);
    SDL_DestroyRenderer(GlobalRenderer);
    SDL_DestroyWindow(GlobalWindow);
    SDL_Quit();
}

void RenderScreen(void) {
    SDL_UpdateTexture(GlobalScreen, 0, GlobalPixels, GlobalPitch);
    SDL_RenderClear(GlobalRenderer);
    SDL_RenderCopy(GlobalRenderer, GlobalScreen, 0, 0);
    SDL_RenderPresent(GlobalRenderer);
}

void DrawPixels(int xOffset, int yOffset) {
    u8* Row = (u8*) GlobalPixels;
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        u32* Pixel = (u32 *) Row;
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            u8 Red = (u8) (x + xOffset);
            u8 Green = (u8) (y + yOffset);
            *Pixel = (u32) (Red << 24 | Green << 16);
            Pixel++;
        }
        Row += GlobalPitch;
    }
}

int main(void) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "%s:%d: SDL_Init failed: %s\n",
                __FILE__,__LINE__,SDL_GetError());
        return 1;
    }
    
    GlobalWindow = SDL_CreateWindow("Dark Carvern",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!GlobalWindow) {
        fprintf(stderr, "%s:%d: SDL_CreateWindow failed: %s\n",
                __FILE__,__LINE__,SDL_GetError());
        Cleanup();
        return 1;
    }

    GlobalRenderer = SDL_CreateRenderer(GlobalWindow,
            0, SDL_RENDERER_SOFTWARE);
    if (!GlobalRenderer) {
        fprintf(stderr, "%s:%d: SDL_CreateRenderer failed: %s\n",
                __FILE__,__LINE__,SDL_GetError());
        Cleanup();
        return 1;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    if (SDL_RenderSetLogicalSize(GlobalRenderer, 
                SCREEN_WIDTH, SCREEN_HEIGHT) < 0) {
        fprintf(stderr, "%s:%d: SDL_RenderSetLogicalSize failed: %s\n",
                __FILE__,__LINE__,SDL_GetError());
        Cleanup();
        return 1;
    }

    GlobalScreen = SDL_CreateTexture(GlobalRenderer, SDL_PIXELFORMAT_RGBA8888, 
            SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!GlobalScreen) {
        fprintf(stderr, "%s:%d: SDL_CreateTexture failed: %s\n",
                __FILE__,__LINE__,SDL_GetError());
        Cleanup();
        return 1;
    }

    GlobalPixels = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(u32));
    if (!GlobalPixels) {
        fprintf(stderr, "%s:%d: malloc failed: %s\n",
                __FILE__,__LINE__,SDL_GetError());
        Cleanup();
        return 1;
    }

    SDL_Event Event;
    int running = 1;
    int x = 0;
    int y = 0;

    while (running) {
        while (SDL_PollEvent(&Event)) {
            if (Event.type == SDL_QUIT) {
                running = 0;
            }
        }

        DrawPixels(x, y);
        y+=2;
        x++;
        RenderScreen();
        SDL_Delay(16);
    }

    Cleanup();
    return 0;
}
