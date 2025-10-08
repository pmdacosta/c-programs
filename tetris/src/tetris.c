#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>

#define global_variable static
#define internal static

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef float f32;
typedef double f64;

#define BOARD_HEIGHT 22
#define BOARD_WIDTH 10
#define BOARD_HEIGHT_VISIBLE 20
#define BOARD_GRID_SIZE 30

#define SCREEN_WIDTH 300
#define SCREEN_HEIGHT 720

#define TETRAMINO_SHAPES_TOTAL 3

// COLOR HELPERS
// Get individual color values from a 32bit color pixel
#define RED(c) ((c & 0xFF000000) >> 24)
#define GREEN(c) ((c & 0x00FF0000) >> 16)
#define BLUE(c) ((c & 0x0000FF00) >> 8)
#define ALPHA(c) (c & 0x000000FF)

// Get a color pixel from individual colors
#define COLOR(r, g, b, a) ((r << 24) | (g << 16) | (b << 8) | a)

// Pallette
#define COLOR_BLACK COLOR(0x00, 0x00, 0x00, 0xFF)
#define COLOR_WHITE COLOR(0xFF, 0xFF, 0xFF, 0xFF)
#define COLOR_RED COLOR(0xFF, 0x00, 0x00, 0xFF)
#define COLOR_GREEN COLOR(0x00, 0xFF, 0x00, 0xFF)
#define COLOR_BLUE COLOR(0x00, 0x00, 0xFF, 0xFF)
#define COLOR_YELLOW COLOR(0xFF, 0xFF, 0x00, 0xFF)
#define COLOR_PURPLE COLOR(0x80, 0x00, 0x80, 0xFF)

typedef enum
{
    GAME_STATE_PHASE_PLAY
} GameStatePhase;

typedef struct
{
    u8 *shape;
    u8 dimension;
} Tetramino;

typedef struct
{
    u8 tetramino_index;
    u8 rotation;
} PieceState;

typedef struct
{
    u8 board[BOARD_HEIGHT_VISIBLE][BOARD_WIDTH];
    PieceState piece;
    GameStatePhase phase;
} GameState;

internal u8
tetramino_color_index(Tetramino *tetramino, u8 row, u8 col, u8 rotation)
{
    u8 dimension = tetramino->dimension;
    u8 t_row;
    u8 t_col;
    rotation = rotation % 4;
    switch (rotation)
    {
    case 0:
    {
        t_row = row;
        t_col = col;
    }
    break;
    case 1:
    {
        t_row = dimension - col - 1;
        t_col = row;
    }
    break;
    case 2:
    {
        t_row = dimension - row - 1;
        t_col = dimension - col - 1;
    }
    break;
    case 3:
    {
        t_row = col;
        t_col = dimension - row - 1;
    }
    break;
    }
    return tetramino->shape[t_row * dimension + t_col];
}

internal void
update_game_play(GameState *state)
{
    state->phase = GAME_STATE_PHASE_PLAY;
}

internal void
update_game(GameState *state)
{
    switch (state->phase)
    {
    case GAME_STATE_PHASE_PLAY:
    {
        update_game_play(state);
    }
    break;
    }
}

global_variable SDL_Window *global_window;
global_variable SDL_Renderer *global_renderer;
global_variable SDL_Texture *global_screen;
global_variable u32 *global_screen_buffer;
global_variable int global_pitch = SCREEN_WIDTH * sizeof(u32);

global_variable u8 _TETRINO_0[] = {
    0, 0, 0, 0,
    1, 1, 1, 1,
    0, 0, 0, 0,
    0, 0, 0, 0
};

global_variable u8 _TETRINO_1[] = {
    2, 2,
    2, 2
};

global_variable u8 _TETRINO_2[] = {
    0, 0, 0,
    3, 3, 3,
    0, 3, 0
};

global_variable Tetramino global_tetramino_array[] = 
{
    { _TETRINO_0, 4 },
    { _TETRINO_1, 2 },
    { _TETRINO_2, 3 },
};

global_variable GameState global_game_state;

global_variable u32 global_colors_base[] = {
    COLOR(0x28, 0x28, 0x28, 0xFF),
    COLOR(0x2D, 0x99, 0x99, 0xFF),
    COLOR(0x99, 0x99, 0x2D, 0xFF),
    COLOR(0x99, 0x2D, 0x99, 0xFF),
    COLOR(0x2D, 0x99, 0x51, 0xFF),
    COLOR(0x99, 0x2D, 0x2D, 0xFF),
    COLOR(0x2D, 0x63, 0x99, 0xFF),
    COLOR(0x99, 0x63, 0x2D, 0xFF)};

global_variable u32 global_colors_light[] = {
    COLOR(0x28, 0x28, 0x28, 0xFF),
    COLOR(0x44, 0xE5, 0xE5, 0xFF),
    COLOR(0xE5, 0xE5, 0x44, 0xFF),
    COLOR(0xE5, 0x44, 0xE5, 0xFF),
    COLOR(0x44, 0xE5, 0x7A, 0xFF),
    COLOR(0xE5, 0x44, 0x44, 0xFF),
    COLOR(0x44, 0x95, 0xE5, 0xFF),
    COLOR(0xE5, 0x95, 0x44, 0xFF)};

global_variable u32 global_colors_dark[] = {
    COLOR(0x28, 0x28, 0x28, 0xFF),
    COLOR(0x1E, 0x66, 0x66, 0xFF),
    COLOR(0x66, 0x66, 0x1E, 0xFF),
    COLOR(0x66, 0x1E, 0x66, 0xFF),
    COLOR(0x1E, 0x66, 0x36, 0xFF),
    COLOR(0x66, 0x1E, 0x1E, 0xFF),
    COLOR(0x1E, 0x42, 0x66, 0xFF),
    COLOR(0x66, 0x42, 0x1E, 0xFF)};

internal void
cleanup(void)
{
    free(global_screen_buffer);
    SDL_DestroyTexture(global_screen);
    SDL_DestroyRenderer(global_renderer);
    SDL_DestroyWindow(global_window);
    SDL_Quit();
}

internal void
buffer_draw_fill_rect(int x, int y, int w, int h, u32 color)
{
    u8 *row_pixel = (u8 *)global_screen_buffer + y * global_pitch;
    for (int row = y; row < y + h; row++)
    {
        u32 *pixel = (u32 *)(row_pixel);
        pixel += x;
        for (int col = x; col < x + w; col++)
        {
            *pixel = color;
            pixel++;
        }
        row_pixel += global_pitch;
    }
}

internal void
buffer_draw_gradient(int x_offset, int y_offset)
{
    u8 *row = (u8 *)global_screen_buffer;
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        u32 *pixel = (u32 *)row;
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            u8 red = (u8)(x + x_offset);
            u8 green = (u8)(y + y_offset);
            *pixel = (u32)(red << 24 | green << 16);
            pixel++;
        }
        row += global_pitch;
    }
}

internal void
buffer_draw_cell(u32 row, u32 col, u8 color_index)
{
    u32 color_base = global_colors_base[color_index];
    u32 color_dark = global_colors_dark[color_index];
    u32 color_light = global_colors_light[color_index];

    u32 edge = BOARD_GRID_SIZE / 8;
    u32 x = col * BOARD_GRID_SIZE;
    u32 y = row * BOARD_GRID_SIZE;
    buffer_draw_fill_rect(x, y, BOARD_GRID_SIZE, BOARD_GRID_SIZE,
                          color_dark);
    buffer_draw_fill_rect(x + edge, y, BOARD_GRID_SIZE - edge, BOARD_GRID_SIZE - edge,
                          color_light);
    buffer_draw_fill_rect(x + edge, y + edge,
                          BOARD_GRID_SIZE - 2 * edge, BOARD_GRID_SIZE - 2 * edge, color_base);
}

internal void
buffer_draw_piece(u8 row, u8 col)
{
    PieceState piece = global_game_state.piece;
    Tetramino tetramino = global_tetramino_array[piece.tetramino_index];
    for (u8 y = 0; y < tetramino.dimension; y++)
    {
        for (u8 x = 0; x < tetramino.dimension; x++) 
        {
            u8 color_index = tetramino_color_index(&tetramino, y, x, piece.rotation);
            if (color_index)
            {
                buffer_draw_cell(row + y, col + x, color_index);
            }
        }
    }

}

int
main(void)
{

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "%s:%d: SDL_Init failed: %s\n",
                __FILE__, __LINE__, SDL_GetError());
        return 1;
    }

    global_window = SDL_CreateWindow("Dark Carvern",
                                     SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                     SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!global_window)
    {
        fprintf(stderr, "%s:%d: SDL_CreateWindow failed: %s\n",
                __FILE__, __LINE__, SDL_GetError());
        cleanup();
        return 1;
    }

    global_renderer = SDL_CreateRenderer(global_window,
                                         0, SDL_RENDERER_SOFTWARE);
    if (!global_renderer)
    {
        fprintf(stderr, "%s:%d: SDL_CreateRenderer failed: %s\n",
                __FILE__, __LINE__, SDL_GetError());
        cleanup();
        return 1;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    if (SDL_RenderSetLogicalSize(global_renderer,
                                 SCREEN_WIDTH, SCREEN_HEIGHT) < 0)
    {
        fprintf(stderr, "%s:%d: SDL_RenderSetLogicalSize failed: %s\n",
                __FILE__, __LINE__, SDL_GetError());
        cleanup();
        return 1;
    }

    global_screen = SDL_CreateTexture(global_renderer, SDL_PIXELFORMAT_RGBA8888,
                                      SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!global_screen)
    {
        fprintf(stderr, "%s:%d: SDL_CreateTexture failed: %s\n",
                __FILE__, __LINE__, SDL_GetError());
        cleanup();
        return 1;
    }

    global_screen_buffer = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(u32));
    if (!global_screen_buffer)
    {
        fprintf(stderr, "%s:%d: malloc failed: %s\n",
                __FILE__, __LINE__, SDL_GetError());
        cleanup();
        return 1;
    }

    SDL_Event Event;
    int running = 1;
    u8 x = 0;
    u8 y = 0;

    global_game_state.piece.tetramino_index = 0;

    while (running)
    {
        while (SDL_PollEvent(&Event))
        {
            if (Event.type == SDL_QUIT)
            {
                running = 0;
                cleanup();
                return 0;
            }
            else if (Event.type == SDL_KEYDOWN)
            {

                switch (Event.key.keysym.sym)
                {

                case SDLK_ESCAPE:
                {
                    cleanup();
                    return 0;
                }
                case SDLK_UP:
                {
                    y--;
                }
                break;

                case SDLK_DOWN:
                {
                    y++;
                }
                break;

                case SDLK_LEFT:
                {
                    x--;
                }
                break;

                case SDLK_RIGHT:
                {
                    x++;
                }
                break;

                default:
                    break;
                }
            }
        }

        // clear buffer
        memset(global_screen_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(u32));
        buffer_draw_piece(y, x);
        SDL_UpdateTexture(global_screen, 0, global_screen_buffer, global_pitch);
        SDL_RenderClear(global_renderer);
        SDL_RenderCopy(global_renderer, global_screen, 0, 0);
        SDL_RenderPresent(global_renderer);
        SDL_Delay(16);
    }

    cleanup();
    return 0;
}
