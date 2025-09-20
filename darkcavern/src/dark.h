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
#define TRANSPARENT_PIXEL COLOR_BLACK

typedef struct {
    u32 *Pixels;
    u32 BitmapWidth;
    u32 BitmapHeight;
    u32 Pitch;
    uchar FirstCharInBitmap;
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
    C_Position Position;
    uchar Glyph;
} C_Player;

typedef struct {
    u32 *Pixels;      // the screen pixels
    u32 Pitch;
    u32 Rows;
    u32 Cols;
    C_Rect Rect;
    C_Font* Font;
    C_Player* Player;
} C_Console;

C_Console* C_ConsoleInit(void);

void C_ConsoleFree(C_Console* Console);

int C_ConsoleSetBitmapFont(C_Console *Console, const char *File);

void C_ConsoleClear(C_Console *Console);

void C_ConsolePutCharAt(C_Console *Console, uchar Glyph, 
                    u32 CellX, u32 CellY,
                    u32 FGColor);

void C_ConsoleFillRect(u32* Pixels, u32 Pitch, C_Rect *DestRect, u32 SourceColor);

C_Rect C_FontGetGlyphRect(C_Font *Font, uchar Glyph);

void C_Debug_PrintAtlas(C_Console* Console);

void C_Debug_DrawGradient(C_Console* Console, int xOffset, int yOffset);

#endif
