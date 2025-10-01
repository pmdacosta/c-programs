/* dark.c */
#include "dark.h"
#include <stdio.h>
#include <stdlib.h>


// === CONSOLE ==========================

// Returns pointer to a C_Console on success
// or 0 (NULL) on error
internal C_Console *C_ConsoleInit(void)
{
    C_Console *Console = malloc(sizeof(C_Console));

    Console->Pixels = calloc((u32)SCREEN_WIDTH * SCREEN_HEIGHT, sizeof(u32));
    if (!Console->Pixels)
    {
        fprintf(stderr, "%s:%d: calloc failed\n", __FILE__, __LINE__);
        return 0;
    }
    Console->Pitch = SCREEN_WIDTH * sizeof(u32);
    Console->Font = NULL;
    C_Rect Rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    Console->Rect = Rect;

    return Console;
}

internal void C_ConsoleFree(C_Console *Console)
{
    if (Console)
    {
        if (Console->Font)
        {
            free(Console->Font->Pixels);
            free(Console->Font);
        }
        free(Console->Pixels);
        free(Console);
    }
}

// returns 0 on success or 1 otherwise
internal int C_ConsoleSetBitmapFont(C_Console *Console, const char *File)
{
    // Load the image data
    SDL_Surface *FontImage = IMG_Load(File);
    if (!FontImage)
    {
        fprintf(stderr, "%s:%d: IMG_Load failed: %s\n", __FILE__, __LINE__,
                IMG_GetError());
        return 1;
    }

    SDL_PixelFormat *Format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
    SDL_Surface *ConvertedFontImage = SDL_ConvertSurface(FontImage, Format, 0);
    SDL_FreeSurface(FontImage);
    SDL_FreeFormat(Format);

    C_Font *Font = malloc(sizeof(C_Font));
    if (!Font)
    {
        fprintf(stderr, "%s:%d: malloc failed\n", __FILE__, __LINE__);
        return 1;
    }

    // Copy the image data so we can hold onto it
    u32 ImgSize =
        (u32)(ConvertedFontImage->h * ConvertedFontImage->w) * sizeof(u32);
    Font->Pixels = malloc(ImgSize);
    if (!Font->Pixels)
    {
        fprintf(stderr, "%s:%d: malloc failed\n", __FILE__, __LINE__);
        return 1;
    }
    memcpy(Font->Pixels, ConvertedFontImage->pixels, ImgSize);

    // Create and configure the font
    Font->BitmapWidth = (u32)ConvertedFontImage->w;
    Font->BitmapHeight = (u32)ConvertedFontImage->h;
    Font->FirstCharInBitmap = 0;
    Font->Pitch = Font->BitmapWidth * sizeof(u32);

    SDL_FreeSurface(ConvertedFontImage);

    if (Console->Font != NULL)
    {
        free(Console->Font->Pixels);
        free(Console->Font);
    }

    Console->Font = Font;
    return 0;
}

internal void C_ConsoleFillRect(u32 *Pixels, u32 Pitch, C_Rect *DestRect, u32 SourceColor)
{
    u32 rightX = DestRect->x + DestRect->w;
    u32 bottomY = DestRect->y + DestRect->h;

    u8 *Row = (u8 *)Pixels;
    Row += Pitch * DestRect->y;
    for (u32 y = DestRect->y; y < bottomY; y++)
    {
        u32 *Pixel = (u32 *)(Row + DestRect->x * sizeof(u32));
        for (u32 x = DestRect->x; x < rightX; x++)
        {
            *Pixel = SourceColor;
            Pixel++;
        }
        Row += Pitch;
    }
}

internal void C_ConsoleClear(C_Console *Console)
{
    C_ConsoleFillRect(Console->Pixels, Console->Pitch, &Console->Rect,
                      COLOR_BLACK);
}

internal C_Rect C_FontGetGlyphRect(C_Font *Font, uchar Glyph)
{
    u32 Index = Glyph - Font->FirstCharInBitmap;
    u32 CharsPerRow = (Font->BitmapWidth / CELL_WIDTH);
    u32 xOffset = (Index % CharsPerRow) * CELL_WIDTH;
    u32 yOffset = (Index / CharsPerRow) * CELL_HEIGHT;

    C_Rect glyphRect = {xOffset, yOffset, CELL_WIDTH, CELL_HEIGHT};
    return glyphRect;
}

internal void C_ConsolePutCharAt(C_Console *Console, uchar Glyph, u32 Row, u32 Col,
                        u32 FGColor)
{

    u32 x = Col * CELL_HEIGHT;
    u32 y = Row * CELL_WIDTH;
    C_Rect ConsoleRect = {x, y, CELL_WIDTH, CELL_HEIGHT};

    C_Rect AtlasRect = C_FontGetGlyphRect(Console->Font, Glyph);

    u32 ConsoleRightX = ConsoleRect.x + ConsoleRect.w;
    u32 ConsoleBottomY = ConsoleRect.y + ConsoleRect.h;

    u8 *AtlasRow = (u8 *)Console->Font->Pixels;
    u8 *ConsoleRow = (u8 *)Console->Pixels;
    AtlasRow += Console->Font->Pitch * AtlasRect.y;
    ConsoleRow += Console->Pitch * ConsoleRect.y;

    for (u32 ConsoleY = ConsoleRect.y, AtlasY = AtlasRect.y;
         ConsoleY < ConsoleBottomY; ConsoleY++, AtlasY++)
    {

        u32 *AtlasPixel = (u32 *)(AtlasRow + AtlasRect.x * sizeof(u32));
        u32 *ConsolePixel = (u32 *)(ConsoleRow + ConsoleRect.x * sizeof(u32));

        for (u32 ConsoleX = ConsoleRect.x, AtlasX = AtlasRect.x;
             ConsoleX < ConsoleRightX;
             ConsoleX++, AtlasX++, AtlasPixel++, ConsolePixel++)
        {

            if (*AtlasPixel != TRANSPARENT_PIXEL)
            {
                *ConsolePixel = FGColor;
            }
        }

        AtlasRow += Console->Font->Pitch;
        ConsoleRow += Console->Pitch;
    }
}

internal void C_Debug_PrintAtlas(C_Console *Console)
{

    u8 *ConsoleRow = (u8 *)Console->Pixels;
    u8 *AtlasRow = (u8 *)Console->Font->Pixels;
    for (u32 Y = 0; Y < Console->Font->BitmapHeight; Y++)
    {
        u32 *ConsolePixel = (u32 *)ConsoleRow;
        u32 *AtlasPixel = (u32 *)AtlasRow;
        for (u32 X = 0; X < Console->Font->BitmapWidth; X++)
        {
            *ConsolePixel = *AtlasPixel;
            ConsolePixel++;
            AtlasPixel++;
        }
        ConsoleRow += Console->Pitch;
        AtlasRow += Console->Font->Pitch;
    }
}


internal void C_Debug_DrawGradient(C_Console *Console, int xOffset, int yOffset)
{
    u8 *Row = (u8 *)Console->Pixels;
    for (u32 y = 0; y < SCREEN_HEIGHT; y++)
    {
        u32 *Pixel = (u32 *)Row;
        for (u32 x = 0; x < SCREEN_WIDTH; x++)
        {
            u8 Red = (u8)(x + xOffset);
            u8 Green = (u8)(y + yOffset);
            *Pixel = (u32)COLOR(Red, Green, 0, 0);
            Pixel++;
        }
        Row += Console->Pitch;
    }
}

// ======================================

// == DARK ==============================

global_variable GameRender GlobalGameRender;
global_variable C_Console *GlobalConsole;

internal void dark_cleanup(void)
{
    C_ConsoleFree(GlobalConsole);
    SDL_DestroyTexture(GlobalGameRender.Screen);
    SDL_DestroyRenderer(GlobalGameRender.Renderer);
    SDL_DestroyWindow(GlobalGameRender.Window);
    IMG_Quit();
    SDL_Quit();
}

internal void dark_exit(int status)
{
    dark_cleanup();
    exit(status);
}

// Returns 0 on success, 1 otherwise
internal int dark_init(void)
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

// ======================================

// == MAP ===============================

global_variable uchar GlobalMap[MAP_ROWS][MAP_COLS];

internal void map_generate(void)
{
    memset(&GlobalMap, '#', sizeof GlobalMap);

    int rooms_total = 20;
    int room_min_width = 5;
    int room_min_height = 3;
    int room_max_width = 30;
    int room_max_height = 20;
    C_Rect rooms[rooms_total];

    int room_index = 0;
    while (room_index < rooms_total)
    {
        u32 r1w = rand() % (room_max_width - room_min_width + 1) + room_min_width;
        u32 r1h = rand() % (room_max_height - room_min_height + 1) + room_min_height;
        u32 r1x = rand() % (MAP_COLS - r1w - 2) + 1;
        u32 r1y = rand() % (MAP_ROWS - r1h - 2) + 1;

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

    for (int i = 0; i < rooms_total - 1; i++)
    {
        int x1 = (rand() % rooms[i].w) + rooms[i].x;
        int x2 = (rand() % rooms[i + 1].w) + rooms[i + 1].x;
        int y1 = (rand() % rooms[i].h) + rooms[i].y;
        int y2 = (rand() % rooms[i + 1].h) + rooms[i + 1].y;

        if (rand() % 2) {

            int x_direction = x1 < x2 ? 1 : -1;
            while (x1 != x2)
            {
                GlobalMap[y1][x1] = ' ';
                x1 += x_direction;
            }

            int y_direction = y1 < y2 ? 1 : -1;
            while (y1 != y2)
            {
                GlobalMap[y1][x1] = ' ';
                y1 += y_direction;
            }
        } else {
            int y_direction = y1 < y2 ? 1 : -1;
            while (y1 != y2)
            {
                GlobalMap[y1][x1] = ' ';
                y1 += y_direction;
            }

            int x_direction = x1 < x2 ? 1 : -1;
            while (x1 != x2)
            {
                GlobalMap[y1][x1] = ' ';
                x1 += x_direction;
            }
        }
    }
}

internal  void map_draw(void)
{
    for (int Row = 0; Row < MAP_ROWS; Row++)
    {
        for (int Col = 0; Col < MAP_COLS; Col++)
        {
            C_ConsolePutCharAt(GlobalConsole, GlobalMap[Row][Col], Row, Col, COLOR_WALL);
        }
    }
}

// ======================================

// == ECS ===============================

global_variable ECS_Entity GlobalEntityArray[ECS_ENTITIES_MAX];
global_variable u32 GlobalEntityIDAvailable = 0;
global_variable u32 GlobalEntityCount = 0;

internal  uchar ECS_EntityGetGlyph(ECS_EntityType Type)
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
internal  u32 ECS_EntityAdd(ECS_EntityType Type, u32 Row, u32 Col, u32 Color)
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

internal void ECS_DisableEntity(int EntityID)
{
    GlobalEntityArray[EntityID].Active = 0;
    GlobalEntityIDAvailable = EntityID;
    GlobalEntityCount--;
}

internal void ECS_Init(void)
{
    for (int i = 0; i < ECS_ENTITIES_MAX; i++)
    {
        GlobalEntityArray[i].Active = 0;
    }
}

internal void ECS_EntityMoveBy(u32 EntityID, int RowChange, int ColChange)
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

