#include "console.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: change order of functions to match header file

// Returns pointer to a C_Console on success
// or 0 (NULL) on error
C_Console* C_ConsoleInit(u32 Width, u32 Height, 
                           u32 Cols, u32 Rows) {
    // Width must be a multiple of Cols
    if (Width % Cols != 0) {
        fprintf(stderr,"%s:%d: Error in C_ConsoleInit: Width must be a multiple of Cols\n",
                __FILE__, __LINE__);
        return 0;
    }

    // Height must be a multiple of Rows
    if (Height % Rows != 0) {
        fprintf(stderr,"%s:%d: Error in C_ConsoleInit: Height must be a multiple of Rows\n",
                __FILE__, __LINE__);
        return 0;
    }
    
    C_Console *Console = malloc(sizeof(C_Console));

    Console->Pixels = calloc((u32) Width * Height, sizeof(u32));
    if (!Console->Pixels) {
        fprintf(stderr,"%s:%d: calloc failed\n",
                __FILE__, __LINE__);
        return 0;
    }
    Console->Pitch = Width * sizeof(u32);
    Console->Width = Width;
    Console->Height = Height;
    Console->Rows = Rows;
    Console->Cols = Cols;
    Console->CellWidth = Width / Cols;
    Console->CellHeight = Height / Rows;
    Console->Font = NULL;
    Console->Cells = calloc(Rows * Cols, sizeof(C_Cell));
    if (!Console->Cells) {
        fprintf(stderr,"%s:%d: calloc failed\n",
                __FILE__, __LINE__);
        return 0;
    }
    C_Rect Rect = {0, 0, Width, Height};
    Console->Rect = Rect;

    Console->Player = malloc(sizeof(C_Player));
    if (!Console->Player) {
        fprintf(stderr,"%s:%d: malloc failed\n",
                __FILE__, __LINE__);
        return 0;
    }
    Console->Player->Glyph = '@';
    Console->Player->Position.x = 10;
    Console->Player->Position.y = 10;

    return Console;
}


// returns 0 on success or 1 otherwise
int C_ConsoleSetBitmapFont(C_Console *Console, const char *File, 
                        u32 CharWidth, u32 CharHeight) {

    // Load the image data
    SDL_Surface* FontImage = IMG_Load(File);
    if (!FontImage) {
        fprintf(stderr,"%s:%d: IMG_Load failed: %s\n",
                __FILE__, __LINE__,IMG_GetError());
        return 1;
    }

    SDL_PixelFormat* Format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
    SDL_Surface* ConvertedFontImage = SDL_ConvertSurface(FontImage, Format, 0);
    SDL_FreeSurface(FontImage);
    SDL_FreeFormat(Format);

    C_Font *Font = malloc(sizeof(C_Font));
    if (!Font) {
        fprintf(stderr,"%s:%d: malloc failed\n",
                __FILE__, __LINE__);
        return 1;
    }

    // Copy the image data so we can hold onto it
    u32 ImgSize = (u32) (ConvertedFontImage->h * ConvertedFontImage->w) * sizeof(u32);
    Font->Pixels = malloc(ImgSize);
    if (!Font->Pixels) {
        fprintf(stderr,"%s:%d: malloc failed\n",
                __FILE__, __LINE__);
        return 1;
    }
    memcpy(Font->Pixels, ConvertedFontImage->pixels, ImgSize);

    // Create and configure the font
    Font->CharWidth = CharWidth;
    Font->CharHeight = CharHeight;
    Font->AtlasWidth = (u32) ConvertedFontImage->w;
    Font->AtlasHeight = (u32) ConvertedFontImage->h;
    Font->FirstCharInAtlas = 0;
    Font->Pitch = Font->AtlasWidth * sizeof(u32);
    Font->TransparentColor = COLOR_BLACK;

    SDL_FreeSurface(ConvertedFontImage);

    if (Console->Font != NULL) {
        free(Console->Font->Pixels);
        free(Console->Font);
    }

    Console->Font = Font;
    return 0;
}

void C_ConsoleClear(C_Console *Console) {
    C_FillRect(Console->Pixels, Console->Pitch, &Console->Rect, COLOR_BLACK);
}

void C_FillRect(u32* Pixels, u32 Pitch, C_Rect *DestRect, u32 SourceColor)
{
    u32 rightX = DestRect->x + DestRect->w;
    u32 bottomY = DestRect->y + DestRect->h;

    u8* Row = (u8*) Pixels;
    Row += Pitch * DestRect->y;
    for (u32 y = DestRect->y; y < bottomY; y++) {
        u32* Pixel = (u32 *) (Row + DestRect->x * sizeof(u32));
        for (u32 x = DestRect ->x; x < rightX; x++) {
            *Pixel = SourceColor;
            Pixel++;
        }
        Row += Pitch;
    }
}

void C_ConsolePutCharAt(C_Console *Console, uchar Glyph, 
        u32 CellX, u32 CellY,
        u32 FGColor) {

    u32 x = CellX * Console->CellWidth;
    u32 y = CellY * Console->CellHeight;
    C_Rect ConsoleRect = {x, y, Console->CellWidth, Console->CellHeight};

    C_Rect AtlasRect = C_RectForGlyph(Glyph, Console->Font);

    u32 ConsoleRightX = ConsoleRect.x + ConsoleRect.w;
    u32 ConsoleBottomY = ConsoleRect.y + ConsoleRect.h;

    u8* AtlasRow = (u8*) Console->Font->Pixels;
    u8* ConsoleRow = (u8*) Console->Pixels;
    AtlasRow += Console->Font->Pitch * AtlasRect.y;
    ConsoleRow += Console->Pitch * ConsoleRect.y;

    for (u32 ConsoleY = ConsoleRect.y, AtlasY = AtlasRect.y; 
            ConsoleY < ConsoleBottomY; 
            ConsoleY++, AtlasY++) {

        u32 *AtlasPixel = (u32*) (AtlasRow + AtlasRect.x * sizeof(u32));
        u32 *ConsolePixel = (u32*) (ConsoleRow + ConsoleRect.x * sizeof(u32));

        for (u32 ConsoleX = ConsoleRect.x, AtlasX = AtlasRect.x; 
                ConsoleX < ConsoleRightX; 
                ConsoleX++, AtlasX++,
                AtlasPixel++, ConsolePixel++) {

            if (*AtlasPixel != Console->Font->TransparentColor) {
                *ConsolePixel = FGColor;
            }
        }

        AtlasRow += Console->Font->Pitch;
        ConsoleRow += Console->Pitch;
    }
}

void C_Debug_PrintAtlas(C_Console* Console) {

    u8* ConsoleRow = (u8*) Console->Pixels;
    u8* AtlasRow = (u8*) Console->Font->Pixels;
    for (u32 Y = 0; Y < Console->Font->AtlasHeight; Y++) {
        u32* ConsolePixel = (u32*) ConsoleRow;
        u32* AtlasPixel = (u32*) AtlasRow;
        for (u32 X = 0; X < Console->Font->AtlasWidth; X++) {
            *ConsolePixel = *AtlasPixel;
            ConsolePixel++;
            AtlasPixel++;
        }
        ConsoleRow += Console->Pitch;
        AtlasRow += Console->Font->Pitch;
    }
}

C_Rect C_RectForGlyph(uchar Glyph, C_Font *Font) {
    u32 Index = Glyph - Font->FirstCharInAtlas;
    u32 CharsPerRow = (Font->AtlasWidth / Font->CharWidth);
    u32 xOffset = (Index % CharsPerRow) * Font->CharWidth;
    u32 yOffset = (Index / CharsPerRow) * Font->CharHeight;

    C_Rect glyphRect = {xOffset, yOffset, Font->CharWidth, Font->CharHeight};
    return glyphRect;
}

void C_Debug_DrawGradient(C_Console* Console, int xOffset, int yOffset) {
    u8* Row = (u8*) Console->Pixels;
    for (u32 y = 0; y < Console->Height; y++) {
        u32* Pixel = (u32 *) Row;
        for (u32 x = 0; x < Console->Width; x++) {
            u8 Red = (u8) (x + xOffset);
            u8 Green = (u8) (y + yOffset);
            *Pixel = (u32) COLOR(Red, Green, 0, 0);
            Pixel++;
        }
        Row += Console->Pitch;
    }
}
