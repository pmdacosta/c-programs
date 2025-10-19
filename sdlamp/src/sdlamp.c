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

global_variable u8* global_wav_buffer = 0;
global_variable u32 global_wav_buffer_pos = 0;
global_variable u32 global_wav_len = 0;
global_variable SDL_AudioSpec global_spec_src;
global_variable SDL_AudioSpec global_spec_desired;
global_variable SDL_AudioStream* global_audio_stream;

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

internal u8
audio_open_file(char* file)
{
    SDL_FreeWAV(global_wav_buffer);
    global_wav_buffer = 0;
    global_wav_buffer_pos = 0;
    global_wav_len = 0;
    SDL_ClearQueuedAudio(global_audio_device_id);
    SDL_AudioStreamClear(global_audio_stream);
    SDL_FreeAudioStream(global_audio_stream);
    global_audio_stream = 0;

    if (!SDL_LoadWAV(file, &global_spec_src, &global_wav_buffer, &global_wav_len))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "sdlamp", SDL_GetError(), global_window);
        return 0;
    }
    
    global_audio_stream = SDL_NewAudioStream(global_spec_src.format,
                       global_spec_src.channels,
                       global_spec_src.freq,
                       global_spec_desired.format,
                       global_spec_desired.channels,
                       global_spec_desired.freq);
    if (!global_audio_stream)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "sdlamp", SDL_GetError(), global_window);
        SDL_FreeWAV(global_wav_buffer);
        global_wav_buffer = 0;
        global_wav_buffer_pos = 0;
        global_wav_len = 0;
        return 0;
    }

    if (SDL_AudioStreamPut(global_audio_stream, global_wav_buffer, global_wav_len) == -1) // TODO: graceful handling
    {
        panic_and_abort("SDL_AudioStreamPut failed", SDL_GetError(), __FILE__, __LINE__);
    }

    SDL_AudioStreamFlush(global_audio_stream);

    return 1;
}

int
main(void)
{
    u8 running = 1;
    u8 audio_paused = 1;
    u8 stream_buffer[32 * 1024];
    SDL_Rect rect_rewind = {100, 100, 100, 100};
    SDL_Rect rect_pause = {400, 100, 100, 100};
    SDL_Event event;

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

    SDL_zero(global_spec_desired);
    global_spec_desired.freq = 48000;
    global_spec_desired.format = AUDIO_F32;
    global_spec_desired.channels = 2;
    global_spec_desired.samples = 4096;
    global_spec_desired.callback = 0;

    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
    if (!audio_open_file("music.wav"))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "sdlamp", SDL_GetError(), global_window);
    }


    global_audio_device_id = SDL_OpenAudioDevice(0, 0, &global_spec_desired, 0, 0);
    if (!global_audio_device_id)
    {
        panic_and_abort("SDL_OpenAudioDevice failed", SDL_GetError(), __FILE__, __LINE__);
    }

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
                    SDL_ClearQueuedAudio(global_audio_device_id);
                    SDL_AudioStreamClear(global_audio_stream);
                    if (SDL_AudioStreamPut(global_audio_stream, global_wav_buffer, global_wav_len) == -1) // TODO: graceful handling
                    {
                        panic_and_abort("SDL_AudioStreamPut failed", SDL_GetError(), __FILE__, __LINE__);
                    }
                    SDL_AudioStreamFlush(global_audio_stream);
                }
                else if (SDL_PointInRect(&pt, &rect_pause) && global_audio_device_id)
                {
                    audio_paused = !audio_paused;
                    SDL_PauseAudioDevice(global_audio_device_id, audio_paused);
                }
                break;
            }

            case SDL_DROPFILE:
            {
                audio_paused = 1;
                SDL_PauseAudioDevice(global_audio_device_id, audio_paused);
                if (!audio_open_file(event.drop.file))
                {
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "sdlamp", SDL_GetError(), global_window);
                }
                SDL_free(event.drop.file);
                break;
            }
            }
        }
        
        if (SDL_GetQueuedAudioSize(global_audio_device_id) < 8 * 1024)
        {
            u32 bytes_remaining = SDL_AudioStreamAvailable(global_audio_stream);
            if (bytes_remaining)
            {
                u32 bytes_load_amount = SDL_min(bytes_remaining, 32 * 1024);
                SDL_AudioStreamGet(global_audio_stream, &stream_buffer, bytes_load_amount);
                SDL_QueueAudio(global_audio_device_id, &stream_buffer, bytes_load_amount);
            }
        }


        SDL_SetRenderDrawColor(global_renderer, 0x00, 0x77, 0x00, 255);
        SDL_RenderClear(global_renderer);

        SDL_SetRenderDrawColor(global_renderer, 0xee, 0xee, 0xee, 255);
        SDL_RenderFillRect(global_renderer, &rect_rewind);
        SDL_RenderFillRect(global_renderer, &rect_pause);

        SDL_RenderPresent(global_renderer);
    }

    SDL_FreeWAV(global_wav_buffer);
    cleanup();
    return 0;
}
