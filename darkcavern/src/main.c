/* main.c */
#include "console.c"
#include "dark.h"
#include <stdlib.h>
#include <time.h>

typedef struct
{
    SDL_Window *Window;
    SDL_Renderer *Renderer;
    SDL_Texture *Screen;
    int Pitch;
} GameRender;

global_variable GameRender GlobalGameRender;
global_variable C_Console *GlobalConsole;
global_variable uchar GlobalMap[C_ROWS][C_COLS];

void dark_cleanup(void)
{
    C_ConsoleFree(GlobalConsole);
    SDL_DestroyTexture(GlobalGameRender.Screen);
    SDL_DestroyRenderer(GlobalGameRender.Renderer);
    SDL_DestroyWindow(GlobalGameRender.Window);
    IMG_Quit();
    SDL_Quit();
}

void dark_exit(int status)
{
    dark_cleanup();
    exit(status);
}

// Returns 0 on success, 1 otherwise
int dark_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "%s:%d: SDL_Init failed: %s\n", __FILE__, __LINE__, SDL_GetError());
        return 1;
    }

    GlobalGameRender.Window = SDL_CreateWindow("Dark Carvern", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!GlobalGameRender.Window)
    {
        fprintf(stderr, "%s:%d: SDL_CreateWindow failed: %s\n", __FILE__, __LINE__, SDL_GetError());
        return 1;
    }

    GlobalGameRender.Renderer = SDL_CreateRenderer(GlobalGameRender.Window, 0, SDL_RENDERER_SOFTWARE);
    if (!GlobalGameRender.Renderer)
    {
        fprintf(stderr, "%s:%d: SDL_CreateRenderer failed: %s\n", __FILE__, __LINE__, SDL_GetError());
        return 1;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    if (SDL_RenderSetLogicalSize(GlobalGameRender.Renderer, SCREEN_WIDTH, SCREEN_HEIGHT) < 0)
    {
        fprintf(stderr, "%s:%d: SDL_RenderSetLogicalSize failed: %s\n",
                __FILE__, __LINE__, SDL_GetError());
        return 1;
    }

    GlobalGameRender.Screen = SDL_CreateTexture(
        GlobalGameRender.Renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!GlobalGameRender.Screen)
    {
        fprintf(stderr, "%s:%d: SDL_CreateTexture failed: %s\n", __FILE__,
                __LINE__, SDL_GetError());
        return 1;
    }

    GlobalGameRender.Pitch = SCREEN_WIDTH * sizeof(u32);

    if (!IMG_Init(IMG_INIT_PNG))
    {
        fprintf(stderr, "%s:%d: IMG_Init failed: %s\n", __FILE__, __LINE__,
                IMG_GetError());
        return 1;
    }

    GlobalConsole = C_ConsoleInit();
    if (!GlobalConsole)
    {
        fprintf(stderr, "%s:%d: C_ConsoleInit failed", __FILE__, __LINE__);
        return 1;
    }

    if (C_ConsoleSetBitmapFont(GlobalConsole, "images/terminal16x16.png"))
    {
        fprintf(stderr, "%s:%d: C_ConsoleSetBitmapFont failed", __FILE__,
                __LINE__);
        return 1;
    }

    return 0;
}

// == ECS ===============================

global_variable ECS_Entity GlobalEntityArray[ECS_ENTITIES_MAX];
global_variable u32 GlobalEntityIDAvailable = 0;
global_variable u32 GlobalEntityCount = 0;

uchar ECS_EntityGetGlyph(ECS_EntityType Type)
{
    switch (Type)
    {
    case ECS_ENTITY_PLAYER:
        return '@';
    case ECS_ENTITY_WALL:
        return '#';
    default:
        return 0;
    }
}

// returns EntityID or ECS_ENTITIES_MAX if no slots available
u32 ECS_EntityAdd(ECS_EntityType Type, u32 Row, u32 Col, u32 Color)
{
    if (GlobalEntityCount == ECS_ENTITIES_MAX)
        return ECS_ENTITIES_MAX;

    int EntityID = GlobalEntityIDAvailable;
    GlobalEntityArray[EntityID].Active = 1;
    GlobalEntityArray[EntityID].Type = Type;
    GlobalEntityArray[EntityID].Row = Row;
    GlobalEntityArray[EntityID].Col = Col;
    GlobalEntityArray[EntityID].Color = Color;
    GlobalEntityCount++;

    // Set new AvailableEntityID
    if (GlobalEntityCount < ECS_ENTITIES_MAX)
    {
        while (GlobalEntityArray[GlobalEntityIDAvailable].Active)
        {
            GlobalEntityIDAvailable =
                (GlobalEntityIDAvailable + 1) % ECS_ENTITIES_MAX;
        }
    }

    return EntityID;
}
void ECS_DisableEntity(int EntityID)
{
    GlobalEntityArray[EntityID].Active = 0;
    GlobalEntityIDAvailable = EntityID;
    GlobalEntityCount--;
}

void ECS_Init(void)
{
    for (int i = 0; i < ECS_ENTITIES_MAX; i++)
    {
        GlobalEntityArray[i].Active = 0;
    }
}

void ECS_EntityMoveBy(u32 EntityID, int RowChange, int ColChange)
{
    ECS_Entity Entity = GlobalEntityArray[EntityID];

    if (Entity.Row == 0 && RowChange < 0)
        return;
    if (Entity.Col == 0 && ColChange < 0)
        return;

    u32 Row = Entity.Row + RowChange;
    u32 Col = Entity.Col + ColChange;

    if (Row >= C_ROWS || Col >= C_COLS)
        return;

    if (GlobalMap[Row][Col] == '#')
        return;

    for (u32 SearchEntityID = 0; SearchEntityID < GlobalEntityCount;
         SearchEntityID++)
    {
        if (SearchEntityID == EntityID)
            continue;
        ECS_Entity *SearchEntity = &GlobalEntityArray[SearchEntityID];
        if (!SearchEntity->Active)
            continue;
        if (SearchEntity->Row == Row && SearchEntity->Col == Col)
        {
            if (SearchEntity->Type == ECS_ENTITY_WALL)
                return;
            break;
        }
    }

    GlobalEntityArray[EntityID].Row = Row;
    GlobalEntityArray[EntityID].Col = Col;
}

// ======================================

// == MAP ===============================

void map_generate(void)
{
    memset(&GlobalMap, '#', sizeof GlobalMap);

    int rooms_total = 10;
    int room_min_width = 5;
    int room_min_height = 3;
    int room_max_width = 20;
    int room_max_height = 15;
    C_Rect rooms[rooms_total];

    int room_index = 0;
    while (room_index < rooms_total)
    {
        u32 r1w = rand() % (room_max_width - room_min_width + 1) + room_min_width;
        u32 r1h = rand() % (room_max_height - room_min_height + 1) + room_min_height;
        u32 r1x = rand() % (C_COLS - r1w - 2) + 1;
        u32 r1y = rand() % (C_ROWS - r1h - 2) + 1;

        int flag_collides = 0;
        // check collisions
        for (int room_check_index = 0; room_check_index < room_index; room_check_index++)
        {
            u32 r2w = rooms[room_check_index].w;
            u32 r2h = rooms[room_check_index].h;
            u32 r2x = rooms[room_check_index].x;
            u32 r2y = rooms[room_check_index].y;

            // Box collision
            // https://jeffreythompson.org/collision-detection/rect-rect.php
            if (r1x + r1w >= r2x && // r1 right edge past r2 left
                r1x <= r2x + r2w && // r1 left edge past r2 right
                r1y + r1h >= r2y && // r1 top edge past r2 bottom
                r1y <= r2y + r2h)   // r1 bottom edge past r2 top
            {
                flag_collides = 1;
                break;
            }
        }

        if (!flag_collides)
        {
            rooms[room_index].w = r1w;
            rooms[room_index].h = r1h;
            rooms[room_index].x = r1x;
            rooms[room_index].y = r1y;
            room_index++;
        }
    }

    for (room_index = 0; room_index < rooms_total; room_index++)
    {
        u32 w = rooms[room_index].w;
        u32 h = rooms[room_index].h;
        u32 x = rooms[room_index].x;
        u32 y = rooms[room_index].y;
        for (u32 Row = y; Row < y + h; Row++)
        {
            for (u32 Col = x; Col < x + w; Col++)
            {
                GlobalMap[Row][Col] = ' ';
            }
        }
    }
}

void map_draw(void)
{
    for (int Row = 0; Row < C_ROWS; Row++)
    {
        for (int Col = 0; Col < C_COLS; Col++)
        {
            C_ConsolePutCharAt(GlobalConsole, GlobalMap[Row][Col], Row, Col, COLOR_WHITE);
        }
    }
}

// ======================================

int main(void)
{
    if (dark_init())
    {
        fprintf(stderr, "%s:%d: Init failed: %s\n", __FILE__, __LINE__,
                SDL_GetError());
        dark_exit(1);
    }

    srand(time(0));

    map_generate();

    SDL_Event Event;
    int running = 1;
    // pick starting player position
    u32 PlayerStartRow;
    u32 PlayerStartCol;
    while (1)
    {
        PlayerStartRow = rand() % C_ROWS;
        PlayerStartCol = rand() % C_COLS;
        if (GlobalMap[PlayerStartRow][PlayerStartCol] == ' ')
            break;
    }
    u32 PlayerID = ECS_EntityAdd(ECS_ENTITY_PLAYER, PlayerStartRow, PlayerStartCol, COLOR_GREEN);

    while (running)
    {

        while (SDL_PollEvent(&Event))
        {

            if (Event.type == SDL_QUIT)
            {
                dark_exit(0);
            }

            if (Event.type == SDL_KEYDOWN)
            {

                switch (Event.key.keysym.sym)
                {

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

                default:
                    break;
                }
            }
        }

        C_ConsoleClear(GlobalConsole);

        map_draw();

        // Draw Entities
        ECS_Entity *Entity;
        for (u32 EntityID = 0; EntityID < GlobalEntityCount; EntityID++)
        {
            Entity = &GlobalEntityArray[EntityID];
            C_ConsolePutCharAt(GlobalConsole, ECS_EntityGetGlyph(Entity->Type), Entity->Row, Entity->Col, Entity->Color);
        }

        SDL_UpdateTexture(GlobalGameRender.Screen, 0, GlobalConsole->Pixels, GlobalConsole->Pitch);
        SDL_RenderClear(GlobalGameRender.Renderer);
        SDL_RenderCopy(GlobalGameRender.Renderer, GlobalGameRender.Screen, 0, 0);
        SDL_RenderPresent(GlobalGameRender.Renderer);

        SDL_Delay(16);
    }

    dark_cleanup();
    return 0;
}
