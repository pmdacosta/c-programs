#ifndef CONSOLE
#define CONSOLE

#include <SDL2/SDL.h>
#include "types.h"

#define CELL_WIDTH 16
#define CELL_HEIGHT 16

// Get individual color values from a 32bit color pixel
#define RED(c) ((c & 0xFF000000) >> 24)
#define GREEN(c) ((c & 0x00FF0000) >> 16)
#define BLUE(c) ((c & 0x0000FF00) >> 8)
#define ALPHA(c) (c & 0x000000FF)

// Get a color pixel from individual colors
#define COLOR(r, g, b, a) ((r << 24) | (g << 16) | (b << 8) | a)

// Pallette
#define COLOR_BLACK COLOR(0xFF, 0xFF, 0xFF, 0xFF)
#define COLOR_WHITE COLOR(0x00, 0x00, 0x00, 0xFF)

typedef struct {
    uchar Glyph;
    u32 FGColor;
    u32 BGColor;
} C_Cell;

typedef struct {
    u32 *Atlas;
    u32 AtlasWidth;
    u32 AtlasHeight;
    u32 CharWidth;
    u32 CharHeight;
    uchar FirstCharInAtlas;
} C_Font;

typedef struct {
    u32 *Pixels;      // the screen pixels
    u32 Pitch;
    u32 Width;
    u32 Height;
    u32 Rows;
    u32 Cols;
    u32 CellWidth;
    u32 CellHeight;
    C_Font *Font;
    C_Cell *Cells;
} C_Console;


C_Console* C_ConsoleInit(u32 Width, u32 Height, 
                           u32 Cols, u32 Rows);

int C_ConsoleSetBitmapFont(C_Console *Console, const char *File, 
                        uchar FirstCharInAtlas,
                        u32 CharWidth, u32 CharHeight);

void C_ConsoleClear(C_Console *Console);

void C_Fill(u32 *Pixels, u32 PixelsPerRow, SDL_Rect *DestRect, u32 Color);

int C_ConsolePutCharAt(C_Console *Console, uchar Glyph, 
                    i32 CellX, i32 CellY,
                    u32 FGColor, u32 BGColor);

void C_FillRectWithAlphaBlend(u32 *Pixels, u32 Pitch, SDL_Rect *DestRect, u32 SourceColor);

int C_CopyBlend(u32 *DestPixels, SDL_Rect *DestRect, u32 DestPitch,
             u32 *SrcPixels, SDL_Rect *SrcRect, u32 SrcPitch,
             u32 NewColor);

SDL_Rect C_RectForGlyph(uchar Glyph, C_Font *Font);

inline u32 C_ColorizePixel(u32 dest, u32 src); 

#endif
