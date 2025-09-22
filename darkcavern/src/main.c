/* main.c */
#include "dark.h"
#include "console.c"

typedef struct {
    SDL_Window* Window;
    SDL_Renderer* Renderer;
    SDL_Texture* Screen;
    int Pitch;
} GameRender;

global_variable GameRender GlobalGameRender;
global_variable C_Console* GlobalConsole;

void dark_cleanup(void) {
    C_ConsoleFree(GlobalConsole);
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

    GlobalConsole = C_ConsoleInit();
    if (!GlobalConsole) {
        fprintf(stderr, "%s:%d: C_ConsoleInit failed",
                __FILE__,__LINE__);
        return 1;
    }

    if (C_ConsoleSetBitmapFont(GlobalConsole, "images/terminal16x16.png")) {
        fprintf(stderr, "%s:%d: C_ConsoleSetBitmapFont failed",
                __FILE__,__LINE__);
        return 1;
    }

    return 0;
}

// == ECS ===============================

global_variable ECS_Entity GlobalEntityArray[ECS_MAX_ENTITIES];
global_variable u32 GlobalEntityIDAvailable = 0;
global_variable u32 GlobalEntityCount = 0;

uchar ECS_EntityGetGlyph(ECS_EntityType Type) {
    switch(Type) {
        case ECS_ENTITY_PLAYER:
            return '@';
        case ECS_ENTITY_WALL:
            return '#';
        default:
            return 0;
    }
}

// returns EntityID or -1 if no slots available
int ECS_EntityAdd(ECS_EntityType Type, u32 Row, u32 Col, u32 Color) {
    if (GlobalEntityCount == ECS_MAX_ENTITIES) return -1;

    int EntityID = GlobalEntityIDAvailable;
    GlobalEntityArray[EntityID].Active = 1;
    GlobalEntityArray[EntityID].Type = Type;
    GlobalEntityArray[EntityID].Row = Row;
    GlobalEntityArray[EntityID].Col = Col;
    GlobalEntityArray[EntityID].Color = Color;
    GlobalEntityCount++;

    // Set new AvailableEntityID
    if (GlobalEntityCount < ECS_MAX_ENTITIES) {
        while (GlobalEntityArray[GlobalEntityIDAvailable].Active) {
            GlobalEntityIDAvailable = (GlobalEntityIDAvailable + 1) % ECS_MAX_ENTITIES;
        }
    }

    return EntityID;
}
void ECS_DisableEntity(int EntityID) {
    GlobalEntityArray[EntityID].Active = 0;
    GlobalEntityIDAvailable = EntityID;
    GlobalEntityCount--;
}

void ECS_Init(void) {
    for (int i = 0; i < ECS_MAX_ENTITIES; i++) {
        GlobalEntityArray[i].Active = 0;
    }
}

void ECS_EntityMoveBy(u32 EntityID, int RowChange, int ColChange) {
    ECS_Entity Entity = GlobalEntityArray[EntityID];

    if (Entity.Row == 0 && RowChange < 0) return; 
    if (Entity.Col == 0 && ColChange < 0) return; 

    u32 Row = Entity.Row + RowChange;
    u32 Col = Entity.Col + ColChange;

    if (Row >= GlobalConsole->Rows || Col >= GlobalConsole->Cols) return;

    for (u32 SearchEntityID = 0; SearchEntityID < GlobalEntityCount; SearchEntityID++) {
        if (SearchEntityID == EntityID) continue;
        ECS_Entity* SearchEntity = &GlobalEntityArray[SearchEntityID];
        if (!SearchEntity->Active) continue;
        if (SearchEntity->Row == Row && SearchEntity->Col == Col) {
            if (SearchEntity->Type == ECS_ENTITY_WALL) return;
            break;
        }
    }

    GlobalEntityArray[EntityID].Row = Row;
    GlobalEntityArray[EntityID].Col = Col;
}

// ======================================


int main(void) {
    if (dark_init()) {
        fprintf(stderr, "%s:%d: Init failed: %s\n",
                __FILE__,__LINE__,SDL_GetError());
        dark_exit(1);
    }

    // Game Loop
    SDL_Event Event;
    int running = 1;
    int PlayerID = ECS_EntityAdd(ECS_ENTITY_PLAYER, 25, 25,  COLOR_GREEN);
    int WallID = ECS_EntityAdd(ECS_ENTITY_WALL, 30, 30,  COLOR_RED);

    while (running) {

        while (SDL_PollEvent(&Event)) {

            if (Event.type == SDL_QUIT) {
                dark_exit(0);
            }

            if (Event.type == SDL_KEYDOWN) {

                switch(Event.key.keysym.sym) {

                    case SDLK_ESCAPE:
                        dark_cleanup();
                        return 0;

                    case SDLK_UP:
                        ECS_EntityMoveBy(PlayerID, -1, 0);
                        break;

                    case SDLK_DOWN:
                        ECS_EntityMoveBy(PlayerID, 1, 0);
                        break;

                    case SDLK_LEFT:
                        ECS_EntityMoveBy(PlayerID, 0, -1);
                        break;

                    case SDLK_RIGHT:
                        ECS_EntityMoveBy(PlayerID, 0, 1);
                        break;

                    default: break;
                }

            }
        } 

        ECS_Entity Player = GlobalEntityArray[PlayerID];
        ECS_Entity Wall = GlobalEntityArray[WallID];

        C_ConsoleClear(GlobalConsole);
        C_ConsolePutCharAt(GlobalConsole, ECS_EntityGetGlyph(Player.Type), Player.Col, Player.Row, Player.Color);
        C_ConsolePutCharAt(GlobalConsole, ECS_EntityGetGlyph(Wall.Type), Wall.Col, Wall.Row, Wall.Color);
        SDL_UpdateTexture(GlobalGameRender.Screen, 0, GlobalConsole->Pixels, GlobalConsole->Pitch);
        SDL_RenderClear(GlobalGameRender.Renderer);
        SDL_RenderCopy(GlobalGameRender.Renderer, GlobalGameRender.Screen,
                0, 0);
        SDL_RenderPresent(GlobalGameRender.Renderer);

        SDL_Delay(16);
    }

    dark_cleanup();
    return 0;
}
