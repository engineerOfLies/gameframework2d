#ifndef __GF2D_AUDIO_H__
#define __GF2D_AUDIO_H__

#include <SDL.h>
#include <SDL_mixer.h>
#include "gf2d_types.h"
#include "gf2d_text.h"
#include "gf2d_list.h"

typedef struct
{
    Uint32 ref_count;
    TextLine filepath;  /**<the sound file that was loaded*/
    Mix_Chunk *sound;
    float volume;
    int defaultChannel;
}Sound;

/**defaultChannel
 * @brief initializes the audio system based on the passed in parameters
 * @param maxSounds the maximum number of sounds that can be loaded into memory at once
 * @param channels the nrumber of allocated audio channels (excluding music channel)
 * @param channelGroups the number of channels to be reserved for groups to be set up
 * @param maxMusic the number of simultaneous music files that will be supported
 * @param enableMP3 if true, initializes audio system with mp3 support, if available
 * @param enableOgg if true, initializes audio system with ogg vorbis support, if available
 */
void gf2d_audio_init(
    Uint32 maxSounds,
    Uint32 channels,
    Uint32 channelGroups,
    Uint32 maxMusic,
    Uint8  enableMP3,
    Uint8  enableOgg);

/**
 * @brief load a WAV or RIFF from file and return it as a sound pointer
 * @param filename the file to load
 * @param volume how loud the sound should be on a scale from 0 to 1.0
 * @param defaultChannel which channel to play this sound on if not specified
 * @return NULL on error or a pointer to the sound file
 */
Sound *gf2d_sound_load(char *filename,float volume,int defaultChannel);

/**
 * @brief play a sound file that has been loaded
 * @param loops number of times to loop,  0 means play once, no loops
 * @param channel which channel to play on, -1 means use default
 * @param group which group to play on, -1 means use default
 */
void gf2d_sound_play(Sound *sound,int loops,float volume,int channel,int group);

/**
 * @brief decrement references to the sound.  Free it when needed
 * @param sound the sound file to free
 */
void gf2d_sound_free(Sound *sound);

/**
 * @brief frees all sounds from memory.  This will invalidate any help Sound pointers
 */
void gf2d_sound_clear_all();

#endif
