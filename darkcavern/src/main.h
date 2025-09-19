#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

#include "types.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

typedef struct {
    SDL_Window* Window;
    SDL_Renderer* Renderer;
    SDL_Texture* Screen;
    int Pitch;
} GameRender;


void Cleanup(void);
int Init(void);
void RenderScreen(void);

#endif
