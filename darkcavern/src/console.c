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
    Console->Pitch = Width * sizeof(u32);
    Console->Width = Width;
    Console->Height = Height;
    Console->Rows = Rows;
    Console->Cols = Cols;
    Console->CellWidth = Width / Cols;
    Console->CellHeight = Height / Rows;
    Console->Font = NULL;
    Console->Cells = calloc(Rows * Cols, sizeof(C_Cell));
    C_Rect Rect = {0, 0, Width, Height};
    Console->Rect = Rect;

    return Console;
}


// returns 0 on success or 1 otherwise
int C_ConsoleSetBitmapFont(C_Console *Console, const char *File, 
                        uchar FirstCharInAtlas,
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
    Font->FirstCharInAtlas = FirstCharInAtlas;    
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
    C_FillRect(Console->Pixels, Console->Pitch, &Console->Rect, 0x000000FF);
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

int C_ConsolePutCharAt(C_Console *Console, uchar Glyph, 
        u32 CellX, u32 CellY,
        u32 FGColor) {

    u32 x = CellX * Console->CellWidth;
    u32 y = CellY * Console->CellHeight;
    C_Rect DestRect = {x, y, Console->CellWidth, Console->CellHeight};

    // Fill the background with alpha blending
    // C_FillRectWithAlphaBlend(Console->Pixels, Console->Pitch, &DestRect, BGColor);
    // C_FillRect(Console->Pixels, Console->Pitch, &DestRect, BGColor);

    // Copy the glyph with alpha blending and desired coloring
    C_Rect SrcRect = C_RectForGlyph(Glyph, Console->Font);
    int Result = C_CopyBlend(Console->Pixels, &DestRect, Console->Pitch, 
            Console->Font->Pixels, &SrcRect, Console->Font->Pitch,
            Console->Font->TransparentColor, FGColor);
    if (Result) {
        fprintf(stderr, "%s:%d: C_CopyBlend failed\n", __FILE__, __LINE__);
        return 1;
    }

    return 0;
}

// For each pixel in the destination rect, alpha blend the 
// background color to the existing color.
// ref: https://en.wikipedia.org/wiki/Alpha_compositing
void C_FillRectWithAlphaBlend(u32 *Pixels, u32 Pitch, C_Rect *DestRect, u32 SourceColor)
{
    u8 SourceAlphaByte = ALPHA(SourceColor);
    if (SourceAlphaByte == 0) return; // Source color is fully transparent -> nothing to do
    if (SourceAlphaByte == 255) {
        // Source color is fully opaque, no need to blend
        C_FillRect(Pixels, Pitch, DestRect, SourceColor);
    }

    u32 RightX = DestRect->x + DestRect->w;
    u32 BottomY = DestRect->y + DestRect->h;

    // Precompute source color channels and alpha terms
    float SourceAlpha = SourceAlphaByte / 255.0f;
    float InvSourceAlpha = 1.0f - SourceAlpha;
    u8 SourceR = RED(SourceColor);
    u8 SourceG = GREEN(SourceColor);
    u8 SourceB = BLUE(SourceColor);

    u8* Row = (u8*) Pixels;
    Row += Pitch * DestRect->y;

    for (u32 y = DestRect->y; y < BottomY; y++) {

        u32 *Pixel = (u32*) (Row + DestRect->x * sizeof(u32));

        for (u32 x = DestRect->x; x < RightX; x++) {
            // Do alpha blending
            u32 DestColor = *Pixel;

            float DestAlpha = ALPHA(DestColor) / 255.0f;
            float DestR = RED(DestColor)   * DestAlpha;
            float DestG = GREEN(DestColor) * DestAlpha;
            float DestB = BLUE(DestColor)  * DestAlpha;

            u8 OutR = (u8) (SourceR + DestR * InvSourceAlpha);
            u8 OutG = (u8) (SourceG + DestG * InvSourceAlpha);
            u8 OutB = (u8) (SourceB + DestB * InvSourceAlpha);
            u8 OutA = (u8) ((SourceAlpha + DestAlpha * InvSourceAlpha) * 255);

            *Pixel = COLOR(OutR, OutG, OutB, OutA);

            Pixel++;
        }

        Row += Pitch;
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

// returns 0 success, 1 otherwise
int C_CopyBlend(u32 *DestPixels, C_Rect *DestRect, u32 DestPitch,
        u32 *SrcPixels, C_Rect *SrcRect, u32 SrcPitch,
        u32 TransparentColor, u32 NewColor)
{
    // Src and Dest rect need to have the same dimensions
    if (DestRect->w != SrcRect->w || DestRect->h != SrcRect->h) {
        fprintf(stderr, "%s:%d: SrcRect and DestRect do not dimentions do not match\n",
                __FILE__, __LINE__);
        return 1;
    }

    // For each pixel in the destination rect, alpha blend to it the 
    // corresponding pixel in the source rect.
    // ref: https://en.wikipedia.org/wiki/Alpha_compositing

    u32 DestRightX = DestRect->x + DestRect->w;
    u32 DestBottomY = DestRect->y + DestRect->h;

    u8* SrcRow = (u8*) SrcPixels;
    u8* DestRow = (u8*) DestPixels;
    SrcRow += SrcPitch * SrcRect->y;
    DestRow += DestPitch * DestRect->y;

    for (u32 DestY = DestRect->y, SrcY = SrcRect->y; 
            DestY < DestBottomY; 
            DestY++, SrcY++) {

        u32 *SrcPixel = (u32*) (SrcRow + SrcRect->x * sizeof(u32));
        u32 *DestPixel = (u32*) (DestRow + DestRect->x * sizeof(u32));

        for (u32 DestX = DestRect->x, SrcX = SrcRect->x; 
                DestX < DestRightX; 
                DestX++, SrcX++,
                SrcPixel++, DestPixel++) {

            if (*SrcPixel == TransparentColor) continue; // The atlas pixel is the transparent color
                                                         // so we don't copy anything

            u32 SrcAlphaByte = ALPHA(NewColor);

            if (SrcAlphaByte == 0) continue; // Source is transparent - so do nothing

            if (SrcAlphaByte == 255) {
                // Src is fully opaque, just overwrite
                *DestPixel = NewColor;
                continue;
            }

            // Alpha Blending
            float SrcA = SrcAlphaByte / 255.0f;
            float InvSrcA = (1.0f - SrcA);

            // Premultiply the source color
            float PremultSrcR = RED(NewColor)   * SrcA;
            float PremultSrcG = GREEN(NewColor) * SrcA;
            float PremultSrcB = BLUE(NewColor)  * SrcA;

            // Premultiply the destination color
            u32 Existing = *DestPixel;
            float DestA = ALPHA(Existing) / 255.0f;
            float DestR = RED(Existing)   * DestA;
            float DestG = GREEN(Existing) * DestA;
            float DestB = BLUE(Existing)  * DestA;

            // Compute final blended colors
            float OutA = SrcA + DestA * InvSrcA;
            float OutR = PremultSrcR + DestR * InvSrcA;
            float OutG = PremultSrcG + DestG * InvSrcA;
            float OutB = PremultSrcB + DestB * InvSrcA;

            // Unpremultiply
            u8 FinalA = (u8) (OutA * 255);
            u8 FinalR = (u8) (OutR / OutA);
            u8 FinalG = (u8) (OutG / OutA);
            u8 FinalB = (u8) (OutB / OutA);

            *DestPixel = COLOR(FinalR, FinalG, FinalB, FinalA);
        }

        SrcRow += SrcPitch;
        DestRow += DestPitch;
    }

    return 0;
}

C_Rect C_RectForGlyph(uchar Glyph, C_Font *Font) {
    u32 Index = Glyph - Font->FirstCharInAtlas;
    u32 CharsPerRow = (Font->AtlasWidth / Font->CharWidth);
    u32 xOffset = (Index % CharsPerRow) * Font->CharWidth;
    u32 yOffset = (Index / CharsPerRow) * Font->CharHeight;

    C_Rect glyphRect = {xOffset, yOffset, Font->CharWidth, Font->CharHeight};
    return glyphRect;
}


// Colorize a pixel without blending the colors, only the alpha channel
u32 C_ColorizePixel(u32 TransparentColor, u32 AtlasColor, u32 NewColor) 
{
    if (AtlasColor == TransparentColor) {
        return AtlasColor;
    }
    return NewColor;
}
