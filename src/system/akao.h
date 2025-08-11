#pragma once

#include "../psyq/typedef.h"



struct AkaoInstrument
{
    u32 addr;
    u32 loop_addr;
    u8 ar;
    u8 dr;
    u8 sl;
    s8 sr;
    u8 rr;
    s8 a_mode;
    s8 s_mode;
    s8 r_mode;
    s32 pitch[0xc];
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
    s16 sr;
    u16 rr;
    s16 vol_l;
    s16 vol_r;
} ;

struct ChannelData
{
    u32 seq;

    u8 length_1;
    u8 length_2;

    s32 volume;
    u32 pitch_base;

    u16 instr_id;
    AkaoVoiceAttr attr;
};

struct ChannelConfig
{
    u32 active_mask;
    u32 on_mask;
    u32 off_mask;

    u32 tempo;
    u32 tempo_update;
};

struct AkaoCommandData
{
    u32 param[0x5];
};



void system_akao_init( FILE* instr_all, FILE* instr_dat );
void system_akao_load_effect_file( FILE* effect_all );
void system_akao_load_instr_files( FILE* instr_all, FILE* instr_dat );
void system_akao_init_data();

void system_akao_main();
void system_akao_main_update();

void system_akao_execute_sequence( ChannelData* channel, AkaoConfig* config, u32 mask )

void system_akao_update_keys_on();
void system_akao_update_keys_off();

void system_akao_music_update_pitch_and_volume( ChannelData* channel, channel_mask, channel_id );
void system_akao_update_channel_params_to_spu( u32 voice_id, AkaoVoiceAttr& attr );

void system_akao_copy_music( u32 src, u32 size );
void system_akao_instr_init( ChannelData* channel, u16 instr_id );
void system_akao_music_channels_init();
