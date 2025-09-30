/* main.c */
#include <stdlib.h>
#include <time.h>

#include "dark.h"
#include "dark.c"

int main(void)
{
    if (dark_init())
    {
        fprintf(stderr, "%s:%d: Init failed: %s\n", __FILE__, __LINE__,
                SDL_GetError());
        dark_exit(1);
    }

    ECS_Init();

    srand((u32)time(0));

    map_generate();

    SDL_Event Event;
    int running = 1;
    // pick starting player position
    u32 PlayerStartRow;
    u32 PlayerStartCol;
    while (1)
    {
        PlayerStartRow = rand() % MAP_ROWS;
        PlayerStartCol = rand() % MAP_COLS;
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
