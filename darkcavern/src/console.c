#include "console.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <stdio.h>
#include <stdlib.h>

// Returns pointer to a PT_Console on success
// or 0 (NULL) on error
PT_Console* PT_ConsoleInit(u32 Width, u32 Height, 
                           u32 Cols, u32 Rows) {
    // Width must be a multiple of Cols
    if (Width % Cols != 0) {
        fprintf(stderr,"%s:%d: Error in PT_ConsoleInit: Width must be a multiple of Cols\n",
                __FILE__, __LINE__);
        return 0;
    }

    // Height must be a multiple of Rows
    if (Height % Rows != 0) {
        fprintf(stderr,"%s:%d: Error in PT_ConsoleInit: Height must be a multiple of Rows\n",
                __FILE__, __LINE__);
        return 0;
    }
    
    PT_Console *Console = malloc(sizeof(PT_Console));

    Console->Pixels = calloc(Width * Height, sizeof(u32));
    Console->Width = Width;
    Console->Height = Height;
    Console->Rows = Rows;
    Console->Cols = Cols;
    Console->CellWidth = Width / Cols;
    Console->CellHeight = Height / Rows;
    Console->Font = NULL;
    Console->Cells = calloc(Rows * Cols, sizeof(PT_Cell));

    return Console;
}


// returns 0 on success or 1 otherwise
int PT_ConsoleSetBitmapFont(PT_Console *Console, const char *File, 
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

    PT_Font *Font = malloc(sizeof(PT_Font));
    if (!Font) {
        fprintf(stderr,"%s:%d: malloc failed\n",
                __FILE__, __LINE__);
        return 1;
    }

    // Copy the image data so we can hold onto it
    u32 ImgSize = (u32) (ConvertedFontImage->h * ConvertedFontImage->w) * sizeof(u32);
    Font->Atlas = malloc(ImgSize);
    if (!Font->Atlas) {
        fprintf(stderr,"%s:%d: malloc failed\n",
                __FILE__, __LINE__);
        return 1;
    }
    memcpy(Font->Atlas, ConvertedFontImage->pixels, ImgSize);

    // Create and configure the font
    Font->CharWidth = CharWidth;
    Font->CharHeight = CharHeight;
    Font->AtlasWidth = (u32) ConvertedFontImage->w;
    Font->AtlasHeight = (u32) ConvertedFontImage->h;
    Font->FirstCharInAtlas = FirstCharInAtlas;    

    SDL_FreeSurface(ConvertedFontImage);

    if (Console->Font != NULL) {
        free(Console->Font->Atlas);
        free(Console->Font);
    }

    Console->Font = Font;
    return 0;
}


