#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <stdint.h>
#include <stdio.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef float f32;
typedef double f64;

int
main(void)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Could not init SDL: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Window *window = SDL_CreateWindow(
        "MyGame",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_OPENGL);

    if (!window)
    {
        printf("Failed to init window: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_GL_CreateContext(window);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        printf("Failed to load GL: %s\n", SDL_GetError());
        exit(1);
    }

    puts("OpenGL Loaded");
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n", glGetString(GL_VERSION));

    puts("Hello there!");

    u8 should_quit = 0;

    while (!should_quit)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                should_quit = 1;
                break;
            default:
                break;
            }
        }
    }
}
