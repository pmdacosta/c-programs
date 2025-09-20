#include "dark.h"
#include "console.c"

typedef struct {
    SDL_Window* Window;
    SDL_Renderer* Renderer;
    SDL_Texture* Screen;
    int Pitch;
} GameRender;

global_variable GameRender GlobalGameRender;
global_variable C_Console* Console;

void dark_cleanup(void) {
    C_ConsoleFree(Console);
    SDL_DestroyTexture(GlobalGameRender.Screen);
    SDL_DestroyRenderer(GlobalGameRender.Renderer);
    SDL_DestroyWindow(GlobalGameRender.Window);
    IMG_Quit();
    SDL_Quit();
}

void dark_exit(int status) {
    dark_cleanup();
    exit(status);
}

// Returns 0 on success, 1 otherwise
int dark_init(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "%s:%d: SDL_Init failed: %s\n",__FILE__,__LINE__,SDL_GetError());
        return 1;
    }
    
    GlobalGameRender.Window = SDL_CreateWindow("Dark Carvern",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!GlobalGameRender.Window) {
        fprintf(stderr, "%s:%d: SDL_CreateWindow failed: %s\n",__FILE__,__LINE__,SDL_GetError());
        return 1;
    }

    GlobalGameRender.Renderer = SDL_CreateRenderer(GlobalGameRender.Window, 0, SDL_RENDERER_SOFTWARE);
    if (!GlobalGameRender.Renderer) {
        fprintf(stderr, "%s:%d: SDL_CreateRenderer failed: %s\n",__FILE__,__LINE__,SDL_GetError());
        return 1;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    if (SDL_RenderSetLogicalSize(GlobalGameRender.Renderer, SCREEN_WIDTH, SCREEN_HEIGHT) < 0) {
        fprintf(stderr, "%s:%d: SDL_RenderSetLogicalSize failed: %s\n",__FILE__,__LINE__,SDL_GetError());
        return 1;
    }

    GlobalGameRender.Screen = SDL_CreateTexture(GlobalGameRender.Renderer, SDL_PIXELFORMAT_RGBA8888, 
            SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!GlobalGameRender.Screen) {
        fprintf(stderr, "%s:%d: SDL_CreateTexture failed: %s\n",__FILE__,__LINE__,SDL_GetError());
        return 1;
    }

    GlobalGameRender.Pitch = SCREEN_WIDTH * sizeof(u32);

    if (!IMG_Init(IMG_INIT_PNG)) {
        fprintf(stderr, "%s:%d: IMG_Init failed: %s\n",__FILE__,__LINE__,IMG_GetError());
        return 1;
    }

    Console = C_ConsoleInit();
    if (!Console) {
        fprintf(stderr, "%s:%d: C_ConsoleInit failed",
                __FILE__,__LINE__);
        return 1;
    }

    if (C_ConsoleSetBitmapFont(Console, "images/terminal16x16.png")) {
        fprintf(stderr, "%s:%d: C_ConsoleSetBitmapFont failed",
                __FILE__,__LINE__);
        return 1;
    }

    return 0;
}

int main(void) {
    if (dark_init()) {
        fprintf(stderr, "%s:%d: Init failed: %s\n",
                __FILE__,__LINE__,SDL_GetError());
        dark_exit(1);
    }

    // Game Loop
    SDL_Event Event;
    int running = 1;
    C_Player* Player = Console->Player;

    while (running) {

        while (SDL_PollEvent(&Event)) {

            if (Event.type == SDL_QUIT) {
                dark_exit(0);
            }

            if (Event.type == SDL_KEYDOWN) {

                switch(Event.key.keysym.sym) {

                    case SDLK_q:
                        dark_cleanup();
                        return 0;

                    case SDLK_UP:
                        if (Player->Position.y > 0) Player->Position.y--;
                        break;

                    case SDLK_DOWN:
                        if (Player->Position.y + 1 < Console->Rows) Player->Position.y++;
                        break;

                    case SDLK_LEFT:
                        if (Player->Position.x > 0) Player->Position.x--;
                        break;

                    case SDLK_RIGHT:
                        if (Player->Position.x + 1 < Console->Cols) Player->Position.x++;
                        break;

                    default: break;
                }

            }
        } 

        C_ConsoleClear(Console);
        C_ConsolePutCharAt(Console, Player->Glyph, 
                Player->Position.x, Player->Position.y, COLOR_RED);
        SDL_UpdateTexture(GlobalGameRender.Screen, 0,
                Console->Pixels, Console->Pitch);
        SDL_RenderClear(GlobalGameRender.Renderer);
        SDL_RenderCopy(GlobalGameRender.Renderer, GlobalGameRender.Screen,
                0, 0);
        SDL_RenderPresent(GlobalGameRender.Renderer);

        SDL_Delay(16);
    }

    dark_cleanup();
    return 0;
}
