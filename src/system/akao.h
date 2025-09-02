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
#define AKAO_SFX_LEGATO_ACT  0x2
#define AKAO_SFX_FULL_LENGTH 0x4

#define AKAO_UPDATE_SPU_VOICE    (SPU_VOICE_VOLL           | SPU_VOICE_VOLR)
#define AKAO_UPDATE_SPU_ADSR     (SPU_VOICE_ADSR_AMODE     | SPU_VOICE_ADSR_SMODE  | SPU_VOICE_ADSR_RMODE | \
                                  SPU_VOICE_ADSR_AR        | SPU_VOICE_ADSR_DR     | SPU_VOICE_ADSR_SR    | \
                                  SPU_VOICE_ADSR_RR        | SPU_VOICE_ADSR_SL)
#define AKAO_UPDATE_SPU_BASE_WOR (SPU_VOICE_WDSA           | SPU_VOICE_ADSR_AMODE  | SPU_VOICE_ADSR_SMODE | \
                                  SPU_VOICE_ADSR_AR        | SPU_VOICE_ADSR_DR     | SPU_VOICE_ADSR_SR    | \
                                  SPU_VOICE_ADSR_SL        | SPU_VOICE_LSAX)
#define AKAO_UPDATE_SPU_BASE     (AKAO_UPDATE_SPU_BASE_WOR | SPU_VOICE_ADSR_RMODE  | SPU_VOICE_ADSR_RR)
#define AKAO_UPDATE_SPU_ALL      (AKAO_UPDATE_SPU_BASE     | AKAO_UPDATE_SPU_VOICE | SPU_VOICE_PITCH)

#define AKAO_COMMON_ATTR         (SPU_COMMON_MVOLL         | SPU_COMMON_MVOLR      | SPU_COMMON_MVOLMODEL | \
                                  SPU_COMMON_MVOLMODER     | SPU_COMMON_CDVOLL     | SPU_COMMON_CDVOLR    | \
                                  SPU_COMMON_CDREV         | SPU_COMMON_CDMIX      | SPU_COMMON_EXTVOLL   | \
                                  SPU_COMMON_EXTVOLR       | SPU_COMMON_EXTREV     | SPU_COMMON_EXTMIX)

#define AKAO_UPDATE_VIBRATO          0x1
#define AKAO_UPDATE_TREMOLO          0x2
#define AKAO_UPDATE_PAN_LFO          0x4
#define AKAO_UPDATE_DRUM_MODE        0x8
#define AKAO_UPDATE_SIDE_CHAIN_PITCH 0x10
#define AKAO_UPDATE_SIDE_CHAIN_VOL   0x20
#define AKAO_UPDATE_OVERLAY          0x100
#define AKAO_UPDATE_ALTERNATIVE      0x200
#define AKAO_UPDATE_ALTERNATIVE_CUR  0x400

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
    u8* seq_start;
    u8* seq_end;
    u16 loop_id;
    u16 loop_times[0x4];
    u8* loop_point[0x4];
    u8* drum_offset;

    u32 update_flags;

    u32 over_voice_id;
    u32 alt_voice_id;

    s32 volume;
    u32 vol_master;
    s32 vol_slide_step;
    u16 vol_slide_steps;

    u16 vol_pan;
    s16 vol_pan_slide_step;
    u16 vol_pan_slide_steps;
    s16 vol_balance;
    s16 vol_balance_slide_step;
    u16 vol_balance_slide_steps;

    u16 key;
    u16 key_stored;
    s16 key_add;
    u16 transpose;
    u16 transpose_stored;

    u32 pitch_base;
    u16 octave;
    s32 pitch_slide;
    s32 pitch_slide_step;
    u16 pitch_slide_steps;
    u16 pitch_slide_steps_cur;
    s16 fine_tuning;

//                                        // 0x36 [][]     pitch addition. summarize 0x30, 0x36 and 0xd6 it to get real pitch.
//    u32 pitch_mul_sound;                // 0x3c
//    s32 pitch_mul_sound_slide_step;     // 0x40
//                                        // 0x50 [][][][] ???.
    u16 type;

    u8 length_1;
    u8 length_2;
    s16 length_stored;
    s16 length_fixed;

    u16 instr_id;

//    u16 pitch_mul_sound_slide_steps;    // 0x5a
//    u16 portamento_steps;               // 0x6c
    u16 sfx_mask;
//                                        // 0x70

    s16 vibrato_pitch;
    u16 vibrato_delay;
    u16 vibrato_delay_cur;
    u16 vibrato_rate;
    u16 vibrato_rate_cur;
    u16 vibrato_type;
    u16 vibrato_base;
    u32 vibrato_wave_id;
    u16 vibrato_depth;
//    u16 vibrato_depth_slide_steps;      // 0x80
//    s16 vibrato_depth_slide_step;       // 0x82

//                                        // 0x84

    s16 tremolo_vol;
    u16 tremolo_delay;
    u16 tremolo_delay_cur;
    u16 tremolo_rate;
    u16 tremolo_rate_cur;
    u16 tremolo_type;
    u32 tremolo_wave_id;
    u16 tremolo_depth;
//    u16 tremolo_depth_slide_steps;      // 0x92
//    s16 tremolo_depth_slide_step;       // 0x94

//    u16 pan_lfo_rate;                   // 0x98
//    u16 pan_lfo_rate_cur;               // 0x9a
//    u16 pan_lfo_type;                   // 0x9c
//    u16 pan_lfo_depth;                  // 0x9e
//    u16 pan_lfo_depth_slide_steps;      // 0xa0
//    s16 pan_lfo_depth_slide_step;       // 0xa2
//    u16 noise_switch_delay;             // 0xa4
//    u16 pitch_lfo_switch_delay;         // 0xa6
//    s16 pan_lfo_vol                     // 0xda
//    u32 pan_lfo_wave;                   // 0x20

    AkaoVoiceAttr attr;
};

struct AkaoConfig
{
    u32 stereo_mono;
    u32 active_mask;
    u32 on_mask;
    u32 keyed_mask;
    u32 off_mask;
//    u32 active_mask_stored;         // 0x14
    u32 tempo;
//    s32 tempo_slide_step;           // 0x1c
    u32 tempo_update;
    u32 over_mask;
    u32 alt_mask;
//    u32 noise_mask;                 // 0x2c
    u32 reverb_mask;
//    u32 pitch_lfo_mask;             // 0x34
    u32 update_flags;
//    s32 reverb_mode;                // 0x3c
    s32 reverb_depth;
//    s32 reverb_depth_slide_step;    // 0x44
//    u16 tempo_slide_steps;          // 0x48
//    u16 music_id;                   // 0x4a
//    u16 condition_stored;           // 0x4c
//    u16 condition;                  // 0x4e
//    u16 reverb_depth_slide_steps;   // 0x50
//    u16 noise_clock;                // 0x52
//    u15 mute_music;                 // 0x54
    u16 timer_upper;
    u16 timer_upper_cur;
    u16 timer_lower;
    u16 timer_lower_cur;
    u16 timer_top_cur;
};

struct AkaoCommandData
{
    u32 param[0x5];
};



void AkaoInit( u8* instr_all, u8* instr_dat );
void AkaoQuit();
void AkaoLoadEffect( u8* effect_all );
void AkaoLoadInstr( u8* instr_all, u8* instr_dat );
void AkaoInitData();
void AkaoSetReverbMode( s32 mode );

void AkaoMain();
void AkaoMainUpdate();

void AkaoCopyMusic( u8* src, u32 size );
void AkaoMusicChannelsInit();
void AkaoInstrInit( AkaoChannel* channel, u16 instr_id );

void AkaoExecuteSequence( AkaoChannel* channel, AkaoConfig* config, u32 mask );

void AkaoUpdateKeysOn();
void AkaoUpdateKeysOff();
void AkaoUpdateReverbVoices();
void AkaoCollectChannelsVoicesMask( AkaoChannel* channel, u32& ret_mask, u32 channels_mask, u32 collect_mask );

void AkaoMusicUpdateSlideAndDelay(AkaoChannel* channel, AkaoConfig* config, u32 channel_mask);
void AkaoMusicUpdatePitchAndVolume( AkaoChannel* channel, u32 channel_mask, u32 channel_id );
void AkaoUpdateChannelAndOverlayParamsToSpu( AkaoChannel* channel, u32 mask, u32 over_voice_id );
void AkaoUpdateChannelParamsToSpu( u32 voice_id, AkaoVoiceAttr& attr );

u8 AkaoGetNextKey( AkaoChannel* channel );



extern AkaoChannel g_channels_1[0x18];
extern AkaoConfig g_channels_1_config;

extern u8* g_akao_music;

extern AkaoInstrument g_akao_instrument[0x80];
