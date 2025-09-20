#ifndef CONSOLE
#define CONSOLE

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "types.h"

// COLOR HELPERS
// Get individual color values from a 32bit color pixel
#define RED(c) ((c & 0xFF000000) >> 24)
#define GREEN(c) ((c & 0x00FF0000) >> 16)
#define BLUE(c) ((c & 0x0000FF00) >> 8)
#define ALPHA(c) (c & 0x000000FF)

// Get a color pixel from individual colors
#define COLOR(r, g, b, a) ((r << 24) | (g << 16) | (b << 8) | a)

// Pallette
#define COLOR_BLACK  COLOR(0x00, 0x00, 0x00, 0xFF)
#define COLOR_WHITE  COLOR(0xFF, 0xFF, 0xFF, 0xFF)
#define COLOR_RED    COLOR(0xFF, 0x00, 0x00, 0xFF)
#define COLOR_GREEN  COLOR(0x00, 0xFF, 0x00, 0xFF)
#define COLOR_BLUE   COLOR(0x00, 0x00, 0xFF, 0xFF)
#define COLOR_YELLOW COLOR(0xFF, 0xFF, 0x00, 0xFF)
#define COLOR_PURPLE COLOR(0x80, 0x00, 0x80, 0xFF)

// ---------- main.c ----------
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

void dark_cleanup(void);
int dark_init(void);

// ---------- console.c ----------
#define CELL_WIDTH 16
#define CELL_HEIGHT 16

typedef struct {
    uchar Glyph;
    u32 FGColor;
    u32 BGColor;
} C_Cell;

typedef struct {
    u32 *Pixels;
    u32 AtlasWidth;
    u32 AtlasHeight;
    u32 CharWidth;
    u32 CharHeight;
    u32 Pitch;
    uchar FirstCharInAtlas;
    u32 TransparentColor;
} C_Font;

typedef struct {
    u32 x;
    u32 y;
    u32 w;
    u32 h;
} C_Rect;

typedef struct {
    u32 x;
    u32 y;
} C_Position;

typedef struct {
    uchar Glyph;
    C_Position Position;
} C_Player;

typedef struct {
    u32 *Pixels;      // the screen pixels
    u32 Pitch;
    u32 Width;
    u32 Height;
    u32 Rows;
    u32 Cols;
    u32 CellWidth;
    u32 CellHeight;
    C_Rect Rect;
    C_Font* Font;
    C_Cell* Cells;
    C_Player* Player;
} C_Console;

C_Console* C_ConsoleInit(u32 Width, u32 Height, 
                           u32 Cols, u32 Rows);

int C_ConsoleSetBitmapFont(C_Console *Console, const char *File, 
                        u32 CharWidth, u32 CharHeight);

void C_ConsoleClear(C_Console *Console);

void C_ConsolePutCharAt(C_Console *Console, uchar Glyph, 
                    u32 CellX, u32 CellY,
                    u32 FGColor);

void C_FillRect(u32* Pixels, u32 Pitch, C_Rect *DestRect, u32 SourceColor);

C_Rect C_RectForGlyph(uchar Glyph, C_Font *Font);

void C_Debug_PrintAtlas(C_Console* Console);

void C_Debug_DrawGradient(C_Console* Console, int xOffset, int yOffset);

#endif
