// On linux compile with:
// g++ -std=c++17 main.cpp glad/src/glad.c -I./glad/include -o prog -lSDL2 -ldl
// On windows compile with (if using mingw)
// g++ main.cpp ./glad/src/glad.c -I./glad/include -o prog.exe -lmingw32 -lSDL2main -lSDL2

// C++ Standard Libraries
// Third-party library
#include "SDL.h"
// Include GLAD
#include <glad/glad.h>

int main(int argc, char* argv[]){
    SDL_Init(0);

    return 0;
}
