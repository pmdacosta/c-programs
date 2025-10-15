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

internal void cleanup(void)
{
    SDL_CloseAudioDevice(audio_device_id);
    SDL_Quit();
}

int
main(void)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    SDL_AudioSpec wav_spec;
    u8 *wav_buffer = 0;
    u32 wav_len = 0;

    if (!SDL_LoadWAV("music.wav", &wav_spec, &wav_buffer, &wav_len))
    {
        fprintf(stderr, "%s:%d: SDL_LoadWAV failed: %s\n", __FILE__, __LINE__, SDL_GetError());
        cleanup();
        return 1;
    }
//TODO: come back to this
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

    SDL_QueueAudio(audio_device_id, wav_buffer, wav_len);
    SDL_FreeWAV(wav_buffer);

    SDL_PauseAudioDevice(audio_device_id, 0);
    SDL_Delay(5000);

    cleanup();
    return 0;
}
