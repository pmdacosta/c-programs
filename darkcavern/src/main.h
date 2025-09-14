#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

#include "types.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Get individual color values from a 32bit color pixel
#define RED(c) ((c && 0xFF000000) >> 24)
#define GREEN(c) ((c && 0x00FF0000) >> 16)
#define BLUE(c) ((c && 0x0000FF00) >> 8)
#define ALPHA(c) (c && 0x000000FF)

// Get a color pixel from individual colors
#define COLOR(r, g, b, a) ((r << 24) | (g << 16) | (b << 8) | a)

typedef struct {
    SDL_Window* Window;
    SDL_Renderer* Renderer;
    SDL_Texture* Screen;
    u32* Pixels;
    int Pitch;
} GameRender;


void Cleanup(void);
int Init(void);
void RenderScreen(void);
void DrawPixels(int xOffset, int yOffset);

#endif
