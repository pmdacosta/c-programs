#include "SDL.h"
#include <stdint.h>

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

global_variable SDL_AudioDeviceID global_audio_device_id = 0;
global_variable SDL_Window *global_window = 0;
global_variable SDL_Renderer *global_renderer = 0;

typedef struct {
    u8 r;
    u8 g;
    u8 b;
} Color;

internal void
cleanup(void)
{
    SDL_CloseAudioDevice(global_audio_device_id);
    SDL_DestroyWindow(global_window);
    SDL_DestroyRenderer(global_renderer);
    SDL_Quit();
}

internal void
panic_and_abort(char *message, char *sdl_error_msg, char *file, i32 line_number)
{
    fprintf(stderr, "%s:%d: %s: %s\n", file, line_number, message, sdl_error_msg);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, message, sdl_error_msg, global_window);
    cleanup();
    exit(1);
}

int
main(void)
{
    u8 *wav_buffer = 0;
    u32 wav_len = 0;
    u8 running = 1;
    u8 audio_paused = 1;
    u8 click_timer = 0;
    u8 click_id = 0;
    SDL_AudioSpec wav_spec;
    SDL_Rect rect_rewind = {100, 100, 100, 100};
    SDL_Rect rect_pause = {400, 100, 100, 100};
    SDL_Event event;
    Color color_bg = {0};
    Color color_bt = {0xff, 0xff, 0xff};

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        panic_and_abort("SDL_Init failed", SDL_GetError(), __FILE__, __LINE__);
    }

    global_window = SDL_CreateWindow("sdlamp", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    if (!global_window)
    {
        panic_and_abort("SDL_CreateWindow failed", SDL_GetError(), __FILE__, __LINE__);
    }

    global_renderer = SDL_CreateRenderer(global_window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (!global_renderer)
    {
        panic_and_abort("SDL_CreateRenderer failed", SDL_GetError(), __FILE__, __LINE__);
    }

    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {

            case SDL_QUIT:
            {
                cleanup();
                exit(0);
            }

            case SDL_MOUSEBUTTONDOWN:
            {
                SDL_Point pt = {event.button.x, event.button.y};
                if (global_audio_device_id)
                {
                    if (SDL_PointInRect(&pt, &rect_rewind))
                    {
                        SDL_ClearQueuedAudio(global_audio_device_id);
                        SDL_QueueAudio(global_audio_device_id, wav_buffer, wav_len);
                        click_id = 0;
                        click_timer = 20;
                    }
                    else if (SDL_PointInRect(&pt, &rect_pause))
                    {
                        audio_paused = !audio_paused;
                        SDL_PauseAudioDevice(global_audio_device_id, audio_paused);
                        click_id = 1;
                        click_timer = 20;
                    }
                }
                break;
            }

            case SDL_DROPFILE:
            {
                audio_paused = 1;
                if (global_audio_device_id) {
                    SDL_PauseAudioDevice(global_audio_device_id, audio_paused);
                    SDL_CloseAudioDevice(global_audio_device_id);
                    global_audio_device_id = 0;
                }
                SDL_FreeWAV(wav_buffer);
                wav_buffer = 0;
                wav_len = 0;
                if (!SDL_LoadWAV(event.drop.file, &wav_spec, &wav_buffer, &wav_len))
                {
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "sdlamp", SDL_GetError(), global_window);
                }
                else
                {
                    global_audio_device_id = SDL_OpenAudioDevice(0, 0, &wav_spec, 0, 0);
                    if (!global_audio_device_id)
                    {
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "sdlamp", SDL_GetError(), global_window);
                        SDL_FreeWAV(wav_buffer);
                        wav_buffer = 0;
                        wav_len = 0;
                    }
                    else
                    {
                        SDL_QueueAudio(global_audio_device_id, wav_buffer, wav_len);
                        SDL_PauseAudioDevice(global_audio_device_id, audio_paused);
                    }
                }
                SDL_free(event.drop.file);
                break;
            }
            }
        }

        if (global_audio_device_id)
        {
            color_bg.r = 0x77;
            color_bg.g = 0x00;
            color_bg.b = 0x00;
        }
        else
        {
            color_bg.r = 0x55;
            color_bg.g = 0x00;
            color_bg.b = 0x00;
        }

        SDL_SetRenderDrawColor(global_renderer, color_bg.r, color_bg.g, color_bg.r, 255);
        SDL_RenderClear(global_renderer);

        if (audio_paused) {
            color_bt.r = 0x00;
            color_bt.g = 0x00;
            color_bt.b = 0x77;
        } else {
            color_bt.r = 0x00;
            color_bt.g = 0x77;
            color_bt.b = 0x00;
        }


        SDL_SetRenderDrawColor(global_renderer, color_bt.r, color_bt.g, color_bt.b, 255);
        SDL_RenderFillRect(global_renderer, &rect_rewind);
        SDL_RenderFillRect(global_renderer, &rect_pause);

        Color color_bt_highlight = color_bt;
        if (click_timer > 0)
        {
            click_timer--;
            color_bt_highlight.r = 0x77;
            color_bt_highlight.g = 0x00;
            color_bt_highlight.b = 0x00;
            SDL_SetRenderDrawColor(global_renderer, color_bt_highlight.r, color_bt_highlight.g, color_bt_highlight.b, 255);
            if (click_id == 0)
            {
                SDL_RenderFillRect(global_renderer, &rect_rewind);
            } else {
                SDL_RenderFillRect(global_renderer, &rect_pause);
            }
        }

        SDL_RenderPresent(global_renderer);
    }

    SDL_FreeWAV(wav_buffer);
    cleanup();
    return 0;
}
