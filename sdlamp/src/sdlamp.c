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

global_variable SDL_AudioDeviceID audio_device_id = 0;
global_variable SDL_Window *window = 0;
global_variable SDL_Renderer *renderer = 0;

internal void
cleanup(void)
{
    SDL_CloseAudioDevice(audio_device_id);
    SDL_Quit();
}

internal void
panic_and_abort(char *message, char *sdl_error_msg, char *file, i32 line_number)
{
    fprintf(stderr, "%s:%d: %s: %s\n", file, line_number, message, sdl_error_msg);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, message, sdl_error_msg, 0);
    cleanup();
    exit(1);
}

int
main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        panic_and_abort("SDL_Init failed", SDL_GetError(), __FILE__, __LINE__);
    }

    window = SDL_CreateWindow("sdlamp", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    if (!window)
    {
        panic_and_abort("SDL_CreateWindow failed", SDL_GetError(), __FILE__, __LINE__);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        panic_and_abort("SDL_CreateRenderer failed", SDL_GetError(), __FILE__, __LINE__);
    }

    SDL_AudioSpec wav_spec;
    u8 *wav_buffer = 0;
    u32 wav_len = 0;

    if (!SDL_LoadWAV("music.wav", &wav_spec, &wav_buffer, &wav_len))
    {
        panic_and_abort("SDL_LoadWAV failed", SDL_GetError(), __FILE__, __LINE__);
    }
// TODO: come back to this
#if 0
    SDL_AudioSpec desired_spec;
    memset(&desired_spec, 0, sizeof(desired_spec));
    desired_spec.freq = 48000;
    desired_spec.format = AUDIO_F32;
    desired_spec.channels = 2;
    desired_spec.samples = 4096;
    desired_spec.callback = 0;
#endif

    audio_device_id = SDL_OpenAudioDevice(0, 0, &wav_spec, 0, 0);
    if (!audio_device_id)
    {
        panic_and_abort("SDL_OpenAudioDevice failed", SDL_GetError(), __FILE__, __LINE__);
    }

    SDL_QueueAudio(audio_device_id, wav_buffer, wav_len);
    u8 audio_paused = 1;

    SDL_Rect rect_rewind = {100, 100, 100, 100};
    SDL_Rect rect_pause = {400, 100, 100, 100};

    u8 running = 1;
    SDL_Event event;
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
                if (SDL_PointInRect(&pt, &rect_rewind))
                {
                    SDL_ClearQueuedAudio(audio_device_id);
                    SDL_QueueAudio(audio_device_id, wav_buffer, wav_len);
                }
                else if (SDL_PointInRect(&pt, &rect_pause))
                {
                    audio_paused = !audio_paused;
                    SDL_PauseAudioDevice(audio_device_id, audio_paused);
                }
                break;
            }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0x00, 0x77, 0x00, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 255);
        SDL_RenderFillRect(renderer, &rect_rewind);
        SDL_RenderFillRect(renderer, &rect_pause);

        SDL_RenderPresent(renderer);
    }

    SDL_FreeWAV(wav_buffer);
    cleanup();
    return 0;
}
