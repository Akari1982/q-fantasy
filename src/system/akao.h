#pragma once

#include "../psyq/typedef.h"
#include "../psyq/libspu.h"

#define AKAO_MUSIC 0x0
#define AKAO_SOUND 0x1
#define AKAO_MENU  0x2

#define AKAO_STEREO          0x1
#define AKAO_MONO            0x2
#define AKAO_STEREO_CHANNELS 0x4

#define AKAO_SFX_LEGATO      0x1
#define AKAO_SFX_FULL_LENGTH 0x4

#define AKAO_UPDATE_SPU_VOICE    (SPU_VOICE_VOLL       | SPU_VOICE_VOLR)
#define AKAO_UPDATE_SPU_ADSR     (SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_RMODE | \
                                  SPU_VOICE_ADSR_AR    | SPU_VOICE_ADSR_DR    | SPU_VOICE_ADSR_SR | SPU_VOICE_ADSR_RR | SPU_VOICE_ADSR_SL)
#define AKAO_UPDATE_SPU_BASE_WOR (SPU_VOICE_WDSA       | SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_SMODE | \
                                  SPU_VOICE_ADSR_AR    | SPU_VOICE_ADSR_DR    | SPU_VOICE_ADSR_SR | \
                                  SPU_VOICE_ADSR_SL    | SPU_VOICE_LSAX)
#define AKAO_UPDATE_SPU_BASE     (AKAO_UPDATE_SPU_BASE_WOR | SPU_VOICE_ADSR_RMODE | SPU_VOICE_ADSR_RR)
#define AKAO_UPDATE_SPU_ALL      (AKAO_UPDATE_SPU_BASE     | AKAO_UPDATE_SPU_VOICE    | SPU_VOICE_PITCH)

#define AKAO_UPDATE_VIBRATO          0x1
#define AKAO_UPDATE_TREMOLO          0x2
#define AKAO_UPDATE_PAN_LFO          0x4
#define AKAO_UPDATE_DRUM_MODE        0x8
#define AKAO_UPDATE_SIDE_CHAIN_PITCH 0x10
#define AKAO_UPDATE_SIDE_CHAIN_VOL   0x20
#define AKAO_UPDATE_OVERLAY          0x100
#define AKAO_UPDATE_ALTERNATIVE      0x200

#define AKAO_UPDATE_NOISE_CLOCK 0x10
#define AKAO_UPDATE_REVERB      0x80



struct AkaoInstrument
{
    u32 addr;
    u32 loop_addr;
    u8 ar;
    u8 dr;
    u8 sl;
    s8 sr;
    u8 rr;
    u8 a_mode;
    u8 s_mode;
    u8 r_mode;
    u32 pitch[0xc];
};

struct AkaoVoiceAttr
{
    u32 voice_id;
    u32 mask;
    u32 addr;
    u32 loop_addr;
    s32 a_mode;
    s32 s_mode;
    s32 r_mode;
    u16 pitch;
    u16 ar;
    u16 dr;
    u16 sl;
    u16 sr;
    u16 rr;
    s16 vol_l;
    s16 vol_r;
} ;

struct AkaoChannel
{
    u8* seq;

    u8 length_1;
    u8 length_2;

    s32 volume;
    u32 vol_master;
    u32 pitch_base;

    u16 instr_id;
    AkaoVoiceAttr attr;
};

struct AkaoConfig
{
    u32 active_mask;
    u32 on_mask;
    u32 off_mask;

    u32 tempo;
    u32 tempo_update;

    u32 update_flags;
    s32 reverb_depth;
};

struct AkaoCommandData
{
    u32 param[0x5];
};



void system_akao_init( u8* instr_all, u8* instr_dat );
void system_akao_load_effect_file( u8* effect_all );
void system_akao_load_instr_files( u8* instr_all, u8* instr_dat );
void system_akao_init_data();

void system_akao_main();
void system_akao_main_update();

void system_akao_execute_sequence( AkaoChannel* channel, AkaoConfig* config, u32 mask );

void system_akao_update_keys_on();
void system_akao_update_keys_off();

void system_akao_music_update_pitch_and_volume( AkaoChannel* channel, u32 channel_mask, u32 channel_id );
void system_akao_update_channel_params_to_spu( u32 voice_id, AkaoVoiceAttr& attr );

void system_akao_copy_music( u8* src, u32 size );
void system_akao_instr_init( AkaoChannel* channel, u16 instr_id );
void system_akao_music_channels_init();
