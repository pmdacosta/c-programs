#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int Init(void);
int LoadMedia(void);
void Cleanup(void);
SDL_Surface* LoadSurface(const char* file);

#endif
