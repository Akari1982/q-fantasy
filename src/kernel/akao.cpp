#include "akao.h"
#include "psyq/libapi.h"
#include "psyq/libspu.h"

#include "ofLog.h"
#include "ofUtils.h"



u32 g_akao_sound_event;
// u32 g_akao_control_flags;               // 0x80062ff8
u16 g_akao_reverb_pan;
// u16 g_akao_reverb_mul;                  // 0x80062fb8
// u32 g_akao_pitch_mul_music;             // 0x80062fe4
// s16 g_akao_pitch_mul_music_slide_steps; // 0x80062f40
// s32 g_akao_pitch_mul_music_slide_step;  // 0x80062f28
// u32 g_akao_vol_mul_music;               // 0x80062f5c
// s16 g_akao_vol_mul_music_slide_steps;   // 0x80062f44
// s32 g_akao_vol_mul_music_slide_step;    // 0x80062f2c
// u32 g_akao_tempo_mul_music;             // 0x80062fe8
// s16 g_akao_tempo_mul_music_slide_steps; // 0x80062f48
// s32 g_akao_tempo_mul_music_slide_step;  // 0x80062f30
// s32 g_akao_cd_vol;                      // 0x80062fd4
// s16 g_akao_cd_vol_slide_steps;          // 0x80062fcc
// s32 g_akao_cd_vol_slide_step;           // 0x80062fb4
// u32 g_akao_command_queue_id;            // 0x80063010
u32 g_akao_stream_mask;
// u32 g_akao_mute_music_mask;             // 0x80062fd8

AkaoChannel g_channels_1[0x18];
AkaoConfig g_channels_1_config;

// AkaoChannel g_channels_2[0x18];         // 0x80097ec8
 AkaoConfig g_channels_2_config;

AkaoChannel g_channels_3[0x8];
AkaoConfig g_channels_3_config;

SpuCommonAttr g_spu_common_attr;
SpuReverbAttr g_spu_reverb_attr;
// AkaoVoiceAttr g_akao_voice_attr;        // 0x8007ebe4

AkaoInstrument g_akao_instrument[0x80];
u8 g_akao_effects_all[0xc800];
u32 g_akao_effects_all_seq = 0;
u8* g_akao_music;
u32 g_akao_music_size;

u8 g_akao_default_sound[0x20] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

u8 g_akao_default_seq[0x4] = { 0xa0, 0x00, 0x00, 0x00 };

u16 g_akao_length_table[0xb] = { 0xc0c0, 0x6060, 0x3030, 0x1818, 0x0c0c, 0x0606, 0x0303, 0x2020, 0x1010, 0x0808, 0x0404 };

s16 g_akao_left_volume_table[ 0x100 ] =
{
    (s16)0x7f80, (s16)0x7e80, (s16)0x7d80, (s16)0x7c80, (s16)0x7b80, (s16)0x7a80, (s16)0x7980, (s16)0x7880, (s16)0x7780, (s16)0x7680, (s16)0x7580, (s16)0x7480, (s16)0x7380, (s16)0x7280, (s16)0x7180, (s16)0x7080,
    (s16)0x6f80, (s16)0x6e80, (s16)0x6d80, (s16)0x6c80, (s16)0x6b80, (s16)0x6a80, (s16)0x6980, (s16)0x6880, (s16)0x6780, (s16)0x6680, (s16)0x6580, (s16)0x6480, (s16)0x6380, (s16)0x6280, (s16)0x6180, (s16)0x6080,
    (s16)0x5f80, (s16)0x5e80, (s16)0x5d80, (s16)0x5c80, (s16)0x5b80, (s16)0x5a80, (s16)0x5980, (s16)0x5880, (s16)0x5780, (s16)0x5680, (s16)0x5580, (s16)0x5480, (s16)0x5380, (s16)0x5280, (s16)0x5180, (s16)0x5080,
    (s16)0x4f80, (s16)0x4e80, (s16)0x4d80, (s16)0x4c80, (s16)0x4b80, (s16)0x4a80, (s16)0x4980, (s16)0x4880, (s16)0x4780, (s16)0x4680, (s16)0x4580, (s16)0x4480, (s16)0x4380, (s16)0x4280, (s16)0x4180, (s16)0x4080,
    (s16)0x3f80, (s16)0x3e80, (s16)0x3d80, (s16)0x3c80, (s16)0x3b80, (s16)0x3a80, (s16)0x3980, (s16)0x3880, (s16)0x3780, (s16)0x3680, (s16)0x3580, (s16)0x3480, (s16)0x3380, (s16)0x3280, (s16)0x3180, (s16)0x3080,
    (s16)0x2f80, (s16)0x2e80, (s16)0x2d80, (s16)0x2c80, (s16)0x2b80, (s16)0x2a80, (s16)0x2980, (s16)0x2880, (s16)0x2780, (s16)0x2680, (s16)0x2580, (s16)0x2480, (s16)0x2380, (s16)0x2280, (s16)0x2180, (s16)0x2080,
    (s16)0x1f80, (s16)0x1e80, (s16)0x1d80, (s16)0x1c80, (s16)0x1b80, (s16)0x1a80, (s16)0x1980, (s16)0x1880, (s16)0x1780, (s16)0x1680, (s16)0x1580, (s16)0x1480, (s16)0x1380, (s16)0x1280, (s16)0x1180, (s16)0x1080,
    (s16)0x0f80, (s16)0x0e80, (s16)0x0d80, (s16)0x0c80, (s16)0x0b80, (s16)0x0a80, (s16)0x0980, (s16)0x0880, (s16)0x0780, (s16)0x0680, (s16)0x0580, (s16)0x0480, (s16)0x0380, (s16)0x0280, (s16)0x0180, (s16)0x0080,
    (s16)0x0000, (s16)0xffff, (s16)0xfffc, (s16)0xfff7, (s16)0xfff0, (s16)0xffe7, (s16)0xffdc, (s16)0xffcf, (s16)0xffc0, (s16)0xffaf, (s16)0xff9c, (s16)0xff87, (s16)0xff70, (s16)0xff57, (s16)0xff3c, (s16)0xff1f,
    (s16)0xff00, (s16)0xfedf, (s16)0xfebc, (s16)0xfe97, (s16)0xfe70, (s16)0xfe47, (s16)0xfe1c, (s16)0xfdef, (s16)0xfdc0, (s16)0xfd8f, (s16)0xfd5c, (s16)0xfd27, (s16)0xfcf0, (s16)0xfcb7, (s16)0xfc7c, (s16)0xfc3f,
    (s16)0xfc00, (s16)0xfbbf, (s16)0xfb7c, (s16)0xfb37, (s16)0xfaf0, (s16)0xfaa7, (s16)0xfa5c, (s16)0xfa0f, (s16)0xf9c0, (s16)0xf96f, (s16)0xf91c, (s16)0xf8c7, (s16)0xf870, (s16)0xf817, (s16)0xf7bc, (s16)0xf75f,
    (s16)0xf700, (s16)0xf69f, (s16)0xf63c, (s16)0xf5d7, (s16)0xf570, (s16)0xf507, (s16)0xf49c, (s16)0xf42f, (s16)0xf3c0, (s16)0xf34f, (s16)0xf2dc, (s16)0xf267, (s16)0xf1f0, (s16)0xf177, (s16)0xf0fc, (s16)0xf07f,
    (s16)0x1000, (s16)0x1081, (s16)0x1104, (s16)0x1189, (s16)0x1210, (s16)0x1299, (s16)0x1324, (s16)0x13b1, (s16)0x1440, (s16)0x14d1, (s16)0x1564, (s16)0x15f9, (s16)0x1690, (s16)0x1729, (s16)0x17c4, (s16)0x1861,
    (s16)0x1900, (s16)0x19a1, (s16)0x1a44, (s16)0x1ae9, (s16)0x1b90, (s16)0x1c39, (s16)0x1ce4, (s16)0x1d91, (s16)0x1e40, (s16)0x1ef1, (s16)0x1fa4, (s16)0x2059, (s16)0x2110, (s16)0x21c9, (s16)0x2284, (s16)0x2341,
    (s16)0x2400, (s16)0x24c1, (s16)0x2584, (s16)0x2649, (s16)0x2710, (s16)0x27d9, (s16)0x28a4, (s16)0x2971, (s16)0x2a40, (s16)0x2b11, (s16)0x2be4, (s16)0x2cb9, (s16)0x2d90, (s16)0x2e69, (s16)0x2f44, (s16)0x3021,
    (s16)0x3100, (s16)0x31e1, (s16)0x32c4, (s16)0x33a9, (s16)0x3490, (s16)0x3579, (s16)0x3664, (s16)0x3751, (s16)0x3840, (s16)0x3931, (s16)0x3a24, (s16)0x3b19, (s16)0x3c10, (s16)0x3d09, (s16)0x3e04, (s16)0x3f01
};

s16 g_akao_right_volume_table[ 0x100 ] =
{
    (s16)0x0080, (s16)0x0180, (s16)0x0280, (s16)0x0380, (s16)0x0480, (s16)0x0580, (s16)0x0680, (s16)0x0780, (s16)0x0880, (s16)0x0980, (s16)0x0a80, (s16)0x0b80, (s16)0x0c80, (s16)0x0d80, (s16)0x0e80, (s16)0x0f80,
    (s16)0x1080, (s16)0x1180, (s16)0x1280, (s16)0x1380, (s16)0x1480, (s16)0x1580, (s16)0x1680, (s16)0x1780, (s16)0x1880, (s16)0x1980, (s16)0x1a80, (s16)0x1b80, (s16)0x1c80, (s16)0x1d80, (s16)0x1e80, (s16)0x1f80,
    (s16)0x2080, (s16)0x2180, (s16)0x2280, (s16)0x2380, (s16)0x2480, (s16)0x2580, (s16)0x2680, (s16)0x2780, (s16)0x2880, (s16)0x2980, (s16)0x2a80, (s16)0x2b80, (s16)0x2c80, (s16)0x2d80, (s16)0x2e80, (s16)0x2f80,
    (s16)0x3080, (s16)0x3180, (s16)0x3280, (s16)0x3380, (s16)0x3480, (s16)0x3580, (s16)0x3680, (s16)0x3780, (s16)0x3880, (s16)0x3980, (s16)0x3a80, (s16)0x3b80, (s16)0x3c80, (s16)0x3d80, (s16)0x3e80, (s16)0x3f80,
    (s16)0x4080, (s16)0x4180, (s16)0x4280, (s16)0x4380, (s16)0x4480, (s16)0x4580, (s16)0x4680, (s16)0x4780, (s16)0x4880, (s16)0x4980, (s16)0x4a80, (s16)0x4b80, (s16)0x4c80, (s16)0x4d80, (s16)0x4e80, (s16)0x4f80,
    (s16)0x5080, (s16)0x5180, (s16)0x5280, (s16)0x5380, (s16)0x5480, (s16)0x5580, (s16)0x5680, (s16)0x5780, (s16)0x5880, (s16)0x5980, (s16)0x5a80, (s16)0x5b80, (s16)0x5c80, (s16)0x5d80, (s16)0x5e80, (s16)0x5f80,
    (s16)0x6080, (s16)0x6180, (s16)0x6280, (s16)0x6380, (s16)0x6480, (s16)0x6580, (s16)0x6680, (s16)0x6780, (s16)0x6880, (s16)0x6980, (s16)0x6a80, (s16)0x6b80, (s16)0x6c80, (s16)0x6d80, (s16)0x6e80, (s16)0x6f80,
    (s16)0x7080, (s16)0x7180, (s16)0x7280, (s16)0x7380, (s16)0x7480, (s16)0x7580, (s16)0x7680, (s16)0x7780, (s16)0x7880, (s16)0x7980, (s16)0x7a80, (s16)0x7b80, (s16)0x7c80, (s16)0x7d80, (s16)0x7e80, (s16)0x7f80,
    (s16)0x3f01, (s16)0x3e04, (s16)0x3d09, (s16)0x3c10, (s16)0x3b19, (s16)0x3a24, (s16)0x3931, (s16)0x3840, (s16)0x3751, (s16)0x3664, (s16)0x3579, (s16)0x3490, (s16)0x33a9, (s16)0x32c4, (s16)0x31e1, (s16)0x3100,
    (s16)0x3021, (s16)0x2f44, (s16)0x2e69, (s16)0x2d90, (s16)0x2cb9, (s16)0x2be4, (s16)0x2b11, (s16)0x2a40, (s16)0x2971, (s16)0x28a4, (s16)0x27d9, (s16)0x2710, (s16)0x2649, (s16)0x2584, (s16)0x24c1, (s16)0x2400,
    (s16)0x2341, (s16)0x2284, (s16)0x21c9, (s16)0x2110, (s16)0x2059, (s16)0x1fa4, (s16)0x1ef1, (s16)0x1e40, (s16)0x1d91, (s16)0x1ce4, (s16)0x1c39, (s16)0x1b90, (s16)0x1ae9, (s16)0x1a44, (s16)0x19a1, (s16)0x1900,
    (s16)0x1861, (s16)0x17c4, (s16)0x1729, (s16)0x1690, (s16)0x15f9, (s16)0x1564, (s16)0x14d1, (s16)0x1440, (s16)0x13dc, (s16)0x1324, (s16)0x1299, (s16)0x1210, (s16)0x1189, (s16)0x1104, (s16)0x1081, (s16)0x1000,
    (s16)0xf07f, (s16)0xf0fc, (s16)0xf177, (s16)0xf1f0, (s16)0xf267, (s16)0xf2dc, (s16)0xf34f, (s16)0xf3c0, (s16)0xf42f, (s16)0xf49c, (s16)0xf507, (s16)0xf570, (s16)0xf5d7, (s16)0xf63c, (s16)0xf69f, (s16)0xf700,
    (s16)0xf75f, (s16)0xf7bc, (s16)0xf817, (s16)0xf870, (s16)0xf8c7, (s16)0xf91c, (s16)0xf843, (s16)0xf9c0, (s16)0xfa0f, (s16)0xfa5c, (s16)0xfaa7, (s16)0xfaf0, (s16)0xfb37, (s16)0xfb7c, (s16)0xfbbf, (s16)0xfc00,
    (s16)0xfc3f, (s16)0xfc7c, (s16)0xfcb7, (s16)0xfcf0, (s16)0xfd27, (s16)0xfd5c, (s16)0xfd8f, (s16)0xfdc0, (s16)0xfdef, (s16)0xfe1c, (s16)0xfe47, (s16)0xfe70, (s16)0xfe97, (s16)0xfebc, (s16)0xfedf, (s16)0xff00,
    (s16)0xff1f, (s16)0xff3c, (s16)0xff57, (s16)0xff70, (s16)0xff87, (s16)0xff9c, (s16)0xffaf, (s16)0xffc0, (s16)0xffcf, (s16)0xffdc, (s16)0xffe7, (s16)0xfff0, (s16)0xfff7, (s16)0xfffc, (s16)0xffff, (s16)0x0000
};

u32 g_akao_wave_table_key[ 0x10 ] = { 0x0, 0xc, 0x12, 0x1c, 0x22, 0x2c, 0x32, 0xd2, 0x168, 0x1ac, 0xfc, 0x150, 0x1c0, 0x1ac, 0x168, 0x1ac };

s16 g_akao_wave_table[ 0x2c4 ] =
{
    (s16)0x1fff, (s16)0xe001, (s16)0x3fff, (s16)0xc001, (s16)0x5fff, (s16)0xa001, (s16)0x7fff, (s16)0x8001, (s16)0x0000, (s16)0x0000, (s16)0xfffe, (s16)0x0000, (s16)0x7fff, (s16)0x8001, (s16)0x0000, (s16)0x0000,
    (s16)0xfffe, (s16)0x0000, (s16)0x1fff, (s16)0x0000, (s16)0x3fff, (s16)0x0000, (s16)0x5fff, (s16)0x0000, (s16)0x7fff, (s16)0x0000, (s16)0x0000, (s16)0xfffe, (s16)0x7fff, (s16)0x0000, (s16)0x7fff, (s16)0x0000,
    (s16)0x0000, (s16)0xfffe, (s16)0xe001, (s16)0x0000, (s16)0xc001, (s16)0x0000, (s16)0xa001, (s16)0x0000, (s16)0x8001, (s16)0x0000, (s16)0x0000, (s16)0xfffe, (s16)0x8001, (s16)0x0000, (s16)0x8001, (s16)0x0000,
    (s16)0x0000, (s16)0xfffe, (s16)0x0000, (s16)0x0500, (s16)0x09e2, (s16)0x12cd, (s16)0x169e, (s16)0x19e1, (s16)0x1c81, (s16)0x1e6d, (s16)0x1f9a, (s16)0x1fff, (s16)0x1f9c, (s16)0x1e71, (s16)0x1c86, (s16)0x19e8,
    (s16)0x16a7, (s16)0x12d7, (s16)0x0e90, (s16)0x09ee, (s16)0x050d, (s16)0x0000, (s16)0xfb0c, (s16)0xf62a, (s16)0xf186, (s16)0xed3d, (s16)0xe96a, (s16)0xe626, (s16)0xe384, (s16)0xe196, (s16)0xe067, (s16)0xe000,
    (s16)0xe061, (s16)0xe18a, (s16)0xe373, (s16)0xe60f, (s16)0xe94f, (s16)0xed1d, (s16)0xf163, (s16)0xf604, (s16)0xfae5, (s16)0x0000, (s16)0x0a01, (s16)0x13c4, (s16)0x259a, (s16)0x2d3c, (s16)0x33c2, (s16)0x3902,
    (s16)0x3cda, (s16)0x3f34, (s16)0x3fff, (s16)0x3f38, (s16)0x3ce2, (s16)0x390d, (s16)0x33d1, (s16)0x2d4f, (s16)0x25af, (s16)0x1d21, (s16)0x13dd, (s16)0x0a1b, (s16)0x0000, (s16)0xf618, (s16)0xec54, (s16)0xe30c,
    (s16)0xda7b, (s16)0xd2d5, (s16)0xcc4d, (s16)0xc709, (s16)0xc32d, (s16)0xc0cf, (s16)0xc000, (s16)0xc0c3, (s16)0xc315, (s16)0xc6e6, (s16)0xcc1f, (s16)0xd29e, (s16)0xda3b, (s16)0xe2c6, (s16)0xec09, (s16)0xf5ca,
    (s16)0x0000, (s16)0x0f02, (s16)0x1da6, (s16)0x3867, (s16)0x43da, (s16)0x4da3, (s16)0x5583, (s16)0x5b47, (s16)0x5ece, (s16)0x5fff, (s16)0x5ed4, (s16)0x5b53, (s16)0x5594, (s16)0x4dba, (s16)0x43f6, (s16)0x3886,
    (s16)0x2bb2, (s16)0x1dcb, (s16)0x0f29, (s16)0x0000, (s16)0xf124, (s16)0xe27e, (s16)0xd492, (s16)0xc7b8, (s16)0xbc40, (s16)0xb273, (s16)0xaa8e, (s16)0xa4c3, (s16)0xa137, (s16)0xa000, (s16)0xa124, (s16)0xa49f,
    (s16)0xaa59, (s16)0xb22e, (s16)0xbbed, (s16)0xc759, (s16)0xd429, (s16)0xe20e, (s16)0xf0af, (s16)0x0000, (s16)0x1403, (s16)0x2788, (s16)0x4b34, (s16)0x5a79, (s16)0x6784, (s16)0x7204, (s16)0x79b5, (s16)0x7e68,
    (s16)0x7fff, (s16)0x7e71, (s16)0x79c5, (s16)0x721b, (s16)0x67a3, (s16)0x5a9e, (s16)0x4b5e, (s16)0x3a43, (s16)0x27ba, (s16)0x1437, (s16)0x0000, (s16)0xec30, (s16)0xd8a8, (s16)0xc619, (s16)0xb4f6, (s16)0xa5ab,
    (s16)0x989a, (s16)0x8e13, (s16)0x865a, (s16)0x819f, (s16)0x8000, (s16)0x8186, (s16)0x862a, (s16)0x8dcc, (s16)0x983e, (s16)0xa53d, (s16)0xb477, (s16)0xc58d, (s16)0xd813, (s16)0xeb95, (s16)0x0000, (s16)0x0000,
    (s16)0xffd9, (s16)0x0000, (s16)0x0000, (s16)0x1403, (s16)0x2788, (s16)0x4b34, (s16)0x5a79, (s16)0x6784, (s16)0x7204, (s16)0x79b5, (s16)0x7e68, (s16)0x7fff, (s16)0x7e71, (s16)0x79c5, (s16)0x721b, (s16)0x67a3,
    (s16)0x5a9e, (s16)0x4b5e, (s16)0x3a43, (s16)0x27ba, (s16)0x1437, (s16)0x0000, (s16)0xec30, (s16)0xd8a8, (s16)0xc619, (s16)0xb4f6, (s16)0xa5ab, (s16)0x989a, (s16)0x8e13, (s16)0x865a, (s16)0x819f, (s16)0x8000,
    (s16)0x8186, (s16)0x862a, (s16)0x8dcc, (s16)0x983e, (s16)0xa53d, (s16)0xb477, (s16)0xc58d, (s16)0xd813, (s16)0xeb95, (s16)0x0000, (s16)0x0000, (s16)0xffd9, (s16)0x0000, (s16)0x09e2, (s16)0x12cd, (s16)0x19e1,
    (s16)0x1e6d, (s16)0x1fff, (s16)0x1e71, (s16)0x19e8, (s16)0x12d7, (s16)0x09ee, (s16)0x000d, (s16)0xf62a, (s16)0xed3d, (s16)0xe626, (s16)0xe196, (s16)0xe000, (s16)0xe18a, (s16)0xe60f, (s16)0xed1d, (s16)0xf604,
    (s16)0x0000, (s16)0x13c4, (s16)0x259a, (s16)0x33c2, (s16)0x3cda, (s16)0x3fff, (s16)0x3ce2, (s16)0x33d1, (s16)0x25af, (s16)0x13dd, (s16)0x001a, (s16)0xec54, (s16)0xda7b, (s16)0xcc4d, (s16)0xc32d, (s16)0xc000,
    (s16)0xc315, (s16)0xcc1f, (s16)0xda3b, (s16)0xec09, (s16)0x0000, (s16)0x1da6, (s16)0x3867, (s16)0x4da3, (s16)0x5b47, (s16)0x5fff, (s16)0x5b53, (s16)0x4dba, (s16)0x3886, (s16)0x1dcb, (s16)0x0027, (s16)0xe27e,
    (s16)0xc7b8, (s16)0xb273, (s16)0xa4c3, (s16)0xa000, (s16)0xa49f, (s16)0xb22e, (s16)0xc759, (s16)0xe20e, (s16)0x0000, (s16)0x2788, (s16)0x4b34, (s16)0x6784, (s16)0x79b5, (s16)0x7fff, (s16)0x79c5, (s16)0x67a3,
    (s16)0x4b5e, (s16)0x27ba, (s16)0x0034, (s16)0xd8a8, (s16)0xb4f6, (s16)0x989a, (s16)0x865a, (s16)0x8000, (s16)0x862a, (s16)0x983e, (s16)0xb477, (s16)0xd813, (s16)0x0000, (s16)0x0000, (s16)0xffec, (s16)0x0000,
    (s16)0x0000, (s16)0x2788, (s16)0x4b34, (s16)0x6784, (s16)0x79b5, (s16)0x7fff, (s16)0x79c5, (s16)0x67a3, (s16)0x4b5e, (s16)0x27ba, (s16)0x0034, (s16)0xd8a8, (s16)0xb4f6, (s16)0x989a, (s16)0x865a, (s16)0x8000,
    (s16)0x862a, (s16)0x983e, (s16)0xb477, (s16)0xd813, (s16)0x0000, (s16)0x0000, (s16)0xffec, (s16)0x0000, (s16)0x0000, (s16)0x07ff, (s16)0x0fff, (s16)0x17ff, (s16)0x1fff, (s16)0x17ff, (s16)0x0fff, (s16)0x07ff,
    (s16)0x0000, (s16)0xf801, (s16)0xf001, (s16)0xe801, (s16)0xe001, (s16)0xe801, (s16)0xf001, (s16)0xf801, (s16)0x0000, (s16)0x0fff, (s16)0x1fff, (s16)0x2fff, (s16)0x3fff, (s16)0x2fff, (s16)0x1fff, (s16)0x0fff,
    (s16)0x0000, (s16)0xf001, (s16)0xe001, (s16)0xd001, (s16)0xc001, (s16)0xd001, (s16)0xe001, (s16)0xf001, (s16)0x0000, (s16)0x17ff, (s16)0x2fff, (s16)0x47ff, (s16)0x5fff, (s16)0x47ff, (s16)0x2fff, (s16)0x17ff,
    (s16)0x0000, (s16)0xe801, (s16)0xd001, (s16)0xb801, (s16)0xa001, (s16)0xb801, (s16)0xd001, (s16)0xe801, (s16)0x0000, (s16)0x1fff, (s16)0x3fff, (s16)0x5fff, (s16)0x7fff, (s16)0x5fff, (s16)0x3fff, (s16)0x1fff,
    (s16)0x0000, (s16)0xe001, (s16)0xc001, (s16)0xa001, (s16)0x8001, (s16)0xa001, (s16)0xc001, (s16)0xe001, (s16)0x0000, (s16)0x0000, (s16)0xfff0, (s16)0x0000, (s16)0x0000, (s16)0x1fff, (s16)0x3fff, (s16)0x5fff,
    (s16)0x7fff, (s16)0x5fff, (s16)0x3fff, (s16)0x1fff, (s16)0x0000, (s16)0xe001, (s16)0xc001, (s16)0xa001, (s16)0x8001, (s16)0xa001, (s16)0xc001, (s16)0xe001, (s16)0x0000, (s16)0x0000, (s16)0xfff0, (s16)0x0000,
    (s16)0x0000, (s16)0x31fd, (s16)0x7d05, (s16)0xf5fd, (s16)0xecee, (s16)0x793c, (s16)0x75d0, (s16)0x07fc, (s16)0xf411, (s16)0xc2f5, (s16)0x660f, (s16)0x330e, (s16)0x3fea, (s16)0xbee1, (s16)0x8c12, (s16)0x1821,
    (s16)0xaffb, (s16)0x4a00, (s16)0xd3e7, (s16)0xf4f5, (s16)0xe137, (s16)0xe7d3, (s16)0x520d, (s16)0x1103, (s16)0xaedf, (s16)0x4af8, (s16)0x0746, (s16)0x70f4, (s16)0x8ec1, (s16)0x9719, (s16)0x3512, (s16)0xf600,
    (s16)0x1b00, (s16)0xb4e9, (s16)0x3b1e, (s16)0xae15, (s16)0x9cc8, (s16)0x65f9, (s16)0xc323, (s16)0x71f8, (s16)0x2cf1, (s16)0xa604, (s16)0x92fc, (s16)0xc90e, (s16)0xb009, (s16)0x10e5, (s16)0x0804, (s16)0x8001,
    (s16)0x12fa, (s16)0x1118, (s16)0xe709, (s16)0x47f8, (s16)0x95d9, (s16)0xdd1c, (s16)0x1b09, (s16)0x0bf4, (s16)0x7bfc, (s16)0x20ff, (s16)0x9404, (s16)0x7dfc, (s16)0x8af5, (s16)0xb717, (s16)0x74f9, (s16)0x2afc,
    (s16)0xdd06, (s16)0xd7f3, (s16)0xf4fd, (s16)0x0608, (s16)0xf218, (s16)0x69df, (s16)0xbc07, (s16)0x4af7, (s16)0x89eb, (s16)0xbb27, (s16)0x1109, (s16)0x7fef, (s16)0x3615, (s16)0xeccd, (s16)0x7621, (s16)0x9815,
    (s16)0x56cf, (s16)0xd406, (s16)0x3322, (s16)0xdfe6, (s16)0xaf0a, (s16)0xb9f5, (s16)0xce08, (s16)0x5d05, (s16)0xbfee, (s16)0xf01a, (s16)0x20e8, (s16)0x9f16, (s16)0x8cf2, (s16)0xc2e7, (s16)0xd22e, (s16)0xc601,
    (s16)0xb9d0, (s16)0x810c, (s16)0x4323, (s16)0x28dd, (s16)0x96f7, (s16)0x5c3c, (s16)0x9fde, (s16)0xaee8, (s16)0xb61b, (s16)0x3ef3, (s16)0x1408, (s16)0xdae9, (s16)0x2c0b, (s16)0xb133, (s16)0xa8e1, (s16)0x05da,
    (s16)0x0914, (s16)0x4e22, (s16)0xffe8, (s16)0x87f1, (s16)0x21f0, (s16)0x8d25, (s16)0x7dfc, (s16)0x1cf0, (s16)0x5e11, (s16)0x3de8, (s16)0xc70a, (s16)0x8f17, (s16)0xfff0, (s16)0x6a0d, (s16)0xffe3, (s16)0x12f3,
    (s16)0x041f, (s16)0x780c, (s16)0xc4ef, (s16)0x5b02, (s16)0x3d02, (s16)0x02f2, (s16)0x1519, (s16)0xdad5, (s16)0xbd09, (s16)0x9219, (s16)0xf503, (s16)0x62dd, (s16)0x3806, (s16)0x0f26, (s16)0xa8fb, (s16)0xe0fe,
    (s16)0xf6e3, (s16)0x0e02, (s16)0x0210, (s16)0x4406, (s16)0x94d4, (s16)0xd10a, (s16)0xa846, (s16)0x10f1, (s16)0x1acc, (s16)0xbdfd, (s16)0x1a30, (s16)0xa0eb, (s16)0x5fea, (s16)0xbf10, (s16)0xbbef, (s16)0xdf1b,
    (s16)0xc20c, (s16)0x2ae0, (s16)0x3ef2, (s16)0xae27, (s16)0x5001, (s16)0xf9dd, (s16)0x3611, (s16)0xad1a, (s16)0x07df, (s16)0x6d05, (s16)0x1a0d, (s16)0xb6db, (s16)0x6d2b, (s16)0x3607, (s16)0xfbcc, (s16)0x4208,
    (s16)0x0f21, (s16)0x36f6, (s16)0xc806, (s16)0x0603, (s16)0xfdd7, (s16)0x7817, (s16)0x0605, (s16)0xaf0b, (s16)0x95fc, (s16)0x10ec, (s16)0xa70e, (s16)0xadf3, (s16)0x660f, (s16)0xbdf2, (s16)0xdef8, (s16)0xe421,
    (s16)0xb1e7, (s16)0xfced, (s16)0x5b1c, (s16)0xa109, (s16)0xccf2, (s16)0xf5f3, (s16)0x9afb, (s16)0xb417, (s16)0x92e8, (s16)0x7f08, (s16)0x4bfa, (s16)0x89fd, (s16)0x3c1c, (s16)0xfcef, (s16)0xf2db, (s16)0x4a1d,
    (s16)0x671d, (s16)0xd6ff, (s16)0x65da, (s16)0xf1fc, (s16)0x6e0a, (s16)0x0a0c, (s16)0x11f8, (s16)0x4a02, (s16)0x5bfc, (s16)0xeb06, (s16)0xa5ed, (s16)0x30e6, (s16)0x8011, (s16)0xd234, (s16)0x08f8, (s16)0x43dc,
    (s16)0xf609, (s16)0xa6f2, (s16)0x9a0d, (s16)0x7006, (s16)0xf5ea, (s16)0x9cfc, (s16)0x811e, (s16)0xade6, (s16)0xe60e, (s16)0x95f7, (s16)0xb304, (s16)0x9cff, (s16)0xefea, (s16)0xf519, (s16)0xfa00, (s16)0x3af6,
    (s16)0xc7d5, (s16)0x2722, (s16)0xbb38, (s16)0x43d1, (s16)0x77ed, (s16)0x4908, (s16)0xce02, (s16)0xab0f, (s16)0x3deb, (s16)0xdb0a, (s16)0x6401, (s16)0x3ff9, (s16)0x76f2, (s16)0x9309, (s16)0xd20f, (s16)0x74e5,
    (s16)0x0000, (s16)0x0000, (s16)0xff00, (s16)0x0000
};

#include "akao_opcodes.h"
#include "akao_commands.h"



AkaoCommand akao_commands[] =
{
    AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommand10,   AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommand14,   AkaoCommand15,   AkaoCommandNull, AkaoCommandNull,
    AkaoCommand18,   AkaoCommand19,   AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommand20,   AkaoCommand21,   AkaoCommand22,   AkaoCommand23,   AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommand20,   AkaoCommand29,   AkaoCommand2a,   AkaoCommand2b,   AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommand30,   AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommand34,   AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommand80,   AkaoCommand81,   AkaoCommand82,   AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommand90,   AkaoCommandNull, AkaoCommand92,   AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommandNull, AkaoCommandNull, AkaoCommand9a,   AkaoCommand9b,   AkaoCommand9c,   AkaoCommand9d,   AkaoCommandNull, AkaoCommandNull,
    AkaoCommanda0,   AkaoCommanda1,   AkaoCommanda2,   AkaoCommanda3,   AkaoCommanda4,   AkaoCommanda5,   AkaoCommanda6,   AkaoCommanda7,
    AkaoCommanda8,   AkaoCommanda9,   AkaoCommandaa,   AkaoCommandab,   AkaoCommandac,   AkaoCommandad,   AkaoCommandae,   AkaoCommandaf,
    AkaoCommandb0,   AkaoCommandb1,   AkaoCommandb2,   AkaoCommandb3,   AkaoCommandb4,   AkaoCommandb5,   AkaoCommandb6,   AkaoCommandb7,
    AkaoCommandb8,   AkaoCommandb9,   AkaoCommandba,   AkaoCommandbb,   AkaoCommandbc,   AkaoCommandbd,   AkaoCommandNull, AkaoCommandNull,
    AkaoCommandc0,   AkaoCommandc1,   AkaoCommandc2,   AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommandc8,   AkaoCommandc9,   AkaoCommandca,   AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommandd0,   AkaoCommandd1,   AkaoCommandd2,   AkaoCommandNull, AkaoCommandd4,   AkaoCommandd5,   AkaoCommandd6,   AkaoCommandNull,
    AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommande0,   AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommande4,   AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull,
    AkaoCommandf0,   AkaoCommandf1,   AkaoCommandf2,   AkaoCommandf3,   AkaoCommandf4,   AkaoCommandf5,   AkaoCommandNull, AkaoCommandNull,
    AkaoCommandf8,   AkaoCommandf9,   AkaoCommandfa,   AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull, AkaoCommandNull
};

AkaoOpcode akao_opcodes[] =
{
    AkaoOpcode_a0_finish_channel,         AkaoOpcode_a1_load_instrument,         AkaoOpcode_a2_next_note_length,            AkaoOpcode_a3_master_volume,
    AkaoOpcode_a4_pitch_bend_slide,       AkaoOpcode_a5_set_octave,              AkaoOpcode_a6_increase_octave,             AkaoOpcode_a7_discrease_octave,
    AkaoOpcode_a8_set_volume,             AkaoOpcode_a9_set_volume_slide,        AkaoOpcode_aa_set_pan,                     AkaoOpcode_ab_set_pan_slide,
    AkaoOpcode_ac_noise_clock_freq,       AkaoOpcode_ad_set_ar,                  AkaoOpcode_ae_set_dr,                      AkaoOpcode_af_set_sl,
    AkaoOpcode_b0_set_voice_dr_sl,        AkaoOpcode_b1_set_sr,                  AkaoOpcode_b2_set_rr,                      AkaoOpcode_b3_reset_adsr,
    AkaoOpcode_b4_vibrato,                AkaoOpcode_b5_vibrato_depth,           AkaoOpcode_b6_vibrato_off,                 AkaoOpcode_b7_attack_mode,
    AkaoOpcode_b8_tremolo,                AkaoOpcode_b9_tremolo_depth,           AkaoOpcode_ba_tremolo_off,                 AkaoOpcode_bb_sustain_mode,
    AkaoOpcode_bc_pan_lfo,                AkaoOpcode_bd_pan_lfo_depth,           AkaoOpcode_be_pan_lfo_off,                 AkaoOpcode_bf_release_mode,
    AkaoOpcode_c0_transpose_absolute,     AkaoOpcode_c1_transpose_relative,      AkaoOpcode_c2_reverb_on,                   AkaoOpcode_c3_reverb_off,
    AkaoOpcode_c4_noise_on,               AkaoOpcode_c5_noise_off,               AkaoOpcode_c6_frequency_modulation_on,     AkaoOpcode_c7_frequency_modulation_off,
    AkaoOpcode_c8_loop_point,             AkaoOpcode_c9_loop_return_times,       AkaoOpcode_ca_loop_return,                 AkaoOpcode_cb_sfx_reset,
    AkaoOpcode_cc_legato_on,              AkaoOpcode_cd_legato_off,              AkaoOpcode_ce_noise_switch,                AkaoOpcode_cf_noise_switch,
    AkaoOpcode_d0_full_length_on,         AkaoOpcode_d1_full_length_off,         AkaoOpcode_d2_frequency_modulation_switch, AkaoOpcode_d3_frequency_modulation_switch,
    AkaoOpcode_d4_side_chain_playback_on, AkaoOpcode_d5_side_chain_playback_off, AkaoOpcode_d6_side_chain_pitch_vol_on,     AkaoOpcode_d7_side_chain_pitch_vol_off,
    AkaoOpcode_d8_fine_tuning_absolute,   AkaoOpcode_d9_fine_tuning_relative,    AkaoOpcode_da_portamento_on,               AkaoOpcode_db_portamento_off,
    AkaoOpcode_dc_fix_note_length,        AkaoOpcode_dd_vibrato_depth_slide,     AkaoOpcode_de_tremolo_depth_slide,         AkaoOpcode_df_pan_lfo_depth_slide,
    AkaoOpcodeNull,                      AkaoOpcodeNull,                       AkaoOpcodeNull,                           AkaoOpcodeNull,
    AkaoOpcodeNull,                      AkaoOpcodeNull,                       AkaoOpcodeNull,                           AkaoOpcodeNull,
    AkaoOpcode_e8_tempo,                  AkaoOpcode_e9_tempo_slide,             AkaoOpcode_ea_reverb_depth,                AkaoOpcode_eb_reverb_depth_slide,
    AkaoOpcode_ec_drum_mode_on,           AkaoOpcode_ed_drum_mode_off,           AkaoOpcode_ee_jump,                        AkaoOpcode_ef_jump_conditional,
    AkaoOpcode_f0_loop_jump_times,        AkaoOpcode_f1_loop_break_times,        AkaoOpcode_f2_load_instrument,             AkaoOpcode_f3,
    AkaoOpcode_f4_overlay_voice_on,       AkaoOpcode_f5_overlay_voice_off,       AkaoOpcode_f6_overlay_volume_balance,      AkaoOpcode_f7_overlay_volume_balance_slide,
    AkaoOpcode_f8_alt_voice_on,           AkaoOpcode_f9_alt_voice_off,           AkaoOpcodeNull,                           AkaoOpcodeNull,
    AkaoOpcodeNull,                      AkaoOpcode_fd_time_signature,          AkaoOpcode_fe_measure_number,              AkaoOpcodeNull
};



// Init AKAO tracker, load instruments, init SPU RAM
// init inner data and structures, open new thread with main handler
void AkaoInit( u8* instr_all, u8* instr_dat )
{
    g_akao_effects_all_seq = 0x1000;

    AkaoLoadInstr( instr_all, instr_dat );

    // load default ADPCM Sample
    PsyqSpuSetTransferStartAddr( 0x76fe0 );
    PsyqSpuWrite( g_akao_default_sound, 0x20 );

    AkaoInitData();

    do
    {
        g_akao_sound_event = PsyqOpenEvent( RCntCNT2, EvSpINT, RCntMdINTR, AkaoMain );
    } while (g_akao_sound_event == -1);

    while( PsyqEnableEvent( g_akao_sound_event ) == 0 ) {}

    while( PsyqSetRCnt( RCntCNT2, 0x43d1, RCntMdINTR ) == 0 ) {}

    while( PsyqStartRCnt( RCntCNT2 ) == 0 ) {}
}



void AkaoQuit()
{
    while( PsyqStopRCnt( RCntCNT2 ) == 0 ) {}
//    system_bios_undeliver_event( RCntCNT2, EvSpINT );
//    while( system_bios_disable_event( g_akao_sound_event ) == 0 ) {}

    while( PsyqCloseEvent( g_akao_sound_event ) == 0 ) {}

//    system_psyq_spu_set_transfer_callback( 0 );
//    system_psyq_spu_set_irq( SPU_OFF );
//    system_psyq_spu_set_irq_callback( 0 );

    PsyqSpuSetKey( SPU_OFF, 0x00ffffff );
}



void AkaoLoadEffect( u8* effect_all )
{
    memcpy( g_akao_effects_all, effect_all, 0xc800 );
}



void AkaoLoadInstr( u8* instr_all, u8* instr_dat )
{
    PsyqSpuSetTransferStartAddr( READ_LE_U32( instr_all + 0x0 ) );
    PsyqSpuWrite( instr_all + 0x10, READ_LE_U32( instr_all + 0x4 ) );

    for( int i = 0; i < 0x80; ++i )
    {
        g_akao_instrument[i].addr = READ_LE_U32( instr_dat + i * 0x40 + 0x0 );
        g_akao_instrument[i].loop_addr = READ_LE_U32( instr_dat + i * 0x40 + 0x4 );
        g_akao_instrument[i].ar = READ_LE_U8( instr_dat + i * 0x40 + 0x8 );
        g_akao_instrument[i].dr = READ_LE_U8( instr_dat + i * 0x40 + 0x9 );
        g_akao_instrument[i].sl = READ_LE_U8( instr_dat + i * 0x40 + 0xa );
        g_akao_instrument[i].sr = READ_LE_U8( instr_dat + i * 0x40 + 0xb );
        g_akao_instrument[i].rr = READ_LE_U8( instr_dat + i * 0x40 + 0xc );
        g_akao_instrument[i].a_mode = READ_LE_S8( instr_dat + i * 0x40 + 0xd );
        g_akao_instrument[i].s_mode = READ_LE_S8( instr_dat + i * 0x40 + 0xe );
        g_akao_instrument[i].r_mode = READ_LE_S8( instr_dat + i * 0x40 + 0xf );
        g_akao_instrument[i].pitch[0x0] = READ_LE_S32( instr_dat + i * 0x40 + 0x10 );
        g_akao_instrument[i].pitch[0x1] = READ_LE_S32( instr_dat + i * 0x40 + 0x14 );
        g_akao_instrument[i].pitch[0x2] = READ_LE_S32( instr_dat + i * 0x40 + 0x18 );
        g_akao_instrument[i].pitch[0x3] = READ_LE_S32( instr_dat + i * 0x40 + 0x1c );
        g_akao_instrument[i].pitch[0x4] = READ_LE_S32( instr_dat + i * 0x40 + 0x20 );
        g_akao_instrument[i].pitch[0x5] = READ_LE_S32( instr_dat + i * 0x40 + 0x24 );
        g_akao_instrument[i].pitch[0x6] = READ_LE_S32( instr_dat + i * 0x40 + 0x28 );
        g_akao_instrument[i].pitch[0x7] = READ_LE_S32( instr_dat + i * 0x40 + 0x2c );
        g_akao_instrument[i].pitch[0x8] = READ_LE_S32( instr_dat + i * 0x40 + 0x30 );
        g_akao_instrument[i].pitch[0x9] = READ_LE_S32( instr_dat + i * 0x40 + 0x34 );
        g_akao_instrument[i].pitch[0xa] = READ_LE_S32( instr_dat + i * 0x40 + 0x38 );
        g_akao_instrument[i].pitch[0xb] = READ_LE_S32( instr_dat + i * 0x40 + 0x3c );
    }
}



void AkaoLoadInstr2( u8* instr2_all, u8* instr2_dat )
{
    PsyqSpuSetTransferStartAddr( READ_LE_U32( instr2_all + 0x0 ) );
    PsyqSpuWrite( instr2_all + 0x10, READ_LE_U32( instr2_all + 0x4 ) );

    for( int i = 0; i < 0x4b; ++i )
    {
        g_akao_instrument[0x35 + i].addr = READ_LE_U32( instr2_dat + i * 0x40 + 0x0 );
        g_akao_instrument[0x35 + i].loop_addr = READ_LE_U32( instr2_dat + i * 0x40 + 0x4 );
        g_akao_instrument[0x35 + i].ar = READ_LE_U8( instr2_dat + i * 0x40 + 0x8 );
        g_akao_instrument[0x35 + i].dr = READ_LE_U8( instr2_dat + i * 0x40 + 0x9 );
        g_akao_instrument[0x35 + i].sl = READ_LE_U8( instr2_dat + i * 0x40 + 0xa );
        g_akao_instrument[0x35 + i].sr = READ_LE_U8( instr2_dat + i * 0x40 + 0xb );
        g_akao_instrument[0x35 + i].rr = READ_LE_U8( instr2_dat + i * 0x40 + 0xc );
        g_akao_instrument[0x35 + i].a_mode = READ_LE_S8( instr2_dat + i * 0x40 + 0xd );
        g_akao_instrument[0x35 + i].s_mode = READ_LE_S8( instr2_dat + i * 0x40 + 0xe );
        g_akao_instrument[0x35 + i].r_mode = READ_LE_S8( instr2_dat + i * 0x40 + 0xf );
        g_akao_instrument[0x35 + i].pitch[0x0] = READ_LE_S32( instr2_dat + i * 0x40 + 0x10 );
        g_akao_instrument[0x35 + i].pitch[0x1] = READ_LE_S32( instr2_dat + i * 0x40 + 0x14 );
        g_akao_instrument[0x35 + i].pitch[0x2] = READ_LE_S32( instr2_dat + i * 0x40 + 0x18 );
        g_akao_instrument[0x35 + i].pitch[0x3] = READ_LE_S32( instr2_dat + i * 0x40 + 0x1c );
        g_akao_instrument[0x35 + i].pitch[0x4] = READ_LE_S32( instr2_dat + i * 0x40 + 0x20 );
        g_akao_instrument[0x35 + i].pitch[0x5] = READ_LE_S32( instr2_dat + i * 0x40 + 0x24 );
        g_akao_instrument[0x35 + i].pitch[0x6] = READ_LE_S32( instr2_dat + i * 0x40 + 0x28 );
        g_akao_instrument[0x35 + i].pitch[0x7] = READ_LE_S32( instr2_dat + i * 0x40 + 0x2c );
        g_akao_instrument[0x35 + i].pitch[0x8] = READ_LE_S32( instr2_dat + i * 0x40 + 0x30 );
        g_akao_instrument[0x35 + i].pitch[0x9] = READ_LE_S32( instr2_dat + i * 0x40 + 0x34 );
        g_akao_instrument[0x35 + i].pitch[0xa] = READ_LE_S32( instr2_dat + i * 0x40 + 0x38 );
        g_akao_instrument[0x35 + i].pitch[0xb] = READ_LE_S32( instr2_dat + i * 0x40 + 0x3c );
    }
}



void AkaoInitData()
{
    // init global AKAO settings
    g_akao_stream_mask = 0;
//    g_akao_pitch_mul_music_slide_steps = 0;
//    g_akao_vol_mul_music_slide_steps = 0;
//    g_akao_tempo_mul_music_slide_steps = 0;
//    g_akao_vol_mul_music = 0x7f0000;
//    [0x80062f68] = w(0);
    g_akao_reverb_pan = 0x40;
//    [0x80062f78] = h(0);
//    [0x80062f8c] = w(0);
//    g_akao_reverb_mul = 0;
//    g_akao_cd_vol_slide_steps = 0;
//    g_akao_cd_vol = 0x7fff0000;
//    g_akao_mute_music_mask = 0;
//    g_akao_pitch_mul_music = 0;
//    g_akao_tempo_mul_music = 0;
//    g_akao_control_flags = 0;
//    g_akao_command_queue_id = 0;

//    [0x80083338] = w(0);
//    [0x80083334 + 0x4a] = h(0);
//    [0x80083398] = w(0);
//    [0x800833de] = h(0);

//    [0x80091dc8] = h(0);

    // masks
    g_channels_3_config.active_mask = 0;
    g_channels_3_config.on_mask = 0;
    g_channels_3_config.keyed_mask = 0;
    g_channels_3_config.off_mask = 0;
    g_channels_3_config.active_mask_stored = 0;
    g_channels_3_config.tempo = 0x66a80000;
    g_channels_3_config.tempo_update = 1;
    g_channels_3_config.noise_mask = 0;
    g_channels_3_config.reverb_mask = 0;
    g_channels_3_config.pitch_lfo_mask = 0;

    // init music channels config
    g_channels_1_config.stereo_mono = AKAO_STEREO;
    g_channels_1_config.active_mask = 0;
    g_channels_1_config.on_mask = 0;
    g_channels_1_config.keyed_mask = 0;
    g_channels_1_config.off_mask = 0;
    g_channels_1_config.active_mask_stored = 0;
    g_channels_1_config.noise_mask = 0;
    g_channels_1_config.reverb_mask = 0;
    g_channels_1_config.pitch_lfo_mask = 0;
    g_channels_1_config.reverb_mode = 0;
    g_channels_1_config.reverb_depth = 0;
    g_channels_1_config.music_id = 0;
    g_channels_1_config.reverb_depth_slide_steps = 0;
    g_channels_1_config.timer_upper = 0;
    g_channels_1_config.timer_upper_cur = 0;
    g_channels_1_config.timer_lower = 0;
    g_channels_1_config.timer_top_cur = 0;

    g_channels_2_config.active_mask = 0;
    g_channels_2_config.music_id = 0;

    // init SPU registers
    g_spu_common_attr.mask = AKAO_COMMON_ATTR;
    g_spu_common_attr.mvol.left = 0x3fff;
    g_spu_common_attr.mvol.right = 0x3fff;
    g_spu_common_attr.mvolmode.left = 0;
    g_spu_common_attr.mvolmode.right = 0;
    g_spu_common_attr.cd.volume.left = 0x7fff;
    g_spu_common_attr.cd.volume.right = 0x7fff;
    g_spu_common_attr.cd.reverb = 0;
    g_spu_common_attr.cd.mix = 1;
    g_spu_common_attr.ext.volume.left = 0;
    g_spu_common_attr.ext.volume.right = 0;
    g_spu_common_attr.ext.reverb = 0;
    g_spu_common_attr.ext.mix = 0;
    PsyqSpuSetCommonAttr( &g_spu_common_attr );

    // init music channels
    for( int i = 0; i < 0x18; ++i )
    {
//        [0x8009c5a0 + i * 0xc + 0x0] = w(0x7f0000);
//        [0x8009c5a0 + i * 0xc + 0x4] = w(0);
//        [0x8009c5a0 + i * 0xc + 0x8] = h(0);

        g_channels_1[i].type = AKAO_MUSIC;
        g_channels_1[i].seq = nullptr;
        g_channels_1[i].seq_start = nullptr;
        g_channels_1[i].update_flags = 0;
        g_channels_1[i].attr.voice_id = i;
//        [0x80096608 + i * 0x108 + 0x50] = w(0);
//
        PsyqSpuSetVoiceVolumeAttr( i, 0, 0, 0, 0 );
    }

    // init sounds channels
    for( int i = 0; i < 0x8; ++i )
    {
        g_channels_3[i].type = AKAO_SOUND;
        g_channels_3[i].seq = nullptr;
        g_channels_3[i].seq_start = nullptr;
        g_channels_3[i].update_flags = 0;
        g_channels_3[i].pitch_mul_sound = 0;
        g_channels_3[i].pitch_mul_sound_slide_steps = 0;
        g_channels_3[i].over_vol_balance = 0x7f00;
        g_channels_3[i].over_vol_balance_slide_steps = 0;
        g_channels_3[i].alt_voice_id = i + 0x10;
        g_channels_3[i].attr.voice_id = i + 0x10;
//        [0x80099788 + i * 0x108 + 0x50] = w(0);
    }
}



void AkaoSetReverbMode( s32 mode )
{
    PsyqSpuGetReverbModeParam( &g_spu_reverb_attr );

    if( g_spu_reverb_attr.mode != mode )
    {
        g_channels_1_config.reverb_mode = mode;

        PsyqSpuSetReverb( SPU_OFF );

        g_spu_reverb_attr.mask = SPU_REV_MODE;
        g_spu_reverb_attr.mode = mode | SPU_REV_MODE_CLEAR_WA;

        PsyqSpuSetReverbModeParam( &g_spu_reverb_attr );

        PsyqSpuSetReverb( SPU_ON );
    }
}



// Main AKAO handler working in separate thread. It updates all SPU registers and inner
// structures. Calls AKAO sequence executions and polls external AKAO commands from queue.
void AkaoMain()
{
//    r_cnt = system_psyq_get_r_cnt( RCntCNT2 );
//    t = system_psyq_vsync( 1 );

//    if( t < hu[0x80062f78] ) [0x80062f78] = h(0);

//    S0 = (t - hu[0x80062f78]) / 0x210; // divide by 528
//    [0x80062f78] = h(t);

//    S2 = S0 & 0xffff;

//    if( (S2 == 0) || (S2 >= 0x9) ) S0 = 0x1;

//    if( g_akao_control_flags & 0x00000004 ) S0 *= 2;

//    while( S0 != 0 )
//    {
        AkaoMainUpdate();
//        S0 -= 1;
//    }

//    r_cnt_delta = system_psyq_get_r_cnt( RCntCNT2 ) - r_cnt;
//    if( r_cnt_delta <= 0 ) r_cnt_delta += 0x43d1;

//    V1 = w[0x8004953c];
//    A0 = w[0x80049540];
//    A1 = w[0x80049544];

//    [0x80049538] = w(V1);
//    [0x8004953c] = w(A0);
//    [0x80049540] = w(A1);
//    [0x80049544] = w(r_cnt_delta);
//    [0x80062e04] = w(V1 + A0 + A1 + r_cnt_delta);
}



void AkaoMainUpdate()
{
    AkaoUpdateKeysOn();

    // handle main music channels
    if( g_channels_1_config.active_mask != 0 )
    {
        // logic updates only few times per seconds
        // this rate depends on tempo fixed point variable
        // tempo can be modified by global settings controlled by commands 0xd0, 0xdq and 0xd2
        u16 tempo = g_channels_1_config.tempo >> 0x10;
//        u8 tempo_mul = (g_akao_tempo_mul_music >> 0x10) & 0xff;
//        if( tempo_mul != 0 )
//        {
//            if( tempo_mul < 0x80 )
//            {
//                tempo += (tempo * tempo_mul) >> 0x7;
//            }
//            else
//            {
//                tempo = (tempo * tempo_mul) >> 0x8;
//            }
//        }
        g_channels_1_config.tempo_update += tempo;

        if( (g_channels_1_config.tempo_update & 0xffff0000) /*|| ((g_akao_control_flags & 0x00000004) != 0)*/ )
        {
            g_channels_1_config.tempo_update &= 0x0000ffff;
//            [0x80062f04] = w(0);

            u32 channel_mask = 0x1;
            u32 channels_mask = g_channels_1_config.active_mask;
            AkaoChannel* channel = g_channels_1;
            while( channels_mask != 0 )
            {
                if( channels_mask & channel_mask )
                {
                    channel->length_start -= 0x1;
                    channel->length_stop -= 0x1;

                    // when start pause ends then we update opcodes and set next note to play
                    // if start and stop length are equal (in case of legato or full length effect) channel
                    // never stops, only notes setting changes and note smoothly transitions from one to another
                    if( channel->length_start == 0 )
                    {
                        AkaoExecuteSequence( channel, &g_channels_1_config, channel_mask );
                    }
                    // when stop pause ends we stop this channel
                    else if( channel->length_stop == 0 )
                    {
                        channel->length_stop |= 0x1; // set default stop length
                        g_channels_1_config.keyed_mask &= ~channel_mask;
                        g_channels_1_config.off_mask |= channel_mask;
                    }

                    AkaoMusicUpdateSlideAndDelay( channel, &g_channels_1_config, channel_mask );
                    channels_mask ^= channel_mask;
                }

                channel += 1;
                channel_mask <<= 1;
            }

//            if( g_channels_1_config.tempo_slide_steps != 0 )
//            {
//                g_channels_1_config.tempo_slide_steps -= 1;
//                g_channels_1_config.tempo += g_channels_1_config.tempo_slide_step;
//            }

//            if( g_channels_1_config.reverb_depth_slide_steps != 0 )
//            {
//                g_channels_1_config.reverb_depth_slide_steps -= 1;
//                g_channels_1_config.reverb_depth += g_channels_1_config.reverb_depth_slide_step;
//                g_channels_1_config.update_flags |= AKAO_UPDATE_REVERB;
//            }

            // update main timer
            if( g_channels_1_config.timer_lower != 0 )
            {
                g_channels_1_config.timer_lower_cur += 1;

                if( g_channels_1_config.timer_lower_cur == g_channels_1_config.timer_lower )
                {
                    g_channels_1_config.timer_lower_cur = 0;
                    g_channels_1_config.timer_upper_cur += 1;

                    if( g_channels_1_config.timer_upper_cur == g_channels_1_config.timer_upper )
                    {
                        g_channels_1_config.timer_upper_cur = 0;
                        g_channels_1_config.timer_top_cur += 1;
                    }
                }
            }
        }
    }

//    if( g_channels_2_config.active_mask != 0 )
//    {
//        A0 = g_channels_1_config.tempo >> 0x10;
//        u8 tempo_mul = (g_akao_tempo_mul_music >> 0x10) & 0xff;
//        if( tempo_mul != 0 )
//        {
//            if( tempo_mul < 0x80 )
//            {
//                A0 += (A0 * tempo_mul) >> 0x7;
//            }
//            else
//            {
//                A0 = (A0 * tempo_mul) >> 0x8;
//            }
//        }
//        g_channels_2_config.tempo_update += A0;
//
//        if( (g_channels_2_config.tempo_update & 0xffff0000) || (g_akao_control_flags & 0x00000004) )
//        {
//            g_channels_2_config.tempo_update &= 0xffff;
//            [0x80062f04] = w(1);
//
//            channel_mask = 0x1;
//            u32 channels_mask = g_channels_2_config.active_mask;
//            AkaoChannel* channel = g_channels_2;
//            while( channels_mask != 0 )
//            {
//                if( channels_mask & channel_mask )
//                {
//                    channel->length_start -= 0x1;
//                    channel->length_stop -= 0x1;
//
//                    if( channel->length_start == 0 )
//                    {
//                        system_akao_execute_sequence( channel, 0x8009a164, channel_mask )
//                    }
//                    else
//                    {
//                        if( channel->length_stop == 0 )
//                        {
//                            channel->length_stop |= 0x1;
//                            g_channels_2_config.keyed_mask &= ~channel_mask;
//                            g_channels_2_config.off_mask |= channel_mask;
//                        }
//                    }
//
//                    AkaoMusicUpdateSlideAndDelay( channel, &g_channels_2_config, channel_mask );
//
//                    channels_mask ^= channel_mask;
//                }
//
//                channel += 1;
//                channel_mask <<= 1;
//            }
//
//            if( g_channels_2_config.tempo_slide_steps != 0 )
//            {
//                g_channels_2_config.tempo_slide_steps -= 1;
//                g_channels_2_config.tempo += g_channels_2_config.tempo_slide_step;
//            }
//        }
//    }

    if( g_channels_3_config.active_mask != 0 )
    {
        g_channels_3_config.tempo_update += g_channels_3_config.tempo >> 0x10;

        if( (g_channels_3_config.tempo_update & 0xffff0000) /*|| ((g_akao_control_flags & 0x00000004) != 0)*/ )
        {
            g_channels_3_config.tempo_update &= 0x0000ffff;

            u32 channel_mask = 0x10000;
            u32 channels_mask = g_channels_3_config.active_mask;
            AkaoChannel* channel = g_channels_3;
            while( channels_mask != 0 )
            {
                if( channels_mask & channel_mask )
                {
//                    if( ((g_akao_control_flags & 0x00000002) == 0) || (channel->type == AKAO_MENU) )
                    {
//                        [channel + 0x50] = w(w[channel + 0x50] + 0x1);

                        channel->length_start -= 0x1;
                        channel->length_stop -= 0x1;

                        if( channel->length_start == 0 )
                        {
                            AkaoExecuteSequence( channel, &g_channels_2_config, channel_mask );
                        }
                        else if( channel->length_stop == 0 )
                        {
                            channel->length_stop |= 0x1;
                            g_channels_3_config.keyed_mask &= ~channel_mask;
                            g_channels_3_config.off_mask |= channel_mask;
                        }

                        AkaoSoundUpdateSlideAndDelay( channel, channel_mask );
                    }

                    channels_mask ^= channel_mask;
                }

                channel += 1;
                channel_mask <<= 1;
            }
        }
    }

//    if( g_channels_1_config.mute_music != 0 )
//    {
//        system_akao_command_9b( 0x8009a000 ); // mute music
//        g_channels_1_config.mute_music = 0;
//    }

//    system_akao_execute_commands_queue();

//    func30380();

    AkaoUpdateKeysOff();
}



void AkaoCopyMusic( u8* src, u32 size )
{
    if( g_akao_music != 0 )
    {
        free( g_akao_music );
    }
    g_akao_music = (u8*)malloc( size );
    g_akao_music_size = size;
    memcpy( g_akao_music, src, size );
}



void AkaoMusicChannelsInit()
{
    u32 channels_mask = READ_LE_U32( g_akao_music + 0x0 ) & 0x00ffffff;
    u8* akao = g_akao_music + 0x4;

    g_channels_1_config.active_mask = channels_mask;
    g_channels_1_config.on_mask = 0;
    g_channels_1_config.keyed_mask = 0;
    g_channels_1_config.off_mask |= 0x00ffffff;
    g_channels_1_config.tempo = 0xffff0000;
    g_channels_1_config.tempo_update = 0x1;
    g_channels_1_config.over_mask = 0;
    g_channels_1_config.alt_mask = 0;
    g_channels_1_config.noise_mask = 0;
    g_channels_1_config.reverb_mask = 0;
    g_channels_1_config.pitch_lfo_mask = 0;
    g_channels_1_config.update_flags = 0;
    g_channels_1_config.reverb_depth = 0;
    g_channels_1_config.reverb_depth_slide_step = 0;
    g_channels_1_config.tempo_slide_steps = 0;
//    g_channels_1_config.condition_stored = 0;
//    g_channels_1_config.condition = 0;
    g_channels_1_config.reverb_depth_slide_steps = 0;
//    g_channels_1_config.mute_music = 0;
    g_channels_1_config.timer_upper_cur = 0;
    g_channels_1_config.timer_lower = 0;
    g_channels_1_config.timer_lower_cur = 0;
    g_channels_1_config.timer_top_cur = 0;

    AkaoChannel* channel = g_channels_1;

    u32 channel_mask = 0x1;
    while( channels_mask != 0 )
    {
        if( channels_mask & channel_mask )
        {
            channel->seq = akao + 0x2 + READ_LE_U16( akao );
            channel->seq_start = channel->seq; // for debug

            channel->vol_master = 0x7f;
            channel->length_start = 0x3;
            channel->length_stop = 0x1;

            AkaoInstrInit( channel, 0x14 );

            channel->drum_offset = g_akao_music;
            channel->pitch_slide = 0;
            channel->update_flags = 0;
            channel->volume = 0x3fff0000;
            channel->vol_slide_steps = 0;
            channel->over_vol_balance_slide_steps = 0;
            channel->vol_pan = 0x4000;
            channel->vol_pan_slide_steps = 0;
            channel->pitch_slide_steps_cur = 0;
//            channel->portamento_steps = 0;
            channel->sfx_mask = 0;
            channel->vibrato_depth = 0;
            channel->vibrato_depth_slide_steps = 0;
            channel->tremolo_depth = 0;
            channel->tremolo_depth_slide_steps = 0;
            channel->pan_lfo_depth = 0;
//            channel->pan_lfo_depth_slide_steps = 0;
//            channel->noise_switch_delay = 0;
//            channel->pitch_lfo_switch_delay = 0;
            channel->loop_id = 0;
            channel->length_stored = 0;
            channel->length_fixed = 0;
            channel->over_vol_balance = 0x4000;
            channel->transpose = 0;
            channel->fine_tuning = 0;
            channel->key_add = 0;
            channel->pan_lfo_vol = 0;

            akao += 0x2;
            channels_mask ^= channel_mask;
        }

        channel += 1;
        channel_mask <<= 1;
    }

//    if( g_akao_control_flags & 0x00000001 )
//    {
//        u32 active_mask = g_channels_1_config.active_mask;
//        g_channels_1_config.active_mask = 0;
//        [0x8009a104 + 0x14] = w(active_mask);
//    }

//    system_akao_update_noise_voices();
    AkaoUpdateReverbVoices();
    AkaoUpdatePitchLfoVoices();
}



void AkaoMusicChannelsStop()
{
    if( g_channels_1_config.active_mask != 0 )
    {
        g_channels_1_config.active_mask |= g_channels_1_config.over_mask | g_channels_1_config.alt_mask;
        g_channels_1_config.on_mask = 0;
        g_channels_1_config.keyed_mask = 0;
        g_channels_1_config.over_mask = 0;
        g_channels_1_config.alt_mask = 0;

        g_channels_1_config.off_mask |= g_channels_1_config.active_mask;

        u32 channel_mask = 0x1;
        u32 channels_mask = g_channels_1_config.active_mask;
        AkaoChannel* channel = g_channels_1;
        while( channels_mask != 0 )
        {
            if( channels_mask & channel_mask )
            {
                channel->seq = g_akao_default_seq;
                channel->length_start = 0x4;
                channel->length_stop = 0x2;
                channels_mask ^= channel_mask;
            }
            channel_mask <<= 1;
            channel += 1;
        }
    }
}



void AkaoSoundGetSequence( u32& offset1, u32& offset2, u16 sound_id )
{
    u16 seq_id = (sound_id & 0x3ff) * 0x2;

    u16 offset = READ_LE_U16( g_akao_effects_all + seq_id * 0x2 );
    offset1 = ( offset != 0xffff ) ? g_akao_effects_all_seq + offset : 0;

    seq_id += 0x1;

    offset = READ_LE_U16( g_akao_effects_all + seq_id * 0x2 );
    offset2 = ( offset != 0xffff ) ? g_akao_effects_all_seq + offset : 0;
}



void AkaoSoundChannelsInit( u8 vol_pan, u16 channel_id, u32 offset1, u32 offset2 )
{
    AkaoChannel* channel_1 = &g_channels_3[channel_id];
    AkaoChannel* channel_2 = &g_channels_3[channel_id + 0x1];

    channel_1->seq = g_akao_default_seq;
//    [channel_1 + 0x50] = w(-1);
    channel_1->type = AKAO_SOUND;
    channel_1->length_start = 0x1;
    channel_1->length_stop = 0x1;

    channel_2->seq = g_akao_default_seq;
//    [channel_2 + 0x50] = w(-1);
    channel_2->type = AKAO_SOUND;
    channel_2->length_start = 0x1;
    channel_2->length_stop = 0x1;

    u32 seq_mask = 0;

    if( offset1 != 0 )
    {
        seq_mask |= 0x1 << (channel_id + 0x10);
        AkaoSoundChannelInit( channel_1, offset1 );
        channel_1->vol_pan = (vol_pan & 0x7f) << 0x8;
        channel_1->vol_pan_slide_steps = 0;
    }

    if( offset2 != 0 )
    {
        seq_mask |= 0x2 << (channel_id + 0x10);
        AkaoSoundChannelInit( channel_2, offset2 );
        channel_2->vol_pan = (vol_pan & 0x7f) << 0x8;
        channel_2->vol_pan_slide_steps = 0;
    }

    g_channels_3_config.active_mask |= seq_mask;
    g_channels_3_config.off_mask |= seq_mask;

    u32 mask = (0x3 << (channel_id + 0x10)) & g_channels_3_config.active_mask;

    g_channels_3_config.on_mask &= ~mask;
    g_channels_3_config.keyed_mask &= ~mask;
    g_channels_3_config.noise_mask &= ~mask;
    g_channels_3_config.reverb_mask &= ~mask;
    g_channels_3_config.pitch_lfo_mask &= ~mask;

//    if( g_akao_control_flags & 0x00000002 )
//    {
//        if( g_channels_3[0x6].type == AKAO_MENU )
//        {
//            g_channels_3_config.active_mask = seq_mask & 0x00c00000;
//            seq_mask &= 0xff3fffff;
//        }
//        else
//        {
//            g_channels_3_config.active_mask = 0;
//        }
//
//        g_channels_3_config.active_mask_stored |= seq_mask;
//    }

//    system_akao_update_noise_voices();
    AkaoUpdateReverbVoices();
    AkaoUpdatePitchLfoVoices();
}



void AkaoSoundChannelInit( AkaoChannel* channel, u32 offset )
{
    AkaoInstrInit( channel, 0x5 );

    channel->seq = &g_akao_effects_all[offset];
    channel->seq_start = channel->seq;
    channel->vol_master = 0x78;
    channel->pitch_slide = 0;
    channel->update_flags = 0;
    channel->volume = 0x32000000;
    channel->vol_slide_steps = 0;
    channel->pitch_slide_steps_cur = 0;
    channel->octave = 0x2;
//    channel->portamento_steps = 0;
    channel->sfx_mask = 0;
    channel->vibrato_depth = 0;
    channel->vibrato_depth_slide_steps = 0;
    channel->tremolo_depth = 0;
    channel->tremolo_depth_slide_steps = 0;
    channel->pan_lfo_depth = 0;
//    channel->pan_lfo_depth_slide_steps = 0;
//    channel->noise_switch_delay = 0;
//    channel->pitch_lfo_switch_delay = 0;
    channel->loop_id = 0;
    channel->length_stored = 0;
    channel->length_fixed = 0;
    channel->transpose = 0;
    channel->fine_tuning = 0;
    channel->key_add = 0;
    channel->pan_lfo_vol = 0;
}



void AkaoInstrInit( AkaoChannel* channel, u16 instr_id )
{
    channel->instr_id = instr_id;
    channel->attr.addr = g_akao_instrument[instr_id].addr;
    channel->attr.loop_addr = g_akao_instrument[instr_id].loop_addr;
    channel->attr.a_mode = g_akao_instrument[instr_id].a_mode;
    channel->attr.s_mode = g_akao_instrument[instr_id].s_mode;
    channel->attr.r_mode = g_akao_instrument[instr_id].r_mode;
    channel->attr.ar = g_akao_instrument[instr_id].ar;
    channel->attr.dr = g_akao_instrument[instr_id].dr;
    channel->attr.sl = g_akao_instrument[instr_id].sl;
    channel->attr.sr = g_akao_instrument[instr_id].sr;
    channel->attr.rr = g_akao_instrument[instr_id].rr;
    channel->attr.mask |= AKAO_UPDATE_SPU_BASE;
}



void AkaoExecuteSequence( AkaoChannel* channel, AkaoConfig* config, u32 mask )
{
    u8 opcode = 0;

    // execute all settings opcodes until note
    while( opcode != 0xa0 )
    {
        u8* akao = channel->seq;
        channel->seq = akao + 0x1;

        opcode = READ_LE_U8( akao );

        if( opcode < 0xa0 ) break; // if next opcode is note then break

        akao_opcodes[opcode - 0xa0]( channel, config, mask );
    }

    // if next opcode is note and not the end of channel then play this note
    if( opcode != 0xa0 )
    {
        if( channel->length_fixed != 0 )
        {
            ofLog( OF_LOG_NOTICE, "MISSING length_fixed" );
//            channel->length_start = channel->length_fixed;
//            channel->length_stop = channel->length_fixed;
        }

        // if length was not set in settings opcode then use note to get length
        if( channel->length_start == 0 )
        {
            // notes divided into 0xc semitones with 0xb presets length for each
            u8 length_id = opcode % 0xb;
            channel->length_start = g_akao_length_table[length_id] & 0x00ff;
            channel->length_stop = (g_akao_length_table[length_id] & 0xff00) >> 0x8;
        }

        // by default we reduce stop length to end note before playing another
        // in case of legato we skip this part to create smooth transition between notes
        // full length note has same effect
        if( ((AkaoGetNextKey( channel ) - 0x84) >= 0xb) && ((channel->sfx_mask & (AKAO_SFX_FULL_LENGTH | AKAO_SFX_LEGATO)) == 0) )
        {
            channel->length_stop -= 0x2;
        }

        channel->length_stored = channel->length_start;

        // if this note is pause/stop effect note 0x8f-0x99
        if( opcode >= 0x8f )
        {
//            channel->portamento_steps = 0;
            channel->vibrato_pitch = 0;
            channel->tremolo_vol = 0;
            channel->sfx_mask &= ~AKAO_SFX_LEGATO_ACT;
        }
        // if this is usual note 0x00-0x83
        // notes 0x84-0x8e are just continue play same note with new length settings
        // they are played without pause
        else if( opcode < 0x84 )
        {
            u32 pitch_base = 0;
            u8 semitone = opcode / 0xb;

            // in drum mode semitone used to determine drum to use
            // from drum data it loads instrument, key and volume settings
            if( channel->update_flags & AKAO_UPDATE_DRUM_MODE )
            {
                if( channel->type == AKAO_MUSIC )
                {
                    config->on_mask |= mask;
                }
                else
                {
                    g_channels_3_config.on_mask |= mask;
                }

                u8 drum_id = semitone % 0xc;

                u8 instr_id = READ_LE_U8( channel->drum_offset + drum_id * 0x5 + 0x0 );

                if( instr_id != channel->instr_id )
                {
                    channel->instr_id = instr_id;
                    channel->attr.addr = g_akao_instrument[instr_id].addr;
                    channel->attr.loop_addr = g_akao_instrument[instr_id].loop_addr;
                    channel->attr.a_mode = g_akao_instrument[instr_id].a_mode;
                    channel->attr.s_mode = g_akao_instrument[instr_id].s_mode;
                    channel->attr.ar = g_akao_instrument[instr_id].ar;
                    channel->attr.dr = g_akao_instrument[instr_id].dr;
                    channel->attr.sl = g_akao_instrument[instr_id].sl;
                    channel->attr.sr = g_akao_instrument[instr_id].sr;
                    channel->attr.mask |= AKAO_UPDATE_SPU_BASE_WOR;

                    if( (channel->update_flags & AKAO_UPDATE_ALTERNATIVE) == 0 )
                    {
                        channel->attr.r_mode = g_akao_instrument[instr_id].r_mode;
                        channel->attr.rr = g_akao_instrument[instr_id].rr;
                        channel->attr.mask |= SPU_VOICE_ADSR_RMODE | SPU_VOICE_ADSR_RR;
                    }
                }

                u8 key = READ_LE_U8( channel->drum_offset + drum_id * 0x5 + 0x1 );

                pitch_base = g_akao_instrument[instr_id].pitch[key % 0xc];

                u8 octave = key / 0xc;
                if( octave >= 0x7 )
                {
                    pitch_base <<= octave - 0x6;
                }
                else if( octave < 0x6 )
                {
                    pitch_base >>= 0x6 - octave;
                }

                channel->volume = READ_LE_U16( channel->drum_offset + drum_id * 0x5 + 0x2 ) << 0x10;
                channel->vol_pan = READ_LE_U8( channel->drum_offset + drum_id * 0x5 + 0x4 ) << 0x8;
            }
            else
            {
                // calculate key by note semitone and octave set in opcodes 0xa5, 0xa6 and 0xa7
                u8 key = channel->octave * 0xc + semitone;

//                if( ( channel->portamento_steps != 0 ) && ( channel->key_stored != 0 ) )
//                {
//                    channel->pitch_slide_steps = channel->portamento_steps;
//                    channel->key_add = (key & 0xff) + channel->transpose - channel->key_stored - channel->transpose_stored;
//                    channel->key = channel->key_stored - channel->transpose - channel->transpose_stored;
//                    key = (channel->key_stored & 0xff) + (channel->transpose_stored & 0xff);
//                }
//                else
                // store unmodified key and transpose key for calculations by settings from opcodes 0xc0 or 0xc1
                {
                    channel->key = key;
                    key += channel->transpose & 0xff;
                }

                // if legato active then we don't need start note (it's still playing)
                // and we don't need restart pitch slide
                if( (channel->sfx_mask & AKAO_SFX_LEGATO_ACT) == 0 )
                {
                    if( channel->type == AKAO_MUSIC )
                    {
                        config->on_mask |= mask;

                        if( channel->update_flags & AKAO_UPDATE_OVERLAY )
                        {
                            config->on_mask |= 0x1 << (( channel->over_voice_id >= 0x18 ) ? channel->over_voice_id - 0x18 : channel->over_voice_id);
                        }
                    }
                    else
                    {
                        g_channels_3_config.on_mask |= mask;
                    }
                    channel->pitch_slide_steps_cur = 0;
                }

                // get pitch for semitone for new key after effects applied for defined instrument
                u8 semitone = key % 0xc;
                pitch_base = g_akao_instrument[channel->instr_id].pitch[semitone];

                // modify pitch by octave from key
                u8 octave = key / 0xc;
                if( octave >= 0x7 )
                {
                    pitch_base <<= octave - 0x6;
                }
                else if( octave < 0x6 )
                {
                    pitch_base >>= 0x6 - octave;
                }
            }

            // set this channel as active and played now
            if( channel->type == AKAO_MUSIC )
            {
                config->keyed_mask |= mask;
            }
            else
            {
                g_channels_3_config.keyed_mask |= mask;
            }

            channel->attr.mask |= AKAO_UPDATE_SPU_VOICE | SPU_VOICE_PITCH;

            // perform fine tuning set by opcodes 0xd8 and 0xd9
            s16 fine_tuning = channel->fine_tuning;
            if( fine_tuning != 0 )
            {
                if( fine_tuning > 0 )
                {
                    fine_tuning = (pitch_base * fine_tuning) >> 0x7;
                }
                else
                {
                    fine_tuning = (pitch_base * fine_tuning) >> 0x8;
                }
                pitch_base = (pitch_base + fine_tuning) & 0xffff;
            }

            channel->pitch_base = pitch_base;

            // reset vibrato effect for new note
            if( channel->update_flags & AKAO_UPDATE_VIBRATO )
            {
                u32 pitch_base = channel->pitch_base * ((channel->vibrato_depth & 0x7f00) >> 0x8);
                if( (channel->vibrato_depth & 0x8000) == 0 ) pitch_base = (pitch_base * 0xf) / 0x100;
                channel->vibrato_base = pitch_base >> 0x7;
                channel->vibrato_wave_id = g_akao_wave_table_key[channel->vibrato_type];
                channel->vibrato_delay_cur = channel->vibrato_delay;
                channel->vibrato_rate_cur = 1;
            }

            // reset tremolo effect for new note
            if( channel->update_flags & AKAO_UPDATE_TREMOLO )
            {
                channel->tremolo_wave_id = g_akao_wave_table_key[channel->tremolo_type];
                channel->tremolo_delay_cur = channel->tremolo_delay;
                channel->tremolo_rate_cur = 1;
            }

            // reset pan lfo effect for new note
            if( channel->update_flags & AKAO_UPDATE_PAN_LFO )
            {
                channel->pan_lfo_wave_id = g_akao_wave_table_key[channel->pan_lfo_type];
                channel->pan_lfo_rate_cur = 1;
            }

            // reset all additions for new note
            channel->vibrato_pitch = 0;
            channel->tremolo_vol = 0;
            channel->pitch_slide = 0;
        }

        // renew legato in action flag for next note if legato still active
        channel->sfx_mask &= ~AKAO_SFX_LEGATO_ACT;
        channel->sfx_mask |= (channel->sfx_mask & AKAO_SFX_LEGATO) << 1;

        // activate pitch bend slide effect set by 0xa4 opcode and recalculate pitch and fine tuning
        if( channel->key_add != 0 )
        {
            channel->key += channel->key_add;

            u16 key = (channel->key & 0xff) + (channel->transpose & 0xff);

            u32 pitch = g_akao_instrument[channel->instr_id].pitch[key % 0xc];

            if( channel->type == AKAO_MUSIC )
            {
                s16 fine_tuning = channel->fine_tuning;
                if( fine_tuning != 0 )
                {
                    if( fine_tuning > 0 )
                    {
                        fine_tuning = (pitch * fine_tuning) >> 0x7;
                    }
                    else
                    {
                        fine_tuning = (pitch * fine_tuning) >> 0x8;
                    }
                    pitch = (pitch + fine_tuning) & 0xffff;
                }
            }

            pitch <<= 0x10;

            u8 octave = key / 0xc;
            if( octave >= 0x7 )
            {
                pitch <<= octave - 0x6;
            }
            else if( octave < 0x6 )
            {
                pitch >>= 0x6 - octave;
            }

            channel->pitch_slide_steps_cur = channel->pitch_slide_steps;
            channel->key_add = 0;
            channel->pitch_slide_step = ((s32)(pitch - (channel->pitch_base << 0x10)) - channel->pitch_slide) / channel->pitch_slide_steps;
        }

        // store current key and transpose for next note effects
        channel->transpose_stored = channel->transpose;
        channel->key_stored = channel->key;
    }
}



void AkaoUpdateKeysOn()
{
    u32 updated_mask = 0;

    // update reverb if reverb depth was changed by opcode 0xea or 0xeb
    if( g_channels_1_config.update_flags & AKAO_UPDATE_REVERB )
    {
        s16 reverb_depth = g_channels_1_config.reverb_depth >> 0x10;

        g_spu_reverb_attr.mask = SPU_REV_DEPTHL | SPU_REV_DEPTHR;

//        if( g_akao_reverb_mul < 0x80 )
//        {
//            reverb_depth += (reverb_depth * g_akao_reverb_mul) >> 0x7;
//        }
//        else
//        {
//            reverb_depth = (reverb_depth * g_akao_reverb_mul) >> 0x8;
//        }

        // change reverb depth pan by global AKAO setting set by command 0xe0
        if( g_akao_reverb_pan < 0x40 )
        {
            g_spu_reverb_attr.depth.left = reverb_depth;
            g_spu_reverb_attr.depth.right = reverb_depth - ((reverb_depth * (g_akao_reverb_pan ^ 0x3f)) >> 0x6);
        }
        else
        {
            g_spu_reverb_attr.depth.left = reverb_depth - ((reverb_depth * (g_akao_reverb_pan & 0x3f)) >> 0x6);
            g_spu_reverb_attr.depth.right = reverb_depth;
        }

        PsyqSpuSetReverbDepth( &g_spu_reverb_attr );

        g_channels_1_config.update_flags ^= AKAO_UPDATE_REVERB;
    }

//    // update noise clock frequency
//    if( g_channels_1_config.update_flags & AKAO_UPDATE_NOISE_CLOCK )
//    {
//        if( g_channels_3_active_mask != 0 )
//        {
//            system_psyq_spu_set_noise_clock( g_channels_3_noise_clock );
//        }
//        else
//        {
//            system_psyq_spu_set_noise_clock( g_channels_1_config.noise_clock );
//        }
//
//        g_channels_1_config.update_flags ^= AKAO_UPDATE_NOISE_CLOCK;
//    }

//    if( g_channels_2_config.active_mask != 0 )
//    {
//        AkaoChannel* channel = g_channels_2;
//        channel_mask = 0x1;
//        channel_id = 0;
//
//        S5 = ~(g_channels_3_active_mask | g_akao_stream_mask) & w[0x80062f68];
//        channels_mask = g_channels_2_config.active_mask & S5 & g_channels_2_config.keyed_mask;
//        updated_mask = S5 & g_channels_2_config.on_mask;
//
//        while( channels_mask != 0 )
//        {
//            if( channels_mask & channel_mask )
//            {
//                AkaoMusicUpdatePitchAndVolume( channel, channel_mask, channel_id );
//
//                if( channel->attr.mask != 0 )
//                {
//
//                    if( channel->update_flags & AKAO_UPDATE_OVERLAY )
//                    {
//                        AkaoUpdateChannelAndOverlayParamsToSpu( channel, S5, channel->over_voice_id - 0x18 );
//                    }
//                    else if( (channel->update_flags & AKAO_UPDATE_ALTERNATIVE) == 0 )
//                    {
//                        AkaoUpdateChannelParamsToSpu( channel->attr.voice_id, channel->attr );
//                    }
//                    else
//                    {
//                        if( channel_mask & g_channels_2_config.on_mask )
//                        {
//                            channel->update_flags ^= AKAO_UPDATE_ALTERNATIVE_CUR;
//                            channel->attr.mask |= AKAO_UPDATE_SPU_ALL;
//                        }
//
//                        if( channel->update_flags & AKAO_UPDATE_ALTERNATIVE_CUR )
//                        {
//                            if( S5 & (0x1 << channel->alt_voice_id) )
//                            {
//                                AkaoUpdateChannelParamsToSpu( channel->alt_voice_id, channel->attr );
//
//                                if( updated_mask & channel_mask )
//                                {
//                                    updated_mask |= 0x1 << channel->alt_voice_id;
//                                    updated_mask &= ~channel_mask;
//                                }
//                            }
//                        }
//                        else
//                        {
//                            AkaoUpdateChannelParamsToSpu( channel->attr.voice_id, channel->attr );
//                        }
//                    }
//                }
//
//                channels_mask ^= channel_mask;
//            }
//
//            channel_mask <<= 0x1;
//            channel += 1;
//            channel_id += 0x1;
//        }
//
//        g_channels_2_config.on_mask = 0;
//    }

    // update music channels
    if( g_channels_1_config.active_mask != 0 )
    {
        u32 exclude_mask = ~(/*w[0x80062f68] |*/ g_channels_3_config.active_mask | g_akao_stream_mask);
        updated_mask |= exclude_mask & g_channels_1_config.on_mask;

        u32 channel_id = 0;

        u32 channel_mask = 1;
        u32 channels_mask = g_channels_1_config.active_mask & exclude_mask & g_channels_1_config.keyed_mask;
        AkaoChannel* channel = g_channels_1;
        while( channels_mask != 0 )
        {
            if( channels_mask & channel_mask )
            {
                AkaoMusicUpdatePitchAndVolume( channel, channel_mask, channel_id );

                if( channel->attr.mask != 0 )
                {
//                    if( g_akao_mute_music_mask & channel_mask )
//                    {
//                        channel->attr.vol_l = 0;
//                        channel->attr.vol_r = 0;
//                    }

                    // if this channel has additional overlay channel then update  this channel too
                    // because it doesn't has mask bit on it's own in active_mask
                    if( channel->update_flags & AKAO_UPDATE_OVERLAY )
                    {
                        AkaoUpdateChannelAndOverlayParamsToSpu( channel, exclude_mask, channel->over_voice_id );
                    }
                    // update channel with alternative voice logic
                    else if( channel->update_flags & AKAO_UPDATE_ALTERNATIVE )
                    {
                        if( g_channels_1_config.on_mask & channel_mask )
                        {
                            channel->attr.mask |= AKAO_UPDATE_SPU_ALL;

                            // switch which voice update this cycle
                            channel->update_flags ^= AKAO_UPDATE_ALTERNATIVE_CUR;
                        }

                        // update alternative voice if we don't has it's voice busy with some else channel
                        if( (channel->update_flags & AKAO_UPDATE_ALTERNATIVE_CUR) && (exclude_mask & (1 << channel->alt_voice_id)) )
                        {
                            AkaoUpdateChannelParamsToSpu( channel->alt_voice_id, channel->attr );

                            if( updated_mask & channel_mask )
                            {
                                updated_mask = (updated_mask | (1 << channel->alt_voice_id)) & ~channel_mask;
                            }
                        }
                        // update normal voice
                        else
                        {
                            AkaoUpdateChannelParamsToSpu( channel->attr.voice_id, channel->attr );
                        }
                    }
                    // update normal voice
                    else
                    {
                        AkaoUpdateChannelParamsToSpu( channel->attr.voice_id, channel->attr );
                    }
                }

                channels_mask ^= channel_mask;
            }

            channel_mask <<= 1;
            channel += 1;
            channel_id += 1;
        }

        g_channels_1_config.on_mask = 0;
    }

    if( g_channels_3_config.active_mask != 0 )
    {
        u32 channel_mask = 0x00010000;
        AkaoChannel* channel = g_channels_3;
        updated_mask |= g_channels_3_config.on_mask;
        u32 channels_mask = g_channels_3_config.active_mask & g_channels_3_config.keyed_mask;

        while( channels_mask != 0 )
        {
            if( channels_mask & channel_mask )
            {
                AkaoSoundUpdatePitchAndVolume( channel, channel_mask );

                if( channel->attr.mask != 0 )
                {
                    AkaoUpdateChannelParamsToSpu( channel->attr.voice_id, channel->attr );
                }

                channels_mask ^= channel_mask;
            }

            channel_mask <<= 0x1;
            channel += 1;
        }

        g_channels_3_config.on_mask = 0;
    }

    if( updated_mask != 0 )
    {
        PsyqSpuSetKey( SPU_ON, updated_mask );
    }
}



void AkaoUpdateKeysOff()
{
    u32 updated_mask = 0;

//    if( g_channels_2_config.active_mask != 0 )
//    {
//        exclude_mask = ~(g_channels_3_active_mask | g_akao_stream_mask);
//        channels_mask = g_channels_2_config.off_mask & w[0x80062f68] & exclude_mask;
//        if( channels_mask != 0 )
//        {
//            system_akao_collect_channels_voices_mask( g_channels_2, updated_mask, channels_mask, exclude_mask );
//        }
//        g_channels_2_config.off_mask = 0;
//    }

    if( g_channels_1_config.active_mask != 0 )
    {
        u32 exclude_mask = ~(/*w[0x80062f68] |*/ g_channels_3_config.active_mask | g_akao_stream_mask);
        u32 channels_mask = exclude_mask & g_channels_1_config.off_mask;
        if( channels_mask != 0 )
        {
            AkaoCollectChannelsVoicesMask( g_channels_1, updated_mask, channels_mask, exclude_mask );
        }
        g_channels_1_config.off_mask = 0;
    }

    updated_mask |= g_channels_3_config.off_mask;
    g_channels_3_config.off_mask = 0;

    if( updated_mask != 0 )
    {
        PsyqSpuSetKey( SPU_OFF, updated_mask );
    }
}



void AkaoUpdateReverbVoices()
{
    u32 updated_mask = 0x0;

//    exclude_mask = ~(g_channels_3_active_mask | g_akao_stream_mask);
//    channels_mask = g_channels_2_config.reverb_mask & w[0x80062f68] & exclude_mask;

//    if( channels_mask != 0 )
//    {
//        system_akao_collect_channels_voices_mask( g_channels_2, updated_mask, channels_mask, exclude_mask );
//    }

//    if( g_akao_control_flags & 0x00000010 )
//    {
//        updated_mask = 0x00ffffff;
//    }
//    else
//    {
        u32 exclude_mask = ~(/*w[0x80062f68] |*/ g_channels_3_config.active_mask | g_akao_stream_mask);
        u32 channels_mask = exclude_mask & g_channels_1_config.reverb_mask;

        if( channels_mask != 0 )
        {
            AkaoCollectChannelsVoicesMask( g_channels_1, updated_mask, channels_mask, exclude_mask );
        }
//    }

    updated_mask |= g_channels_3_config.reverb_mask;

    PsyqSpuSetReverbVoice( SPU_ON, updated_mask );

    updated_mask ^= 0x00ffffff;
    PsyqSpuSetReverbVoice( SPU_OFF, updated_mask );
}



void AkaoUpdatePitchLfoVoices()
{
    u32 updated_mask = 0x0;

//    exclude_mask = ~(g_channels_3_active_mask | g_akao_stream_mask);
//    channels_mask = (g_channels_2_config.pitch_lfo_mask & w[0x80062f68]) & exclude_mask;

//    if( channels_mask != 0 )
//    {
//        system_akao_collect_channels_voices_mask( &g_channels_2, updated_mask, channels_mask, exclude_mask );
//    }

    u32 exclude_mask = ~(/*w[0x80062f68] |*/ g_channels_3_config.active_mask | g_akao_stream_mask);
    u32 channels_mask = exclude_mask & g_channels_1_config.pitch_lfo_mask;

    if( channels_mask != 0 )
    {
        AkaoCollectChannelsVoicesMask( g_channels_1, updated_mask, channels_mask, exclude_mask );
    }

    updated_mask |= g_channels_3_config.pitch_lfo_mask;

    PsyqSpuSetPitchLfoVoice( SPU_ON, updated_mask );

    updated_mask ^= 0x00ffffff;
    PsyqSpuSetPitchLfoVoice( SPU_OFF, updated_mask );
}



void AkaoCollectChannelsVoicesMask( AkaoChannel* channel, u32& ret_mask, u32 channels_mask, u32 exclude_mask )
{
    ret_mask |= channels_mask;

    u32 channel_mask = 0x1;
    while( channels_mask != 0 )
    {
        if( channels_mask & channel_mask )
        {
            // collect overlay voices if they not busy with some other channels
            if( channel->update_flags & AKAO_UPDATE_OVERLAY )
            {
                u32 over_voice_id = channel->over_voice_id;
                if( over_voice_id >= 0x18 ) over_voice_id -= 0x18;
                if( exclude_mask & (1 << over_voice_id) )
                {
                    ret_mask |= 0x1 << over_voice_id;
                }
            }
            // collect alternative voices if they not busy with some other channels
            else if( channel->update_flags & AKAO_UPDATE_ALTERNATIVE )
            {
                if( exclude_mask & (0x1 << channel->alt_voice_id) )
                {
                    ret_mask |= 0x1 << channel->alt_voice_id;
                }
            }

            channels_mask ^= channel_mask;
        }

        channel_mask <<= 1;
        channel += 1;
    }
}



void AkaoMusicUpdateSlideAndDelay( AkaoChannel* channel, AkaoConfig* config, u32 channel_mask )
{
    // update volume slide logic controlled by opcode 0xa9
    if( channel->vol_slide_steps != 0 )
    {
        channel->vol_slide_steps -= 0x1;

        s32 vol_new = channel->volume + channel->vol_slide_step;
        if( (vol_new & 0xffe00000) != (channel->volume & 0xffe00000) )
        {
            channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
        }
        channel->volume = vol_new;
    }

    // update volume balance slide between main and overlay channels logic controlled by opcode 0xf7
    if( channel->over_vol_balance_slide_steps != 0 )
    {
        channel->over_vol_balance_slide_steps -= 1;

        s16 over_vol_balance = channel->over_vol_balance + channel->over_vol_balance_slide_step;
        if( channel->update_flags & AKAO_UPDATE_OVERLAY )
        {
            if( (over_vol_balance & 0xff00) != (channel->over_vol_balance & 0xff00) )
            {
                channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
            }
        }
        channel->over_vol_balance = over_vol_balance;
    }

    // update volume pan slide between left and right volume logic controlled by opcode 0xab
    if( channel->vol_pan_slide_steps != 0 )
    {
        channel->vol_pan_slide_steps -= 0x1;

        u16 vol_pan_new = channel->vol_pan + channel->vol_pan_slide_step;
        if( (vol_pan_new & 0xff00) != (channel->vol_pan & 0xff00) )
        {
            channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
        }
        channel->vol_pan = vol_pan_new;
    }

    // update delay before starting of vibrato effect after start of playing note controlled by opcode 0xb4
    if( channel->vibrato_delay_cur != 0 )
    {
        channel->vibrato_delay_cur -= 1;
    }

    // update delay before starting of tremolo effect after start of playing note controlled by opcode 0xb8
    if( channel->tremolo_delay_cur != 0 )
    {
        channel->tremolo_delay_cur -= 1;
    }

//    if( channel->noise_switch_delay != 0 )
//    {
//        channel->noise_switch_delay -= 1;
//
//        if( channel->noise_switch_delay == 0 )
//        {
//            config->noise_mask ^= channel_mask;
//            g_channels_1_config.update_flags |= AKAO_UPDATE_NOISE_CLOCK;
//            system_akao_update_noise_voices();
//        }
//    }

//    if( channel->pitch_lfo_switch_delay != 0 )
//    {
//        channel->pitch_lfo_switch_delay -= 1;
//
//        if( channel->pitch_lfo_switch_delay == 0 )
//        {
//            config->pitch_lfo_mask ^= channel_mask;
//            AkaoUpdatePitchLfoVoices();
//        }
//    }

    // update vibrato depth slide effect controlled by opcode 0xdd
    if( channel->vibrato_depth_slide_steps != 0 )
    {
        channel->vibrato_depth_slide_steps -= 1;
        channel->vibrato_depth += channel->vibrato_depth_slide_step;

        u32 vibrato_base = ((channel->vibrato_depth & 0x7f00) >> 0x8) * channel->pitch_base;
        if( (channel->vibrato_depth & 0x8000) == 0 ) vibrato_base = (vibrato_base * 0xf) / 0x100;
        channel->vibrato_base = vibrato_base >> 0x7;

        if( channel->vibrato_delay_cur == 0 )
        {
            if( channel->vibrato_rate_cur != 1 )
            {
                if( (g_akao_wave_table[channel->vibrato_wave_id + 0x0] == 0) && (g_akao_wave_table[channel->vibrato_wave_id + 0x1] == 0) )
                {
                    channel->vibrato_wave_id += g_akao_wave_table[channel->vibrato_wave_id + 0x2];
                }

                s16 vibrato_pitch = (channel->vibrato_base * g_akao_wave_table[channel->vibrato_wave_id]) >> 0x10;
                if( vibrato_pitch != channel->vibrato_pitch )
                {
                    channel->vibrato_pitch = vibrato_pitch;
                    channel->attr.mask |= SPU_VOICE_PITCH;

                    if( vibrato_pitch >= 0 ) channel->vibrato_pitch *= 0x2;
                }
            }
        }
    }

    if( channel->tremolo_depth_slide_steps != 0 )
    {
        channel->tremolo_depth_slide_steps -= 1;
        channel->tremolo_depth += channel->tremolo_depth_slide_step;

        if( channel->tremolo_delay_cur == 0 )
        {
            if( channel->tremolo_rate_cur != 1 )
            {
                if( (g_akao_wave_table[channel->tremolo_wave_id + 0x0] == 0) && (g_akao_wave_table[channel->tremolo_wave_id + 0x1] == 0) )
                {
                    channel->tremolo_wave_id += g_akao_wave_table[channel->tremolo_wave_id + 0x2];
                }

                s16 tremolo_vol = (((((((channel->volume >> 0x10) * channel->vol_master) >> 0x7) * (channel->tremolo_depth >> 0x8)) << 0x9) >> 0x10) * g_akao_wave_table[channel->tremolo_wave_id]) >> 0xf;
                if( tremolo_vol != channel->tremolo_vol )
                {
                    channel->tremolo_vol = tremolo_vol;
                    channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
                }
            }
        }
    }

//    if( channel->pan_lfo_depth_slide_steps != 0 )
//    {
//        channel->pan_lfo_depth_slide_steps -= 1;
//
//        channel->pan_lfo_depth += channel->pan_lfo_depth_slide_step;
//
//        if( channel->pan_lfo_rate_cur != 1 )
//        {
//            A0 = channel->pan_lfo_wave;
//            if( (h[A0 + 0x0] == 0 ) && (h[A0 + 2] == 0) )
//            {
//                A0 += h[A0 + 0x4] * 0x2;
//            }
//
//            A1 = ((channel->pan_lfo_depth >> 0x8) * h[A0]) >> 0xf;
//            if( A1 != channel->pan_lfo_vol )
//            {
//                channel->pan_lfo_vol = A1;
//                channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
//            }
//        }
//    }

    // update pitch slide effect controlled by opcode 0xa4
    if( channel->pitch_slide_steps_cur != 0 )
    {
        channel->pitch_slide_steps_cur -= 1;

        s32 pitch_slide = channel->pitch_slide + channel->pitch_slide_step;
        if( (pitch_slide & 0xffff0000) != (channel->pitch_slide & 0xffff0000) )
        {
            channel->attr.mask |= SPU_VOICE_PITCH;
        }
        channel->pitch_slide = pitch_slide;
    }
}



void AkaoSoundUpdateSlideAndDelay( AkaoChannel* channel, u32 channel_mask )
{
//    if( channel->vol_slide_steps != 0 )
//    {
//        channel->vol_slide_steps -= 0x1;
//
//        s32 vol_new = channel->volume + channel->vol_slide_step;
//        if( (vol_new & 0xffe00000) != (channel->volume & 0xffe00000) )
//        {
//            channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
//        }
//        channel->volume = vol_new;
//    }

//    if( channel->noise_switch_delay != 0 )
//    {
//        channel->noise_switch_delay -= 1;
//
//        if( channel->noise_switch_delay == 0 )
//        {
//            g_channels_3_noise_mask ^= channel_mask;
//            g_channels_1_config.update_flags |= AKAO_UPDATE_NOISE_CLOCK;
//
//            system_akao_update_noise_voices();
//        }
//    }

//    if( channel->pitch_lfo_switch_delay != 0 )
//    {
//        channel->pitch_lfo_switch_delay -= 1;
//
//        if( channel->pitch_lfo_switch_delay == 0 )
//        {
//            g_channels_3_pitch_lfo_mask ^= channel_mask;
//
//            system_akao_update_pitch_lfo_voices();
//        }
//    }

//    if( channel->vibrato_depth_slide_steps != 0 )
//    {
//        channel->vibrato_depth_slide_steps -= 1;
//        channel->vibrato_depth += channel->vibrato_depth_slide_step;
//
//        A0 = ((channel->vibrato_depth & 0x7f00) >> 0x8) * channel->pitch_base;
//        if( (channel->vibrato_depth & 0x8000) == 0 ) A0 *= 0xf / 0x100;
//        channel->vibrato_base = A0 >> 0x7;
//
//        if( channel->vibrato_rate_cur != 1 )
//        {
//            A0 = channel->vibrato_wave;
//
//            if( (h[A0 + 0x0] == 0) && (h[A0 + 0x2] == 0) )
//            {
//                A0 += h[A0 + 0x4] * 2;
//            }
//
//            A1 = (channel->vibrato_base * h[A0 + 0x0]) >> 10;
//
//            if( A1 != channel->vibrato_pitch )
//            {
//                channel->vibrato_pitch = A1;
//                channel->attr.mask |= SPU_VOICE_PITCH;
//
//                if( A1 >= 0 )
//                {
//                    channel->vibrato_pitch = A1 * 2;
//                }
//            }
//        }
//    }

//    if( channel->tremolo_depth_slide_steps != 0 )
//    {
//        channel->tremolo_depth_slide_steps -= 1;
//
//        channel->tremolo_depth += channel->tremolo_depth_slide_step;
//
//        if( channel->tremolo_rate_cur != 1 )
//        {
//            A0 = channel->tremolo_wave;
//            if( (h[A0 + 0x0] == 0) && (h[A0 + 0x2] == 0) )
//            {
//                A0 += h[A0 + 0x4] * 2;
//            }
//
//            A1 = (((((((channel->volume >> 0x10) * channel->vol_master) >> 0x7) * (channel->tremolo_depth >> 0x8)) << 0x9) >> 0x10) * h[A0 + 0x0]) >> 0xf;
//
//            if( A1 != channel->tremolo_vol )
//            {
//                channel->tremolo_vol = A1;
//                channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
//            }
//        }
//    }

//    if( channel->pan_lfo_depth_slide_steps != 0 )
//    {
//        channel->pan_lfo_depth_slide_steps -= 1;
//
//        channel->pan_lfo_depth += channel->pan_lfo_depth_slide_step;
//
//        if( channel->pan_lfo_rate_cur != 1 )
//        {
//            A0 = channel->pan_lfo_wave;
//            if( (h[A0 + 0x0] == 0) && (h[A0 + 0x2] == 0) )
//            {
//                A0 += h[A0 + 0x4] * 2;
//            }
//
//            A1 = ((channel->pan_lfo_depth >> 0x8) * h[A0 + 0x0]) >> 0xf;
//
//            if( A1 != channel->pan_lfo_vol )
//            {
//                channel->pan_lfo_vol = A1;
//                channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
//            }
//        }
//    }

//    if( channel->pitch_slide_steps_cur != 0 )
//    {
//        channel->pitch_slide_steps_cur -= 1;
//
//        u32 A1 = channel->pitch_slide + channel->pitch_slide_step;
//        if( (A1 & 0xffff0000) != (channel->pitch_slide & 0xffff0000) )
//        {
//            channel->attr.mask |= SPU_VOICE_PITCH;
//        }
//        channel->pitch_slide = A1;
//    }
}



void AkaoMusicUpdatePitchAndVolume( AkaoChannel* channel, u32 channel_mask, u32 channel_id )
{
    s32 volume_level = ((channel->volume >> 0x10) * channel->vol_master) >> 0x7;

    if( (channel->update_flags & AKAO_UPDATE_VIBRATO) && (channel->vibrato_delay_cur == 0) )
    {
        channel->vibrato_rate_cur -= 1;

        if( channel->vibrato_rate_cur == 0 )
        {
            channel->vibrato_rate_cur = channel->vibrato_rate;

            if( (g_akao_wave_table[channel->vibrato_wave_id + 0x0] == 0) && (g_akao_wave_table[channel->vibrato_wave_id + 0x1] == 0) )
            {
                channel->vibrato_wave_id += g_akao_wave_table[channel->vibrato_wave_id + 0x2];
            }

            s16 vibrato_pitch = (channel->vibrato_base * g_akao_wave_table[channel->vibrato_wave_id]) >> 0x10;
            if( vibrato_pitch != channel->vibrato_pitch )
            {
                channel->vibrato_pitch = vibrato_pitch;
                channel->attr.mask |= SPU_VOICE_PITCH;

                if( vibrato_pitch >= 0 ) channel->vibrato_pitch *= 0x2;
            }

            channel->vibrato_wave_id += 0x1;
        }
    }

    if( (channel->update_flags & AKAO_UPDATE_TREMOLO) && (channel->tremolo_delay_cur == 0) )
    {
        channel->tremolo_rate_cur -= 1;

        if( channel->tremolo_rate_cur == 0 )
        {
            channel->tremolo_rate_cur = channel->tremolo_rate;

            if( (g_akao_wave_table[channel->tremolo_wave_id + 0x0] == 0) && (g_akao_wave_table[channel->tremolo_wave_id + 0x1] == 0) )
            {
                channel->tremolo_wave_id += g_akao_wave_table[channel->tremolo_wave_id + 0x2];
            }

            s16 tremolo_vol = ((((volume_level * (channel->tremolo_depth >> 0x8)) << 0x9) >> 0x10) * g_akao_wave_table[channel->tremolo_wave_id]) >> 0xf;
            if( tremolo_vol != channel->tremolo_vol )
            {
                channel->tremolo_vol = tremolo_vol;
                channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
            }

            channel->tremolo_wave_id += 0x1;
        }
    }

    if( channel->update_flags & AKAO_UPDATE_PAN_LFO )
    {
        channel->pan_lfo_rate_cur -= 1;

        if( channel->pan_lfo_rate_cur == 0 )
        {
            channel->pan_lfo_rate_cur = channel->pan_lfo_rate;

            if( (g_akao_wave_table[channel->pan_lfo_wave_id + 0x0] == 0) && (g_akao_wave_table[channel->pan_lfo_wave_id + 0x1] == 0) )
            {
                channel->pan_lfo_wave_id += g_akao_wave_table[channel->pan_lfo_wave_id + 0x2];
            }

            s16 pan_lfo_vol = ((channel->pan_lfo_depth >> 0x8) * g_akao_wave_table[channel->pan_lfo_wave_id]) >> 0xf;
            if( pan_lfo_vol != channel->pan_lfo_vol )
            {
                channel->pan_lfo_vol = pan_lfo_vol;
                channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
            }

            channel->pan_lfo_wave_id += 0x1;

        }
    }

//    if( channel->update_flags & AKAO_UPDATE_SIDE_CHAIN_VOL )
//    {
//        channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
//        volume_level = ((((channel - 1)->attr.pitch << 0x11) >> 0x10) * channel->vol_master) >> 0x7;
//    }

    if( channel->attr.mask & AKAO_UPDATE_SPU_VOICE )
    {
        volume_level = ((volume_level + channel->tremolo_vol) /* * ((g_akao_vol_mul_music >> 0x10) & 0x7f)*/)/* >> 0x7*/;
//
//        if( hu[0x8009c5a0 + 0x0 * 0xc + 0x8] != 0 )
//        {
//            volume_level = (volume_level * h[0x8009c5a0 + channel_id * 0xc + 0x2]) >> 0x7;
//        }

        u8 vol_pan = ((channel->vol_pan >> 0x8) + channel->pan_lfo_vol) & 0xff;

        if( g_channels_1_config.stereo_mono == AKAO_STEREO )
        {
            channel->attr.vol_l = (volume_level * g_akao_left_volume_table[vol_pan]) >> 0xf;
            channel->attr.vol_r = (volume_level * g_akao_right_volume_table[vol_pan]) >> 0xf;
        }
        else if( g_channels_1_config.stereo_mono == AKAO_STEREO_CHANNELS )
        {
            channel->attr.vol_l = (volume_level * g_akao_left_volume_table[vol_pan]) >> 0xf;
            channel->attr.vol_r = (volume_level * g_akao_right_volume_table[vol_pan]) >> 0xf;

            if( channel_mask & 0x00aaaaaa )
            {
                channel->attr.vol_r = ~channel->attr.vol_r;
            }
            else
            {
                channel->attr.vol_l = ~channel->attr.vol_l;
            }
        }
        else // mono
        {
            channel->attr.vol_l = (volume_level * g_akao_left_volume_table[0x40]) >> 0xf;
            channel->attr.vol_r = (volume_level * g_akao_left_volume_table[0x40]) >> 0xf;
        }
    }

//    if( channel->update_flags & AKAO_UPDATE_SIDE_CHAIN_PITCH )
//    {
//        V1 = channel->vibrato_pitch + (channel->pitch_slide >> 0x10) + (channel - 1)->attr.pitch;
//        u8 pitch_mul_music = (g_akao_pitch_mul_music >> 0x10) & 0xff;
//        if( pitch_mul_music != 0 )
//        {
//            if( pitch_mul_music < 0x80 )
//            {
//                V1 += (V1 * pitch_mul_music) >> 0x7;
//            }
//            else
//            {
//                V1 = (V1 * pitch_mul_music) >> 0x8;
//            }
//        }
//        channel->attr.mask |= SPU_VOICE_PITCH;
//        channel->attr.pitch = V1 & 0x3fff;
//    }
/*else*/if( channel->attr.mask & SPU_VOICE_PITCH )
    {
        u32 pitch_base = channel->pitch_base + channel->vibrato_pitch + (channel->pitch_slide >> 0x10);

//        u8 pitch_mul_music = (g_akao_pitch_mul_music >> 0x10) & 0xff;
//        if( pitch_mul_music != 0 )
//        {
//            if( pitch_mul_music < 0x80 )
//            {
//                pitch_base += (pitch_base * pitch_mul_music) >> 0x7;
//            }
//            else
//            {
//                pitch_base = (pitch_base * pitch_mul_music) >> 0x8;
//            }
//        }
        channel->attr.pitch = pitch_base & 0x3fff;
    }
}



void AkaoSoundUpdatePitchAndVolume( AkaoChannel* channel, u32 channel_mask )
{
    s32 volume_level = ((channel->volume >> 0x10) * channel->vol_master) >> 0x7;

//    if( channel->update_flags & AKAO_UPDATE_VIBRATO )
//    {
//        channel->vibrato_rate_cur -= 1;
//
//        if( channel->vibrato_rate_cur == 0 )
//        {
//            channel->vibrato_rate_cur = channel->vibrato_rate;
//
//            V1 = channel->vibrato_wave;
//
//            if( (h[V1 + 0x0] == 0) && (h[V1 + 0x2] == 0) )
//            {
//                channel->vibrato_wave = V1 + h[V1 + 0x4] * 0x2;
//            }
//
//            V1 = channel->vibrato_wave;
//            channel->vibrato_wave = V1 + 0x2;
//
//            A0 = (channel->vibrato_base * h[V1 + 0x0]) >> 0x10;
//            if( A0 != channel->vibrato_pitch )
//            {
//                channel->vibrato_pitch = A0;
//                if( A0 >= 0 ) channel->vibrato_pitch = A0 * 0x2;
//                channel->attr.mask |= SPU_VOICE_PITCH;
//            }
//        }
//    }

//    if( channel->update_flags & AKAO_UPDATE_TREMOLO )
//    {
//        channel->tremolo_rate_cur -= 1;
//
//        if( channel->tremolo_rate_cur == 0 )
//        {
//            channel->tremolo_rate_cur = channel->tremolo_rate;
//
//            V1 = channel->tremolo_wave;
//            if( (h[V1 + 0x0] == 0) && (h[V1 + 0x2] == 0) )
//            {
//                channel->tremolo_wave = V1 + h[V1 + 0x4] * 0x2;
//            }
//
//            V1 = channel->tremolo_wave;
//            channel->tremolo_wave = V1 + 0x2;
//
//            A0 = ((((volume_level * (channel->tremolo_depth >> 0x8)) << 0x9) >> 0x10) * h[V1 + 0x0]) >> 0xf;
//            if( A0 != channel->tremolo_vol )
//            {
//                channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
//                channel->tremolo_vol = A0;
//            }
//        }
//    }

//    if( channel->update_flags & AKAO_UPDATE_PAN_LFO )
//    {
//        channel->pan_lfo_rate_cur -= 1;
//
//        if( channel->pan_lfo_rate_cur == 0 )
//        {
//            channel->pan_lfo_rate_cur = channel->pan_lfo_rate;
//
//            V1 = channel->pan_lfo_wave;
//            if( (h[V1 + 0x0] == 0) && (h[V1 + 0x2] == 0) )
//            {
//                channel->pan_lfo_wave = V1 + h[V1 + 0x4] * 0x2;
//            }
//
//            V1 = channel->pan_lfo_wave;
//            channel->pan_lfo_wave = V1 + 0x2;
//
//            A0 = ((channel->pan_lfo_depth >> 0x8) * h[V1 + 0x0]) >> 0xf;
//            if( A0 != channel->pan_lfo_vol )
//            {
//                channel->pan_lfo_vol = A0;
//                channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
//            }
//        }
//    }

//    if( channel->update_flags & AKAO_UPDATE_SIDE_CHAIN_VOL )
//    {
//        channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
//        volume_level = ((((channel - 1)->attr.pitch << 0x11) >> 0x10) * channel->vol_master) >> 0x7;
//    }

    if( channel->attr.mask & AKAO_UPDATE_SPU_VOICE )
    {
        volume_level += channel->tremolo_vol;

        u8 vol_pan = 0x40;

        if( channel->type != AKAO_MENU )
        {
            vol_pan = ((channel->vol_pan >> 0x8) + channel->pan_lfo_vol) & 0xff;
            volume_level = (volume_level * (channel->over_vol_balance >> 0x8)) >> 0x7;
        }

        if( g_channels_1_config.stereo_mono == AKAO_STEREO )
        {
            channel->attr.vol_l = (volume_level * g_akao_left_volume_table[vol_pan]) >> 0xf;
            channel->attr.vol_r = (volume_level * g_akao_right_volume_table[vol_pan]) >> 0xf;
        }
        else if( g_channels_1_config.stereo_mono == AKAO_STEREO_CHANNELS )
        {
            channel->attr.vol_l = (volume_level * g_akao_left_volume_table[vol_pan]) >> 0xf;
            channel->attr.vol_r = (volume_level * g_akao_right_volume_table[vol_pan]) >> 0xf;

            if( channel_mask & 0x00aaaaaa )
            {
                channel->attr.vol_r = ~channel->attr.vol_r;
            }
            else
            {
                channel->attr.vol_l = ~channel->attr.vol_l;
            }
        }
        else // mono
        {
            channel->attr.vol_l = (volume_level * g_akao_left_volume_table[0x40]) >> 0xf;
            channel->attr.vol_r = (volume_level * g_akao_left_volume_table[0x40]) >> 0xf;
        }
    }

    u32 pitch_base;

//    if( channel->update_flags & AKAO_UPDATE_SIDE_CHAIN_PITCH )
//    {
//        pitch_base = channel->vibrato_pitch + (channel->pitch_slide >> 0x10) + (channel - 1)->attr.pitch;
//    }
/*    else*/ if( channel->attr.mask & SPU_VOICE_PITCH )
    {
        pitch_base = channel->vibrato_pitch + (channel->pitch_slide >> 0x10) + channel->pitch_base;
    }

    if( /*(channel->update_flags & AKAO_UPDATE_SIDE_CHAIN_PITCH) ||*/ (channel->attr.mask & SPU_VOICE_PITCH) )
    {
//        u8 pitch_mul_sound = (channel->pitch_mul_sound & 0x0000ff00) >> 0x8;
//        if( (channel->type != AKAO_MENU) && (pitch_mul_sound != 0) )
//        {
//            if( pitch_mul_sound < 0x80 )
//            {
//                pitch_base += (pitch_base * pitch_mul_sound) >> 0x7;
//            }
//            else
//            {
//                pitch_base = (pitch_base * pitch_mul_sound) >> 0x8;
//            }
//        }
        channel->attr.mask |= SPU_VOICE_PITCH;
        channel->attr.pitch = pitch_base & 0x3fff;
    }
}



void AkaoUpdateChannelAndOverlayParamsToSpu( AkaoChannel* channel, u32 mask, u32 over_voice_id )
{
    AkaoChannel* channel_2 = &g_channels_1[channel->over_voice_id];

    s16 volume_mod = 0x7f - (channel->over_vol_balance >> 0x8);

    channel_2->attr.vol_l = (channel->attr.vol_l * channel->over_vol_balance) >> 0x10;
    channel->attr.vol_l = (channel->attr.vol_l * volume_mod) >> 0x8;
    channel_2->attr.vol_r = (channel->attr.vol_r * channel->over_vol_balance) >> 0x10;
    channel->attr.vol_r = (channel->attr.vol_r * volume_mod) >> 0x8;

    channel_2->attr.pitch = channel->attr.pitch;
    channel_2->attr.mask |= channel->attr.mask;

    AkaoUpdateChannelParamsToSpu( channel->attr.voice_id, channel->attr );

    if( mask & ( 0x1 << over_voice_id ) )
    {
        AkaoUpdateChannelParamsToSpu( over_voice_id, channel_2->attr );
    }
}



void AkaoUpdateChannelParamsToSpu( u32 voice_id, AkaoVoiceAttr& attr )
{
    if( attr.mask & SPU_VOICE_PITCH )
    {
        PsyqSpuSetVoicePitch( voice_id, attr.pitch );
        attr.mask &= ~SPU_VOICE_PITCH;
    }

    if( attr.mask & AKAO_UPDATE_SPU_VOICE )
    {
        PsyqSpuSetVoiceVolume( voice_id, attr.vol_l, attr.vol_r );
        attr.mask &= ~AKAO_UPDATE_SPU_VOICE;
    }

    if( attr.mask & SPU_VOICE_WDSA )
    {
        PsyqSpuSetVoiceStartAddr( voice_id, attr.addr );
        attr.mask &= ~SPU_VOICE_WDSA;
    }

    if( attr.mask & SPU_VOICE_LSAX )
    {
        PsyqSpuSetVoiceLoopStartAddr( voice_id, attr.loop_addr );
        attr.mask &= ~SPU_VOICE_LSAX;
    }

    if( attr.mask & (SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_SR) )
    {
        PsyqSpuSetVoiceSRAttr( voice_id, attr.sr, attr.s_mode );
        attr.mask &= ~(SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_SR);
    }

    if( attr.mask & (SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_AR) )
    {
        PsyqSpuSetVoiceARAttr( voice_id, attr.ar, attr.a_mode );
        attr.mask &= ~(SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_AR);
    }

    if( attr.mask & (SPU_VOICE_ADSR_RMODE | SPU_VOICE_ADSR_RR) )
    {
        PsyqSpuSetVoiceRRAttr( voice_id, attr.rr, attr.r_mode );
        attr.mask &= ~(SPU_VOICE_ADSR_RMODE | SPU_VOICE_ADSR_RR);
    }

    if( attr.mask & (SPU_VOICE_ADSR_DR | SPU_VOICE_ADSR_SL) )
    {
        PsyqSpuSetVoiceDR( voice_id, attr.dr );
        PsyqSpuSetVoiceSL( voice_id, attr.sl );
        attr.mask &= ~(SPU_VOICE_ADSR_DR | SPU_VOICE_ADSR_SL);
    }
}



u8 AkaoGetNextKey( AkaoChannel* channel )
{
    u8* akao = channel->seq;
    u16 loop_nest = channel->loop_id;

    while( true )
    {
        u8 opcode = READ_LE_U8( akao );

        if( opcode < 0x9a ) // usual note
        {
            if( opcode >= 0x8f )
            {
//                channel->portamento_steps = 0;
                channel->sfx_mask &= ~(AKAO_SFX_FULL_LENGTH | AKAO_SFX_LEGATO);
            }
            return READ_LE_U8( akao );
        }
//
        // unimplemented
        if( opcode < 0xa0 ) return 0xa0;

        u8 op_size[] = { 0x00, 0x02, 0x02, 0x02, 0x03, 0x02, 0x01, 0x01, 0x02, 0x03, 0x02, 0x03, 0x02, 0x02, 0x02, 0x02,
                         0x03, 0x02, 0x02, 0x01, 0x04, 0x02, 0x01, 0x02, 0x04, 0x02, 0x01, 0x02, 0x03, 0x02, 0x01, 0x02,
                         0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x02,
                         0x01, 0x00, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x00, 0x02, 0x03, 0x03, 0x03,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x04, 0x03, 0x04, 0x03, 0x01, 0x00, 0x00,
                         0x00, 0x00, 0x02, 0x01, 0x03, 0x01, 0x02, 0x03, 0x02, 0x01, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00 };
        u8 size = op_size[opcode - 0xa0];

        if( size != 0 )
        {
            akao += size;
        }
        else
        {
            switch( opcode )
            {
                case 0xc9: // loop_return_times
                {
                    akao += 0x1;

                    if( (READ_LE_U8( akao ) + 0x1) != channel->loop_times[loop_nest] )
                    {
                        akao = channel->loop_point[loop_nest] ;
                    }
                    else
                    {
                        akao += 0x1;
                        loop_nest = (loop_nest - 1) & 0x3;
                    }
                }
                break;

                case 0xca: // loop_return
                {
                    akao = channel->loop_point[loop_nest];
                }
                break;

                case 0xcb: // sfx_reset
                case 0xcd: // legato_off
                case 0xd1: // full_length_off
                case 0xdb: // portamento_off
                {
                    akao += 0x1;
//                    channel->portamento_steps = 0;
                    channel->sfx_mask &= ~(AKAO_SFX_FULL_LENGTH | AKAO_SFX_LEGATO);
                }
                break;

                case 0xee: // jump
                {
                    akao += 0x1;
                    akao += READ_LE_S16( akao ) + 0x2;
                }
                break;

                case 0xef: // jump_conditional
                {
                    akao += 0x1;
//                    A2 = READ_LE_U8( akao );
                    akao += 0x1;
//                    if( g_channels_1_config.condition >= A2 )
//                    {
//                        akao += h[akao] + 0x2;
//                    }
//                    else
//                    {
                        akao += 0x2;
//                    }
                }
                break;

                case 0xf0: // loop_jump_times
                case 0xf1: // loop_break_times
                {
                    akao += 0x1;
                    if( READ_LE_U8( akao ) == (channel->loop_times[loop_nest] + 1) )
                    {
                        akao += 0x1;
                        akao += READ_LE_S16( akao ) + 0x2;
                        loop_nest = (loop_nest - 1) & 0x3;
                    }
                    else
                    {
                        akao += 0x3;
                    }
                }
                break;

                default:
                {
//                    channel->portamento_steps = 0;
                    channel->sfx_mask &= ~(AKAO_SFX_FULL_LENGTH | AKAO_SFX_LEGATO);
                    return 0xa0;
                }
            }
        }
    }
}
