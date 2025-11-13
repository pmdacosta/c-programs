#include "SDL.h"
#include "glad/glad.h"
#include <stdio.h>

#define print_sdl_error(message) fprintf(stderr, "%s:%d: %s: %s\n", __FILE__, __LINE__, message, SDL_GetError())

int
main(void)
{
    SDL_Window *window;
    SDL_GLContext *context;
    Uint8 running;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL);
    if (!window)
    {
        print_sdl_error("SDL_CreateWindow failed");
        return 1;
    }

    context = SDL_GL_CreateContext(window);
    if (!context)
    {
        print_sdl_error("SDL_GL_CreateContext failed");
        return 1;
    }

    gladLoadGLLoader(SDL_GL_GetProcAddress);
    glViewport(0,0, 640, 480);

    running = 1;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
            {
                running = 0;
                goto quit;
                break;
            }
            case SDL_MOUSEMOTION:
            {
                break;
            }
            case SDL_KEYDOWN:
            {
                break;
            }
            default:
            {
                break;
            }
            }
        }

        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        SDL_GL_SwapWindow(window);
        SDL_Delay(16);
    }

quit:
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
