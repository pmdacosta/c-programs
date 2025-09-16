#include "main.h"
#include "console.h"
#include "types.h"
#include <SDL2/SDL_image.h>

global_variable GameRender GlobalGameRender;
global_variable C_Console* Console;

void Cleanup(void) {
    if (Console) {
        if (Console->Font) {
            free(Console->Font->Atlas);
            free(Console->Font);
        }
        free(Console->Cells);
        free(Console->Pixels);
        free(Console);
    }
    free(GlobalGameRender.Pixels);
    SDL_DestroyTexture(GlobalGameRender.Screen);
    SDL_DestroyRenderer(GlobalGameRender.Renderer);
    SDL_DestroyWindow(GlobalGameRender.Window);
    IMG_Quit();
    SDL_Quit();
}

void RenderScreen(void) {
    C_ConsoleClear(Console);
    SDL_UpdateTexture(GlobalGameRender.Screen, 0, GlobalGameRender.Pixels, GlobalGameRender.Pitch);
    SDL_RenderClear(GlobalGameRender.Renderer);
    SDL_RenderCopy(GlobalGameRender.Renderer, GlobalGameRender.Screen, 0, 0);
    SDL_RenderPresent(GlobalGameRender.Renderer);
}

void DrawPixels(int xOffset, int yOffset) {
    u8* Row = (u8*) GlobalGameRender.Pixels;
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        u32* Pixel = (u32 *) Row;
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            u8 Red = (u8) (x + xOffset);
            u8 Green = (u8) (y + yOffset);
            *Pixel = (u32) COLOR(Red, Green, 0, 0);
            Pixel++;
        }
        Row += GlobalGameRender.Pitch;
    }
}

// Returns 0 on success, 1 otherwise
int Init(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "%s:%d: SDL_Init failed: %s\n",
                __FILE__,__LINE__,SDL_GetError());
        return 1;
    }
    
    GlobalGameRender.Window = SDL_CreateWindow("Dark Carvern",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!GlobalGameRender.Window) {
        fprintf(stderr, "%s:%d: SDL_CreateWindow failed: %s\n",
                __FILE__,__LINE__,SDL_GetError());
        return 1;
    }

    GlobalGameRender.Renderer = SDL_CreateRenderer(GlobalGameRender.Window,
            0, SDL_RENDERER_SOFTWARE);
    if (!GlobalGameRender.Renderer) {
        fprintf(stderr, "%s:%d: SDL_CreateRenderer failed: %s\n",
                __FILE__,__LINE__,SDL_GetError());
        return 1;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    if (SDL_RenderSetLogicalSize(GlobalGameRender.Renderer, 
                SCREEN_WIDTH, SCREEN_HEIGHT) < 0) {
        fprintf(stderr, "%s:%d: SDL_RenderSetLogicalSize failed: %s\n",
                __FILE__,__LINE__,SDL_GetError());
        return 1;
    }

    GlobalGameRender.Screen = SDL_CreateTexture(GlobalGameRender.Renderer, SDL_PIXELFORMAT_RGBA8888, 
            SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!GlobalGameRender.Screen) {
        fprintf(stderr, "%s:%d: SDL_CreateTexture failed: %s\n",
                __FILE__,__LINE__,SDL_GetError());
        return 1;
    }

    GlobalGameRender.Pixels = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(u32));
    if (!GlobalGameRender.Pixels) {
        fprintf(stderr, "%s:%d: malloc failed: %s\n",
                __FILE__,__LINE__,SDL_GetError());
        return 1;
    }

    GlobalGameRender.Pitch = SCREEN_WIDTH * sizeof(u32);

    if (!IMG_Init(IMG_INIT_PNG)) {
        fprintf(stderr, "%s:%d: IMG_Init failed: %s\n",
                __FILE__,__LINE__,IMG_GetError());
        return 1;
    }

    Console = C_ConsoleInit(SCREEN_WIDTH, SCREEN_HEIGHT, 
            SCREEN_WIDTH / CELL_WIDTH, SCREEN_HEIGHT / CELL_HEIGHT);

    if (!Console) {
        fprintf(stderr, "%s:%d: C_ConsoleInit failed",
                __FILE__,__LINE__);
        return 1;
    }

    if (C_ConsoleSetBitmapFont(Console, "images/terminal16x16.png", ' ',
                CELL_WIDTH, CELL_HEIGHT)) {
        fprintf(stderr, "%s:%d: C_ConsoleSetBitmapFont failed",
                __FILE__,__LINE__);
        return 1;
    }

    return 0;
}

int main(void) {
    if (Init()) {
        fprintf(stderr, "%s:%d: Init failed: %s\n",
                __FILE__,__LINE__,SDL_GetError());
        Cleanup();
        return 1;
    }

    // Game Loop

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
