#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define global_variable static
#define internal static

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef float f32;
typedef double f64;

#define BOARD_HEIGHT 22
#define BOARD_WIDTH 10
#define BOARD_HEIGHT_VISIBLE 20
#define HEADER_HEIGHT 2
#define BOARD_GRID_SIZE 30

#define SCREEN_WIDTH 300
#define SCREEN_HEIGHT 720

#define ArrayCount(x) (sizeof(x) / sizeof((x)[0]))

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
    i8 offset_row;
    i8 offset_col;
} PieceState;

typedef struct
{
    u8 board[BOARD_HEIGHT][BOARD_WIDTH];
    PieceState piece;
    GameStatePhase phase;
} GameState;

typedef struct
{
    u8 left;
    u8 right;
    u8 down;
    u8 space;
    u8 z;
    u8 x;
} InputState;

global_variable InputState clear_input_state;

global_variable SDL_Window *global_window;
global_variable SDL_Renderer *global_renderer;
global_variable SDL_Texture *global_screen;
global_variable u32 *global_screen_buffer;
global_variable u32 global_pitch = SCREEN_WIDTH * sizeof(u32);
global_variable u64 global_frame_counter = 0;
global_variable u8 global_drop_rate = 48;

global_variable u8 _TETRAMINO_0[] = {
    0, 0, 0, 0,
    1, 1, 1, 1,
    0, 0, 0, 0,
    0, 0, 0, 0
};

global_variable u8 _TETRAMINO_1[] = {
    2, 2,
    2, 2
};

global_variable u8 _TETRAMINO_2[] = {
    0, 0, 0,
    3, 3, 3,
    0, 3, 0
};

static const u8 _TETRAMINO_3[] = {
    0, 4, 4,
    4, 4, 0,
    0, 0, 0
};

static const u8 _TETRAMINO_4[] = {
    5, 5, 0,
    0, 5, 5,
    0, 0, 0
};

static const u8 _TETRAMINO_5[] = {
    6, 0, 0,
    6, 6, 6,
    0, 0, 0
};

static const u8 _TETRAMINO_6[] = {
    0, 0, 7,
    7, 7, 7,
    0, 0, 0
};


global_variable Tetramino global_tetramino_array[] = 
{
    { _TETRAMINO_0, 4 },
    { _TETRAMINO_1, 2 },
    { _TETRAMINO_2, 3 },
    { _TETRAMINO_3, 3 },
    { _TETRAMINO_4, 3 },
    { _TETRAMINO_5, 3 },
    { _TETRAMINO_6, 3 },
};

global_variable GameState global_game_state;
global_variable InputState global_input_state;

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

u8
update_check_piece_valid_postion(PieceState* piece)
{
    Tetramino* tetramino = global_tetramino_array + piece->tetramino_index;
    for (i8 row = 0; row < tetramino->dimension; row++)
    {
        for (i8 col = 0; col < tetramino->dimension; col++)
        {
            u8 color_index = tetramino_color_index(tetramino, row, col, piece->rotation);
            if (color_index)
            {
                i8 board_row = piece->offset_row + row;
                i8 board_col = piece->offset_col + col;
                if (board_col < 0)
                {
                    return 0;
                }
                if (board_col >= BOARD_WIDTH)
                {
                    return 0;
                }
                if (board_row < 0)
                {
                    return 0;
                }
                if (board_row >= BOARD_HEIGHT)
                {
                    return 0;
                }
                if (global_game_state.board[board_row][board_col])
                {
                    return 0;
                }
            }
        }
    }
    return 1;
}
internal void
update_merge_piece_board(void)
{
    PieceState piece = global_game_state.piece;
    Tetramino* tetramino = global_tetramino_array + piece.tetramino_index;
    for (i8 y = 0; y < tetramino->dimension; y++)
    {
        for (i8 x = 0; x < tetramino->dimension; x++) 
        {
            u8 color_index = tetramino_color_index(tetramino, y, x, piece.rotation);
            i8 row = y + piece.offset_row;
            i8 col = x + piece.offset_col;
            global_game_state.board[row][col] += color_index;
        }
    }

}

internal void
update_reset_piece(void)
{
    global_game_state.piece.tetramino_index = rand() % ArrayCount(global_tetramino_array);
    Tetramino* tetramino = global_tetramino_array + global_game_state.piece.tetramino_index;
    global_game_state.piece.rotation = 0;
    global_game_state.piece.offset_row = 0;
    global_game_state.piece.offset_col = (BOARD_WIDTH / 2) - tetramino->dimension / 2;
}

internal void
update_soft_drop(void)
{
    global_game_state.piece.offset_row++;
    if (!update_check_piece_valid_postion(&global_game_state.piece))
    {
        global_game_state.piece.offset_row--;
        update_merge_piece_board();
        update_reset_piece();
    }
}

internal void
update_hard_drop(void)
{
    while (update_check_piece_valid_postion(&global_game_state.piece))
    {
        global_game_state.piece.offset_row++;
    }
    global_game_state.piece.offset_row--;
    update_merge_piece_board();
    update_reset_piece();
}

internal void
update_game_play(void)
{
    PieceState piece = global_game_state.piece;
    if (global_input_state.left)
    {
        --piece.offset_col;
    }
    if (global_input_state.right)
    {
        ++piece.offset_col;
    }
    if (global_input_state.z)
    {
        piece.rotation = (piece.rotation + 1) % 4;
    }
    if (global_input_state.x)
    {
        piece.rotation = (piece.rotation - 1) % 4;
    }

    if (update_check_piece_valid_postion(&piece)) 
    {
        global_game_state.piece = piece;
    }

    if (global_input_state.space)
    {
        update_hard_drop();
    }
    if (global_input_state.down)
    {
        update_soft_drop();
    }
    if (global_frame_counter % global_drop_rate == 0)
    {
        update_soft_drop();
    }

}

internal void
update_game(void)
{
    switch (global_game_state.phase)
    {
    case GAME_STATE_PHASE_PLAY:
    {
        update_game_play();
    }
    break;
    }
}

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
    row += HEADER_HEIGHT;
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
buffer_draw_piece(void)
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
                buffer_draw_cell(piece.offset_row + y, piece.offset_col + x, color_index);
            }
        }
    }
}

internal void
buffer_draw_board(void)
{
    for (u8 y = 0; y < BOARD_HEIGHT; y++)
    {
        for (u8 x = 0; x < BOARD_WIDTH; x++) 
        {
            u8 color_index = global_game_state.board[y][x];
            buffer_draw_cell(y, x, color_index);
        }
    }
}

internal void
buffer_draw_header(void)
{
    for (u8 x = 0; x < BOARD_WIDTH; x++) 
    {
        buffer_draw_fill_rect(x * BOARD_GRID_SIZE,
                0 * BOARD_GRID_SIZE,
                BOARD_GRID_SIZE,
                BOARD_GRID_SIZE,
                0x00FF00FF);
    }

    for (u8 x = 0; x < BOARD_WIDTH; x++) 
    {
        buffer_draw_fill_rect(x * BOARD_GRID_SIZE,
                1 * BOARD_GRID_SIZE,
                BOARD_GRID_SIZE,
                BOARD_GRID_SIZE,
                0xFF0000FF);
    }

    return;
    for (u8 y = 0; y < HEADER_HEIGHT ; y++)
    {
        for (u8 x = 0; x < BOARD_WIDTH; x++) 
        {
            buffer_draw_fill_rect(x * BOARD_GRID_SIZE,
                                  y * BOARD_GRID_SIZE,
                                  BOARD_GRID_SIZE,
                                  BOARD_GRID_SIZE,
                                  0x00FF00FF);
        }
    }
}


int
main(void)
{
    srand((i32)time(0));

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

    update_reset_piece();

    SDL_Event Event;
    int running = 1;

    while (running)
    {
        global_input_state = clear_input_state;
        while (SDL_PollEvent(&Event))
        {
            if (Event.type == SDL_QUIT)
            {
                running = 0;
                cleanup();
                return 0;
            }
			if (Event.type == SDL_KEYDOWN)
			{

				switch (Event.key.keysym.sym)
				{

				case SDLK_ESCAPE:
                    running = 0;
                    cleanup();
					return 0;

				case SDLK_z:
                    global_input_state.z = 1;
					break;

				case SDLK_x:
                    global_input_state.x = 1;
					break;

				case SDLK_LEFT:
                    global_input_state.left = 1;
					break;

				case SDLK_RIGHT:
                    global_input_state.right = 1;
					break;

				case SDLK_DOWN:
                    global_input_state.down = 1;
					break;

				case SDLK_SPACE:
                    global_input_state.space = 1;
					break;

				default:
					break;
				}
			}
        }

        update_game_play();

        // clear buffer
        memset(global_screen_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(u32));
        buffer_draw_board();
        buffer_draw_piece();
        buffer_draw_header();
        SDL_UpdateTexture(global_screen, 0, global_screen_buffer, global_pitch);
        SDL_RenderClear(global_renderer);
        SDL_RenderCopy(global_renderer, global_screen, 0, 0);
        SDL_RenderPresent(global_renderer);
        SDL_Delay(16);
        global_frame_counter++;
    }

    cleanup();
    return 0;
}
