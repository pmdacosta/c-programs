#ifndef CONSOLE
#define CONSOLE

#include "types.h"

#define CELL_WIDTH 16
#define CELL_HEIGHT 16

// Get individual color values from a 32bit color pixel
#define RED(c) ((c && 0xFF000000) >> 24)
#define GREEN(c) ((c && 0x00FF0000) >> 16)
#define BLUE(c) ((c && 0x0000FF00) >> 8)
#define ALPHA(c) (c && 0x000000FF)

// Get a color pixel from individual colors
#define COLOR(r, g, b, a) ((r << 24) | (g << 16) | (b << 8) | a)

typedef struct {
    uchar Glyph;
    u32 FGColor;
    u32 BGColor;
} PT_Cell;

typedef struct {
    u32 *Atlas;
    u32 AtlasWidth;
    u32 AtlasHeight;
    u32 CharWidth;
    u32 CharHeight;
    uchar FirstCharInAtlas;
} PT_Font;

typedef struct {
    u32 *Pixels;      // the screen pixels
    u32 Width;
    u32 Height;
    u32 Rows;
    u32 Cols;
    u32 CellWidth;
    u32 CellHeight;
    PT_Font *Font;
    PT_Cell *Cells;
} PT_Console;



PT_Console* PT_ConsoleInit(u32 Width, u32 Height, 
                           u32 Cols, u32 Rows);

int PT_ConsoleSetBitmapFont(PT_Console *Console, const char *File, 
                        uchar FirstCharInAtlas,
                        u32 CharWidth, u32 CharHeight);

#endif
