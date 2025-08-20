#include "akao.h"
#include "../psyq/libapi.h"
#include "../psyq/libspu.h"



u32 g_akao_sound_event;
// u32 g_akao_control_flags;               // 0x80062ff8
// u32 g_akao_effects_all;                 // 0x80062f74
// u32 g_akao_effects_all_seq;             // 0x80062f84
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
// AkaoConfig g_channels_2_config;      // 0x8009a164

// AkaoChannel g_channels_3[0x8];          // 0x80099788
// u32 g_channels_3_on_mask;               // 0x80099fd0
// u32 g_channels_3_off_mask;              // 0x80099fd8
// u32 g_channels_3_active_mask;           // 0x80099fcc
// u32 g_channels_3_active_mask_stored;    // 0x80099fdc
// u32 g_channels_3_noise_mask;            // 0x80099fec
// u32 g_channels_3_reverb_mask;           // 0x80099ff0
// u32 g_channels_3_pitch_lfo_mask;        // 0x80099ff4
// u16 g_channels_3_noise_clock;           // 0x80099ffa

// SpuCommonAttr g_spu_common_attr;        // 0x8009c578
SpuReverbAttr g_spu_reverb_attr;
// AkaoVoiceAttr g_akao_voice_attr;        // 0x8007ebe4

AkaoInstrument g_akao_instrument[0x80];
u8 g_effect_all[0xc800];
u8* g_akao_music;
u32 g_akao_music_size;

u8 g_akao_default_sound[0x20] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

u16 g_akao_length_table[0xb] =
{
    0xc0c0, 0x6060, 0x3030, 0x1818, 0x0c0c, 0x0606, 0x0303, 0x2020, 0x1010, 0x0808, 0x0404
};

s16 g_akao_left_volume_table[ 0x100 ] =
{
    0x7f80, 0x7e80, 0x7d80, 0x7c80, 0x7b80, 0x7a80, 0x7980, 0x7880, 0x7780, 0x7680, 0x7580, 0x7480, 0x7380, 0x7280, 0x7180, 0x7080,
    0x6f80, 0x6e80, 0x6d80, 0x6c80, 0x6b80, 0x6a80, 0x6980, 0x6880, 0x6780, 0x6680, 0x6580, 0x6480, 0x6380, 0x6280, 0x6180, 0x6080,
    0x5f80, 0x5e80, 0x5d80, 0x5c80, 0x5b80, 0x5a80, 0x5980, 0x5880, 0x5780, 0x5680, 0x5580, 0x5480, 0x5380, 0x5280, 0x5180, 0x5080,
    0x4f80, 0x4e80, 0x4d80, 0x4c80, 0x4b80, 0x4a80, 0x4980, 0x4880, 0x4780, 0x4680, 0x4580, 0x4480, 0x4380, 0x4280, 0x4180, 0x4080,
    0x3f80, 0x3e80, 0x3d80, 0x3c80, 0x3b80, 0x3a80, 0x3980, 0x3880, 0x3780, 0x3680, 0x3580, 0x3480, 0x3380, 0x3280, 0x3180, 0x3080,
    0x2f80, 0x2e80, 0x2d80, 0x2c80, 0x2b80, 0x2a80, 0x2980, 0x2880, 0x2780, 0x2680, 0x2580, 0x2480, 0x2380, 0x2280, 0x2180, 0x2080,
    0x1f80, 0x1e80, 0x1d80, 0x1c80, 0x1b80, 0x1a80, 0x1980, 0x1880, 0x1780, 0x1680, 0x1580, 0x1480, 0x1380, 0x1280, 0x1180, 0x1080,
    0x0f80, 0x0e80, 0x0d80, 0x0c80, 0x0b80, 0x0a80, 0x0980, 0x0880, 0x0780, 0x0680, 0x0580, 0x0480, 0x0380, 0x0280, 0x0180, 0x0080,
    0x0000, 0xffff, 0xfffc, 0xfff7, 0xfff0, 0xffe7, 0xffdc, 0xffcf, 0xffc0, 0xffaf, 0xff9c, 0xff87, 0xff70, 0xff57, 0xff3c, 0xff1f,
    0xff00, 0xfedf, 0xfebc, 0xfe97, 0xfe70, 0xfe47, 0xfe1c, 0xfdef, 0xfdc0, 0xfd8f, 0xfd5c, 0xfd27, 0xfcf0, 0xfcb7, 0xfc7c, 0xfc3f,
    0xfc00, 0xfbbf, 0xfb7c, 0xfb37, 0xfaf0, 0xfaa7, 0xfa5c, 0xfa0f, 0xf9c0, 0xf96f, 0xf91c, 0xf8c7, 0xf870, 0xf817, 0xf7bc, 0xf75f,
    0xf700, 0xf69f, 0xf63c, 0xf5d7, 0xf570, 0xf507, 0xf49c, 0xf42f, 0xf3c0, 0xf34f, 0xf2dc, 0xf267, 0xf1f0, 0xf177, 0xf0fc, 0xf07f,
    0x1000, 0x1081, 0x1104, 0x1189, 0x1210, 0x1299, 0x1324, 0x13b1, 0x1440, 0x14d1, 0x1564, 0x15f9, 0x1690, 0x1729, 0x17c4, 0x1861,
    0x1900, 0x19a1, 0x1a44, 0x1ae9, 0x1b90, 0x1c39, 0x1ce4, 0x1d91, 0x1e40, 0x1ef1, 0x1fa4, 0x2059, 0x2110, 0x21c9, 0x2284, 0x2341,
    0x2400, 0x24c1, 0x2584, 0x2649, 0x2710, 0x27d9, 0x28a4, 0x2971, 0x2a40, 0x2b11, 0x2be4, 0x2cb9, 0x2d90, 0x2e69, 0x2f44, 0x3021,
    0x3100, 0x31e1, 0x32c4, 0x33a9, 0x3490, 0x3579, 0x3664, 0x3751, 0x3840, 0x3931, 0x3a24, 0x3b19, 0x3c10, 0x3d09, 0x3e04, 0x3f01
};

s16 g_akao_right_volume_table[ 0x100 ] =
{
    0x0080, 0x0180, 0x0280, 0x0380, 0x0480, 0x0580, 0x0680, 0x0780, 0x0880, 0x0980, 0x0a80, 0x0b80, 0x0c80, 0x0d80, 0x0e80, 0x0f80,
    0x1080, 0x1180, 0x1280, 0x1380, 0x1480, 0x1580, 0x1680, 0x1780, 0x1880, 0x1980, 0x1a80, 0x1b80, 0x1c80, 0x1d80, 0x1e80, 0x1f80,
    0x2080, 0x2180, 0x2280, 0x2380, 0x2480, 0x2580, 0x2680, 0x2780, 0x2880, 0x2980, 0x2a80, 0x2b80, 0x2c80, 0x2d80, 0x2e80, 0x2f80,
    0x3080, 0x3180, 0x3280, 0x3380, 0x3480, 0x3580, 0x3680, 0x3780, 0x3880, 0x3980, 0x3a80, 0x3b80, 0x3c80, 0x3d80, 0x3e80, 0x3f80,
    0x4080, 0x4180, 0x4280, 0x4380, 0x4480, 0x4580, 0x4680, 0x4780, 0x4880, 0x4980, 0x4a80, 0x4b80, 0x4c80, 0x4d80, 0x4e80, 0x4f80,
    0x5080, 0x5180, 0x5280, 0x5380, 0x5480, 0x5580, 0x5680, 0x5780, 0x5880, 0x5980, 0x5a80, 0x5b80, 0x5c80, 0x5d80, 0x5e80, 0x5f80,
    0x6080, 0x6180, 0x6280, 0x6380, 0x6480, 0x6580, 0x6680, 0x6780, 0x6880, 0x6980, 0x6a80, 0x6b80, 0x6c80, 0x6d80, 0x6e80, 0x6f80,
    0x7080, 0x7180, 0x7280, 0x7380, 0x7480, 0x7580, 0x7680, 0x7780, 0x7880, 0x7980, 0x7a80, 0x7b80, 0x7c80, 0x7d80, 0x7e80, 0x7f80,
    0x3f01, 0x3e04, 0x3d09, 0x3c10, 0x3b19, 0x3a24, 0x3931, 0x3840, 0x3751, 0x3664, 0x3579, 0x3490, 0x33a9, 0x32c4, 0x31e1, 0x3100,
    0x3021, 0x2f44, 0x2e69, 0x2d90, 0x2cb9, 0x2be4, 0x2b11, 0x2a40, 0x2971, 0x28a4, 0x27d9, 0x2710, 0x2649, 0x2584, 0x24c1, 0x2400,
    0x2341, 0x2284, 0x21c9, 0x2110, 0x2059, 0x1fa4, 0x1ef1, 0x1e40, 0x1d91, 0x1ce4, 0x1c39, 0x1b90, 0x1ae9, 0x1a44, 0x19a1, 0x1900,
    0x1861, 0x17c4, 0x1729, 0x1690, 0x15f9, 0x1564, 0x14d1, 0x1440, 0x13dc, 0x1324, 0x1299, 0x1210, 0x1189, 0x1104, 0x1081, 0x1000,
    0xf07f, 0xf0fc, 0xf177, 0xf1f0, 0xf267, 0xf2dc, 0xf34f, 0xf3c0, 0xf42f, 0xf49c, 0xf507, 0xf570, 0xf5d7, 0xf63c, 0xf69f, 0xf700,
    0xf75f, 0xf7bc, 0xf817, 0xf870, 0xf8c7, 0xf91c, 0xf843, 0xf9c0, 0xfa0f, 0xfa5c, 0xfaa7, 0xfaf0, 0xfb37, 0xfb7c, 0xfbbf, 0xfc00,
    0xfc3f, 0xfc7c, 0xfcb7, 0xfcf0, 0xfd27, 0xfd5c, 0xfd8f, 0xfdc0, 0xfdef, 0xfe1c, 0xfe47, 0xfe70, 0xfe97, 0xfebc, 0xfedf, 0xff00,
    0xff1f, 0xff3c, 0xff57, 0xff70, 0xff87, 0xff9c, 0xffaf, 0xffc0, 0xffcf, 0xffdc, 0xffe7, 0xfff0, 0xfff7, 0xfffc, 0xffff, 0x0000
};

u32 g_akao_wave_table_key[ 0x10 ] =
{
    0x0, 0xc, 0x12, 0x1c, 0x22, 0x2c, 0x32, 0xd2, 0x168, 0x1ac, 0xfc, 0x150, 0x1c0, 0x1ac, 0x168, 0x1ac
};

s16 g_akao_wave_table[ 0x2c4 ] =
{
    // type 0x0 (id 0x0)
    0x1fff, 0xe001, 0x3fff, 0xc001, 0x5fff, 0xa001, 0x7fff, 0x8001, 0x0000, 0x0000, 0xfffe, 0x0000,
    // type 0x1 (id 0xc)
                                                                                                    0x7fff, 0x8001, 0x0000, 0x0000, // 0x0
    0xfffe, 0x0000,
    // type 0x2 (id 0x12)
                    0x1fff, 0x0000, 0x3fff, 0x0000, 0x5fff, 0x0000, 0x7fff, 0x0000, 0x0000, 0xfffe,
    // type 0x3 (id 0x1c)
                                                                                                    0x7fff, 0x0000, 0x7fff, 0x0000, // 0x10
    0x0000, 0xfffe,
    // type 0x4 (id 0x22)
                    0xe001, 0x0000, 0xc001, 0x0000, 0xa001, 0x0000, 0x8001, 0x0000, 0x0000, 0xfffe,
    // type 0x5 (id 0x2c)
                                                                                                    0x8001, 0x0000, 0x8001, 0x0000, // 0x20
    0x0000, 0xfffe,
    // type 0x6 (id 0x32)
                    0x0000, 0x0500, 0x09e2, 0x12cd, 0x169e, 0x19e1, 0x1c81, 0x1e6d, 0x1f9a, 0x1fff, 0x1f9c, 0x1e71, 0x1c86, 0x19e8, // 0x30
    0x16a7, 0x12d7, 0x0e90, 0x09ee, 0x050d, 0x0000, 0xfb0c, 0xf62a, 0xf186, 0xed3d, 0xe96a, 0xe626, 0xe384, 0xe196, 0xe067, 0xe000, // 0x40
    0xe061, 0xe18a, 0xe373, 0xe60f, 0xe94f, 0xed1d, 0xf163, 0xf604, 0xfae5, 0x0000, 0x0a01, 0x13c4, 0x259a, 0x2d3c, 0x33c2, 0x3902, // 0x50
    0x3cda, 0x3f34, 0x3fff, 0x3f38, 0x3ce2, 0x390d, 0x33d1, 0x2d4f, 0x25af, 0x1d21, 0x13dd, 0x0a1b, 0x0000, 0xf618, 0xec54, 0xe30c, // 0x60
    0xda7b, 0xd2d5, 0xcc4d, 0xc709, 0xc32d, 0xc0cf, 0xc000, 0xc0c3, 0xc315, 0xc6e6, 0xcc1f, 0xd29e, 0xda3b, 0xe2c6, 0xec09, 0xf5ca, // 0x70
    0x0000, 0x0f02, 0x1da6, 0x3867, 0x43da, 0x4da3, 0x5583, 0x5b47, 0x5ece, 0x5fff, 0x5ed4, 0x5b53, 0x5594, 0x4dba, 0x43f6, 0x3886, // 0x80
    0x2bb2, 0x1dcb, 0x0f29, 0x0000, 0xf124, 0xe27e, 0xd492, 0xc7b8, 0xbc40, 0xb273, 0xaa8e, 0xa4c3, 0xa137, 0xa000, 0xa124, 0xa49f, // 0x90
    0xaa59, 0xb22e, 0xbbed, 0xc759, 0xd429, 0xe20e, 0xf0af, 0x0000, 0x1403, 0x2788, 0x4b34, 0x5a79, 0x6784, 0x7204, 0x79b5, 0x7e68, // 0xa0
    0x7fff, 0x7e71, 0x79c5, 0x721b, 0x67a3, 0x5a9e, 0x4b5e, 0x3a43, 0x27ba, 0x1437, 0x0000, 0xec30, 0xd8a8, 0xc619, 0xb4f6, 0xa5ab, // 0xb0
    0x989a, 0x8e13, 0x865a, 0x819f, 0x8000, 0x8186, 0x862a, 0x8dcc, 0x983e, 0xa53d, 0xb477, 0xc58d, 0xd813, 0xeb95, 0x0000, 0x0000, // 0xc0
    0xffd9, 0x0000,
    // type 0x7 (id 0xd2)
                    0x0000, 0x1403, 0x2788, 0x4b34, 0x5a79, 0x6784, 0x7204, 0x79b5, 0x7e68, 0x7fff, 0x7e71, 0x79c5, 0x721b, 0x67a3, // 0xd0
    0x5a9e, 0x4b5e, 0x3a43, 0x27ba, 0x1437, 0x0000, 0xec30, 0xd8a8, 0xc619, 0xb4f6, 0xa5ab, 0x989a, 0x8e13, 0x865a, 0x819f, 0x8000, // 0xe0
    0x8186, 0x862a, 0x8dcc, 0x983e, 0xa53d, 0xb477, 0xc58d, 0xd813, 0xeb95, 0x0000, 0x0000, 0xffd9,
    // type 0xa (id 0xfc)
                                                                                                    0x0000, 0x09e2, 0x12cd, 0x19e1, // 0xf0
    0x1e6d, 0x1fff, 0x1e71, 0x19e8, 0x12d7, 0x09ee, 0x000d, 0xf62a, 0xed3d, 0xe626, 0xe196, 0xe000, 0xe18a, 0xe60f, 0xed1d, 0xf604, // 0x100
    0x0000, 0x13c4, 0x259a, 0x33c2, 0x3cda, 0x3fff, 0x3ce2, 0x33d1, 0x25af, 0x13dd, 0x001a, 0xec54, 0xda7b, 0xcc4d, 0xc32d, 0xc000, // 0x110
    0xc315, 0xcc1f, 0xda3b, 0xec09, 0x0000, 0x1da6, 0x3867, 0x4da3, 0x5b47, 0x5fff, 0x5b53, 0x4dba, 0x3886, 0x1dcb, 0x0027, 0xe27e, // 0x120
    0xc7b8, 0xb273, 0xa4c3, 0xa000, 0xa49f, 0xb22e, 0xc759, 0xe20e, 0x0000, 0x2788, 0x4b34, 0x6784, 0x79b5, 0x7fff, 0x79c5, 0x67a3, // 0x130
    0x4b5e, 0x27ba, 0x0034, 0xd8a8, 0xb4f6, 0x989a, 0x865a, 0x8000, 0x862a, 0x983e, 0xb477, 0xd813, 0x0000, 0x0000, 0xffec, 0x0000, // 0x140
    // type 0xb (id 0x150)
    0x0000, 0x2788, 0x4b34, 0x6784, 0x79b5, 0x7fff, 0x79c5, 0x67a3, 0x4b5e, 0x27ba, 0x0034, 0xd8a8, 0xb4f6, 0x989a, 0x865a, 0x8000, // 0x150
    0x862a, 0x983e, 0xb477, 0xd813, 0x0000, 0x0000, 0xffec, 0x0000,
    // type 0x8 0xe (id 0x168)
                                                                    0x0000, 0x07ff, 0x0fff, 0x17ff, 0x1fff, 0x17ff, 0x0fff, 0x07ff, // 0x160
    0x0000, 0xf801, 0xf001, 0xe801, 0xe001, 0xe801, 0xf001, 0xf801, 0x0000, 0x0fff, 0x1fff, 0x2fff, 0x3fff, 0x2fff, 0x1fff, 0x0fff, // 0x170
    0x0000, 0xf001, 0xe001, 0xd001, 0xc001, 0xd001, 0xe001, 0xf001, 0x0000, 0x17ff, 0x2fff, 0x47ff, 0x5fff, 0x47ff, 0x2fff, 0x17ff, // 0x180
    0x0000, 0xe801, 0xd001, 0xb801, 0xa001, 0xb801, 0xd001, 0xe801, 0x0000, 0x1fff, 0x3fff, 0x5fff, 0x7fff, 0x5fff, 0x3fff, 0x1fff, // 0x190
    0x0000, 0xe001, 0xc001, 0xa001, 0x8001, 0xa001, 0xc001, 0xe001, 0x0000, 0x0000, 0xfff0, 0x0000,
    // type 0x9 0xd 0xf (id 0x1ac)
                                                                                                    0x0000, 0x1fff, 0x3fff, 0x5fff, // 0x1a0
    0x7fff, 0x5fff, 0x3fff, 0x1fff, 0x0000, 0xe001, 0xc001, 0xa001, 0x8001, 0xa001, 0xc001, 0xe001, 0x0000, 0x0000, 0xfff0, 0x0000, // 0x1b0
    // type 0xc (id 0x1c0)
    0x0000, 0x31fd, 0x7d05, 0xf5fd, 0xecee, 0x793c, 0x75d0, 0x07fc, 0xf411, 0xc2f5, 0x660f, 0x330e, 0x3fea, 0xbee1, 0x8c12, 0x1821, // 0x1c0
    0xaffb, 0x4a00, 0xd3e7, 0xf4f5, 0xe137, 0xe7d3, 0x520d, 0x1103, 0xaedf, 0x4af8, 0x0746, 0x70f4, 0x8ec1, 0x9719, 0x3512, 0xf600, // 0x1d0
    0x1b00, 0xb4e9, 0x3b1e, 0xae15, 0x9cc8, 0x65f9, 0xc323, 0x71f8, 0x2cf1, 0xa604, 0x92fc, 0xc90e, 0xb009, 0x10e5, 0x0804, 0x8001, // 0x1e0
    0x12fa, 0x1118, 0xe709, 0x47f8, 0x95d9, 0xdd1c, 0x1b09, 0x0bf4, 0x7bfc, 0x20ff, 0x9404, 0x7dfc, 0x8af5, 0xb717, 0x74f9, 0x2afc, // 0x1f0
    0xdd06, 0xd7f3, 0xf4fd, 0x0608, 0xf218, 0x69df, 0xbc07, 0x4af7, 0x89eb, 0xbb27, 0x1109, 0x7fef, 0x3615, 0xeccd, 0x7621, 0x9815, // 0x200
    0x56cf, 0xd406, 0x3322, 0xdfe6, 0xaf0a, 0xb9f5, 0xce08, 0x5d05, 0xbfee, 0xf01a, 0x20e8, 0x9f16, 0x8cf2, 0xc2e7, 0xd22e, 0xc601, // 0x210
    0xb9d0, 0x810c, 0x4323, 0x28dd, 0x96f7, 0x5c3c, 0x9fde, 0xaee8, 0xb61b, 0x3ef3, 0x1408, 0xdae9, 0x2c0b, 0xb133, 0xa8e1, 0x05da, // 0x220
    0x0914, 0x4e22, 0xffe8, 0x87f1, 0x21f0, 0x8d25, 0x7dfc, 0x1cf0, 0x5e11, 0x3de8, 0xc70a, 0x8f17, 0xfff0, 0x6a0d, 0xffe3, 0x12f3, // 0x230
    0x041f, 0x780c, 0xc4ef, 0x5b02, 0x3d02, 0x02f2, 0x1519, 0xdad5, 0xbd09, 0x9219, 0xf503, 0x62dd, 0x3806, 0x0f26, 0xa8fb, 0xe0fe, // 0x240
    0xf6e3, 0x0e02, 0x0210, 0x4406, 0x94d4, 0xd10a, 0xa846, 0x10f1, 0x1acc, 0xbdfd, 0x1a30, 0xa0eb, 0x5fea, 0xbf10, 0xbbef, 0xdf1b, // 0x250
    0xc20c, 0x2ae0, 0x3ef2, 0xae27, 0x5001, 0xf9dd, 0x3611, 0xad1a, 0x07df, 0x6d05, 0x1a0d, 0xb6db, 0x6d2b, 0x3607, 0xfbcc, 0x4208, // 0x260
    0x0f21, 0x36f6, 0xc806, 0x0603, 0xfdd7, 0x7817, 0x0605, 0xaf0b, 0x95fc, 0x10ec, 0xa70e, 0xadf3, 0x660f, 0xbdf2, 0xdef8, 0xe421, // 0x270
    0xb1e7, 0xfced, 0x5b1c, 0xa109, 0xccf2, 0xf5f3, 0x9afb, 0xb417, 0x92e8, 0x7f08, 0x4bfa, 0x89fd, 0x3c1c, 0xfcef, 0xf2db, 0x4a1d, // 0x280
    0x671d, 0xd6ff, 0x65da, 0xf1fc, 0x6e0a, 0x0a0c, 0x11f8, 0x4a02, 0x5bfc, 0xeb06, 0xa5ed, 0x30e6, 0x8011, 0xd234, 0x08f8, 0x43dc, // 0x290
    0xf609, 0xa6f2, 0x9a0d, 0x7006, 0xf5ea, 0x9cfc, 0x811e, 0xade6, 0xe60e, 0x95f7, 0xb304, 0x9cff, 0xefea, 0xf519, 0xfa00, 0x3af6, // 0x2a0
    0xc7d5, 0x2722, 0xbb38, 0x43d1, 0x77ed, 0x4908, 0xce02, 0xab0f, 0x3deb, 0xdb0a, 0x6401, 0x3ff9, 0x76f2, 0x9309, 0xd20f, 0x74e5, // 0x2b0
    0x0000, 0x0000, 0xff00, 0x0000
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



void AkaoInit( u8* instr_all, u8* instr_dat )
{
    AkaoLoadInstr( instr_all, instr_dat );

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
    memcpy( g_effect_all, effect_all, 0xc800 );
}



void AkaoLoadInstr( u8* instr_all, u8* instr_dat )
{
    PsyqSpuSetTransferStartAddr( READ_LE_U32( instr_all + 0x0 ) );
    PsyqSpuWrite( instr_all + 0x10, READ_LE_U32( instr_all + 0x4 ) );

    for( int i = 0; i < 0x80; ++i )
    {
        READ_LE_U32( instr_dat + i * 0x40 + 0x0 );
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



void AkaoInitData()
{
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

//    // masks
//    g_channels_3_active_mask = 0;
//    g_channels_3_on_mask = 0;
//    [0x80099fd4] = w(0);
//    g_channels_3_off_mask = 0;
//    g_channels_3_active_mask_stored = 0;
//    [0x80099fe0] = w(0x66a80000);
//    [0x80099fe8] = w(1);
//    g_channels_3_noise_mask = 0;
//    g_channels_3_reverb_mask = 0;
//    g_channels_3_pitch_lfo_mask = 0;

    g_channels_1_config.stereo_mono = AKAO_STEREO;
    g_channels_1_config.active_mask = 0;
    g_channels_1_config.on_mask = 0;
//    [0x8009a104 + 0xc] = w(0);
    g_channels_1_config.off_mask = 0;
//    g_channels_1_config.active_mask_stored = 0;
//    g_channels_1_config.noise_mask = 0;
    g_channels_1_config.reverb_mask = 0;
//    g_channels_1_config.pitch_lfo_mask = 0;
//    g_channels_1_config.reverb_mode = 0;
    g_channels_1_config.reverb_depth = 0;
//    g_channels_1_config.music_id = 0;
//    g_channels_1_config.reverb_depth_slide_steps = 0;
//    g_channels_1_config.timer_upper = 0;
//    g_channels_1_config.timer_upper_cur = 0;
//    g_channels_1_config.timer_lower = 0;
//    g_channels_1_config.timer_top_cur = 0;

//    g_channels_2_config.active_mask = 0;
//    g_channels_2_config.music_id = 0;

//    g_spu_common_attr.mask = 0x00003fcf;
//    g_spu_common_attr.mvol.left = 0x3fff;
//    g_spu_common_attr.mvol.right = 0x3fff;
//    g_spu_common_attr.mvolmode.left = 0;
//    g_spu_common_attr.mvolmode.right = 0;
//    g_spu_common_attr.cd.volume.left = 0x7fff;
//    g_spu_common_attr.cd.volume.right = 0x7fff;
//    g_spu_common_attr.cd.reverb = 0;
//    g_spu_common_attr.cd.mix = 1;
//    g_spu_common_attr.ext.volume.left = 0;
//    g_spu_common_attr.ext.volume.right = 0;
//    g_spu_common_attr.ext.reverb = 0;
//    g_spu_common_attr.ext.mix = 0;

//    system_psyq_spu_set_common_attr( g_spu_common_attr );

    for( int i = 0; i < 0x18; ++i )
    {
//        [0x8009c5a0 + i * 0xc + 0x0] = w(0x7f0000);
//        [0x8009c5a0 + i * 0xc + 0x4] = w(0);
//        [0x8009c5a0 + i * 0xc + 0x8] = h(0);
//
        g_channels_1[i].update_flags = 0;
//        [0x80096608 + i * 0x108 + 0x50] = w(0);
        g_channels_1[i].type = AKAO_MUSIC;
        g_channels_1[i].attr.voice_id = i;
//
        PsyqSpuSetVoiceVolumeAttr( i, 0, 0, 0, 0 );
    }

//    // init sounds channels
//    for( int i = 0; i < 0x8; ++i )
//    {
//        g_channels_3[i].alt_voice_id = i + 0x10;
//        g_channels_3[i].update_flags = 0;
//        g_channels_3[i].pitch_mul_sound = 0;
//        [0x80099788 + i * 0x108 + 0x50] = w(0);
//        g_channels_3[i].type = AKAO_SOUND;
//        g_channels_3[i].pitch_mul_sound_slide_steps = 0;
//        g_channels_3[i].vol_balance_slide_steps = 0;
//        g_channels_3[i].vol_balance = 0x7f00;
//        g_channels_3[i].attr.voice_id = i + 0x10;
//    }
}



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

    if( g_channels_1_config.active_mask != 0 )
    {
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
                    channel->length_1 -= 0x1;
                    channel->length_2 -= 0x1;

                    if( channel->length_1 == 0 )
                    {
                        AkaoExecuteSequence( channel, &g_channels_1_config, channel_mask );
                    }
                    else if( channel->length_2 == 0 )
                    {
                        channel->length_2 |= 0x1;
//                        [0x8009a104 + 0xc] = w(w[0x8009a104 + 0xc] & ~channel_mask);
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
//
//            if( g_channels_1_config.reverb_depth_slide_steps != 0 )
//            {
//                g_channels_1_config.reverb_depth_slide_steps -= 1;
//                g_channels_1_config.reverb_depth += g_channels_1_config.reverb_depth_slide_step;
//                g_channels_1_config.update_flags |= AKAO_UPDATE_REVERB;
//            }
//
//            if( g_channels_1_config.timer_lower != 0 )
//            {
//                g_channels_1_config.timer_lower_cur += 1;
//
//                if( g_channels_1_config.timer_lower_cur == g_channels_1_config.timer_lower )
//                {
//                    g_channels_1_config.timer_lower_cur = 0;
//                    g_channels_1_config.timer_upper_cur += 1;
//
//                    if( g_channels_1_config.timer_upper_cur == g_channels_1_config.timer_upper )
//                    {
//                        g_channels_1_config.timer_upper_cur = 0;
//                        g_channels_1_config.timer_top_cur += 1;
//                    }
//                }
//            }
        }
    }
//
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
//                    channel->length_1 -= 0x1;
//                    channel->length_2 -= 0x1;
//
//                    if( channel->length_1 == 0 )
//                    {
//                        system_akao_execute_sequence( channel, 0x8009a164, channel_mask )
//                    }
//                    else
//                    {
//                        if( channel->length_2 == 0 )
//                        {
//                            channel->length_2 |= 0x1;
//                            [0x8009a164 + 0xc] = w(w[0x8009a164 + 0xc] & ~channel_mask);
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
//
//    if( g_channels_3_active_mask != 0 )
//    {
//        [0x80099fe8] = w(w[0x80099fe8] + (hu[0x80099fe0] >> 0x10));
//
//        if( (w[0x80099fe8] & 0xffff0000) || (g_akao_control_flags & 0x00000004) )
//        {
//            [0x80099fe8] = w(w[0x80099fe8] & 0x0000ffff);
//
//            channel_mask = 0x10000;
//            u32 channels_mask = g_channels_3_active_mask;
//            AkaoChannel* channel = g_channels_3;
//            while( channels_mask != 0 )
//            {
//                if( channels_mask & channel_mask )
//                {
//                    if( ((g_akao_control_flags & 0x00000002) == 0) || (channel->type == AKAO_MENU) )
//                    {
//                        [channel + 0x50] = w(w[channel + 0x50] + 0x1);
//
//                        channel->length_1 -= 0x1;
//                        channel->length_2 -= 0x1;
//
//                        if( channel->length_1 == 0 )
//                        {
//                            system_akao_execute_sequence( channel, &g_channels_2_config, channel_mask );
//                        }
//                        else if( channel->length_2 == 0 )
//                        {
//                            channel->length_2 |= 0x1;
//                            [0x80099fd4] = w(w[0x80099fd4] & ~channel_mask);
//                            g_channels_3_off_mask |= channel_mask;
//                        }
//
//                        system_akao_sound_update_slide_and_delay( channel, channel_mask );
//                    }
//
//                    channels_mask ^= channel_mask;
//                }
//
//                channel += 1;
//                channel_mask <<= 1;
//            }
//        }
//    }
//
//    if( g_channels_1_config.mute_music != 0 )
//    {
//        system_akao_command_9b( 0x8009a000 ); // mute music
//        g_channels_1_config.mute_music = 0;
//    }
//
//    system_akao_execute_commands_queue();
//
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
//    [0x8009a104 + 0xc] = w(0);
    g_channels_1_config.off_mask |= 0x00ffffff;
    g_channels_1_config.tempo = 0xffff0000;
    g_channels_1_config.tempo_update = 0x1;
//    g_channels_1_config.over_mask = 0;
//    g_channels_1_config.alt_mask = 0;
//    g_channels_1_config.noise_mask = 0;
    g_channels_1_config.reverb_mask = 0;
//    g_channels_1_config.pitch_lfo_mask = 0;
    g_channels_1_config.update_flags = 0;
    g_channels_1_config.reverb_depth = 0;
//    g_channels_1_config.reverb_depth_slide_step = 0;
//    g_channels_1_config.tempo_slide_steps = 0;
//    g_channels_1_config.condition_stored = 0;
//    g_channels_1_config.condition = 0;
//    g_channels_1_config.reverb_depth_slide_steps = 0;
//    g_channels_1_config.mute_music = 0;
//    g_channels_1_config.timer_upper_cur = 0;
//    g_channels_1_config.timer_lower = 0;
//    g_channels_1_config.timer_lower_cur = 0;
//    g_channels_1_config.timer_top_cur = 0;

    AkaoChannel* channel = g_channels_1;
    u32 channel_mask = 0x1;
    while( channels_mask != 0 )
    {
        if( channels_mask & channel_mask )
        {
            channel->seq = akao + 0x2 + READ_LE_U16( akao );
            // for debug purpose
            {
                channel->seq_start = channel->seq;
                if( (channels_mask ^ channel_mask) != 0 )
                {
                    channel->seq_end = akao + 0x4 + READ_LE_U16( akao + 0x2 ) - 1;
                }
                else
                {
                    channel->seq_end = g_akao_music + g_akao_music_size - 1;
                }
            }

            channel->vol_master = 0x7f;
            channel->length_1 = 0x3;
            channel->length_2 = 0x1;

            AkaoInstrInit( channel, 0x14 );

            channel->drum_offset = g_akao_music;
            channel->pitch_slide = 0;
            channel->update_flags = 0;
            channel->volume = 0x3fff0000;
            channel->vol_slide_steps = 0;
//            channel->vol_balance_slide_steps = 0;
            channel->vol_pan = 0x4000;
            channel->vol_pan_slide_steps = 0;
            channel->pitch_slide_steps_cur = 0;
//            channel->portamento_steps = 0;
            channel->sfx_mask = 0;
            channel->vibrato_depth = 0;
//            channel->vibrato_depth_slide_steps = 0;
            channel->tremolo_depth = 0;
//            channel->tremolo_depth_slide_steps = 0;
//            channel->pan_lfo_depth = 0;
//            channel->pan_lfo_depth_slide_steps = 0;
//            channel->noise_switch_delay = 0;
//            channel->pitch_lfo_switch_delay = 0;
            channel->loop_id = 0;
            channel->length_stored = 0;
            channel->length_fixed = 0;
//            channel->vol_balance = 0x4000;
            channel->transpose = 0;
            channel->fine_tuning = 0;
            channel->key_add = 0;
//            channel->pan_lfo_vol = 0;
//
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
//
//    system_akao_update_noise_voices();
    AkaoUpdateReverbVoices();
//    system_akao_update_pitch_lfo_voices();
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
    while( opcode != 0xa0 )
    {
        u8* akao = channel->seq;
        channel->seq = akao + 0x1;

        opcode = READ_LE_U8( akao );

        //ofLog(OF_LOG_NOTICE, "channel" + ofToHex(channel->attr.voice_id) + ": opcode " + ofToHex((int)opcode));

        if( opcode < 0xa0 ) break;

        akao_opcodes[opcode - 0xa0]( channel, config, mask );
    }

    if( opcode != 0xa0 )
    {
//        if( channel->length_fixed != 0 )
//        {
//            channel->length_1 = channel->length_fixed;
//            channel->length_2 = channel->length_fixed;
//        }

        if( channel->length_1 == 0 )
        {
            u8 length_id = opcode % 0xb;
            channel->length_1 = g_akao_length_table[length_id] & 0x00ff;
            channel->length_2 = (g_akao_length_table[length_id] & 0xff00) >> 0x8;
        }

        if( ((AkaoGetNextKey( channel ) - 0x84) >= 0xb) && ((channel->sfx_mask & (AKAO_SFX_FULL_LENGTH | AKAO_SFX_LEGATO)) == 0) )
        {
            channel->length_2 -= 0x2;
        }

        channel->length_stored = channel->length_1;

        if( opcode >= 0x8f )
        {
//            channel->portamento_steps = 0;
            channel->vibrato_pitch = 0;
            channel->tremolo_vol = 0;
            channel->sfx_mask &= ~AKAO_SFX_LEGATO_ACT;
        }
        else if( opcode < 0x84 )
        {
            u32 pitch_base = 0;
            u8 semitone = opcode / 0xb;

            if( channel->update_flags & AKAO_UPDATE_DRUM_MODE )
            {
                if( channel->type == AKAO_MUSIC )
                {
                    config->on_mask |= mask;
                }
//                else
//                {
//                    g_channels_3_on_mask |= mask;
//                }
//
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

//                    if( (channel->update_flags & AKAO_UPDATE_ALTERNATIVE) == 0 )
//                    {
//                        channel->attr.r_mode = g_akao_instrument[instr_id].r_mode;
//                        channel->attr.rr = g_akao_instrument[instr_id].rr;
//                        channel->attr.mask |= SPU_VOICE_ADSR_RMODE | SPU_VOICE_ADSR_RR;
//                    }
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
                u8 key = channel->octave * 0xc + semitone;

//                if( ( channel->portamento_steps != 0 ) && ( channel->key_stored != 0 ) )
//                {
//                    channel->pitch_slide_steps = channel->portamento_steps;
//                    channel->key_add = (key & 0xff) + channel->transpose - channel->key_stored - channel->transpose_stored;
//                    channel->key = channel->key_stored - channel->transpose - channel->transpose_stored;
//                    key = (channel->key_stored & 0xff) + (channel->transpose_stored & 0xff);
//                }
//                else
                {
                    channel->key = key;
                    key += channel->transpose & 0xff;
                }

                if( (channel->sfx_mask & AKAO_SFX_LEGATO_ACT) == 0 )
                {
                    if( channel->type == AKAO_MUSIC )
                    {
                        config->on_mask |= mask;
//
//                        if( channel->update_flags & AKAO_UPDATE_OVERLAY )
//                        {
//                            V1 = channel->over_voice_id;
//                            if( channel->over_voice_id >= 0x18 ) V1 -= 0x18;
//                            config->on_mask |= 0x1 << V1;
//                        }
                    }
//                    else
//                    {
//                        g_channels_3_on_mask |= mask;
//                    }
                    channel->pitch_slide_steps_cur = 0;
                }

                u8 semitone = key % 0xc;
                pitch_base = g_akao_instrument[channel->instr_id].pitch[semitone];

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

//            if( channel->type == AKAO_MUSIC )
//            {
//                [config + 0xc] = w(w[config + 0xc] | mask);
//            }
//            else
//            {
//                [0x80099fd4] = w(w[0x80099fd4] | mask);
//            }

            channel->attr.mask |= AKAO_UPDATE_SPU_VOICE | SPU_VOICE_PITCH;

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

            if( channel->update_flags & AKAO_UPDATE_VIBRATO )
            {
                u32 pitch_base = channel->pitch_base * ((channel->vibrato_depth & 0x7f00) >> 0x8);
                if( (channel->vibrato_depth & 0x8000) == 0 ) pitch_base *= 0xf / 0x100;
                channel->vibrato_base = pitch_base >> 0x7;
                channel->vibrato_wave_id = g_akao_wave_table_key[channel->vibrato_type];
                channel->vibrato_delay_cur = channel->vibrato_delay;
                channel->vibrato_rate_cur = 1;
            }

            if( channel->update_flags & AKAO_UPDATE_TREMOLO )
            {
                channel->tremolo_wave_id = g_akao_wave_table_key[channel->tremolo_type];
                channel->tremolo_delay_cur = channel->tremolo_delay;
                channel->tremolo_rate_cur = 1;
            }

//            if( channel->update_flags & AKAO_UPDATE_PAN_LFO )
//            {
//                channel->pan_lfo_rate_cur = 1;
//                channel->pan_lfo_wave = w[0x8004a5cc + channel->pan_lfo_type * 0x4];
//            }

            channel->vibrato_pitch = 0;
            channel->tremolo_vol = 0;
            channel->pitch_slide = 0;
        }

        channel->sfx_mask &= ~AKAO_SFX_LEGATO_ACT;
        channel->sfx_mask |= (channel->sfx_mask & AKAO_SFX_LEGATO) << 1;

        if( channel->key_add != 0 )
        {
            //ofLog( OF_LOG_NOTICE, "channel->key: " + ofToString( channel->key ) );
            //ofLog( OF_LOG_NOTICE, "channel->key_add: " + ofToString( channel->key_add ) );

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

            //ofLog( OF_LOG_NOTICE, "pitch: " + ofToString( pitch >> 0x10 ) );
            //ofLog( OF_LOG_NOTICE, "channel->pitch_base: " + ofToString( channel->pitch_base ) );
            //ofLog( OF_LOG_NOTICE, "channel->pitch_slide: " + ofToString( channel->pitch_slide >> 0x10 ) );
            //ofLog( OF_LOG_NOTICE, "channel->pitch_slide_step: " + ofToString( channel->pitch_slide_step >> 0x10 ) );
        }

        channel->transpose_stored = channel->transpose;
        channel->key_stored = channel->key;
    }
}



void AkaoUpdateKeysOn()
{
    u32 updated_mask = 0;

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
//
//    if( g_channels_2_config.active_mask != 0 )
//    {
//        AkaoChannel* channel = g_channels_2;
//        channel_mask = 0x1;
//        channel_id = 0;
//
//        S5 = ~(g_channels_3_active_mask | g_akao_stream_mask) & w[0x80062f68];
//        channels_mask = g_channels_2_config.active_mask & S5 & w[0x8009a164 + 0xc];
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
//                        system_akao_update_channel_and_overlay_params_to_spu( channel, S5, channel->over_voice_id - 0x18 );
//                    }
//                    else if( (channel->update_flags & AKAO_UPDATE_ALTERNATIVE) == 0 )
//                    {
//                        AkaoUpdateChannelParamsToSpu( channel->attr.voice_id, channel->attr );
//                    }
//                    else
//                    {
//                        if( channel_mask & g_channels_2_config.on_mask )
//                        {
//                            channel->update_flags ^= 0x00000400;
//                            channel->attr.mask |= AKAO_UPDATE_SPU_ALL;
//                        }
//
//                        if( channel->update_flags & 0x00000400 )
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

    if( g_channels_1_config.active_mask != 0 )
    {
//        S5 = ~(w[0x80062f68] | g_channels_3_active_mask | g_akao_stream_mask);
        updated_mask |= /*S5 &*/ g_channels_1_config.on_mask;

        u32 channel_id = 0;

        u32 channel_mask = 1;
        u32 channels_mask = g_channels_1_config.active_mask /*& (S5 & w[0x8009a104 + 0xc])*/;
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
//
//                    if( channel->update_flags & AKAO_UPDATE_OVERLAY )
//                    {
//                        system_akao_update_channel_and_overlay_params_to_spu( channel, S5, channel->over_voice_id );
//                    }
//                    else if( channel->update_flags & AKAO_UPDATE_ALTERNATIVE )
//                    {
//                        if( g_channels_1_config.on_mask & channel_mask )
//                        {
//                            channel->update_flags ^= 0x00000400;
//                            channel->attr.mask |= AKAO_UPDATE_SPU_ALL;
//                        }
//
//                        if( channel->update_flags & 0x00000400 )
//                        {
//                            if( S5 & ( 1 << channel->alt_voice_id ) )
//                            {
//                                AkaoUpdateChannelParamsToSpu( channel->alt_voice_id, channel->attr );
//
//                                if( updated_mask & channel_mask )
//                                {
//                                    updated_mask = (updated_mask | (1 << channel->alt_voice_id)) & ~channel_mask;
//                                }
//                            }
//                            else
//                            {
//                                AkaoUpdateChannelParamsToSpu( channel->attr.voice_id, channel->attr );
//                            }
//                        }
//                        else
//                        {
//                            AkaoUpdateChannelParamsToSpu( channel->attr.voice_id, channel->attr );
//                        }
//                    }
//                    else
//                    {
                        AkaoUpdateChannelParamsToSpu( channel->attr.voice_id, channel->attr );
//                    }
                }

                channels_mask ^= channel_mask;
            }

            channel_mask <<= 1;
            channel += 1;
            channel_id += 1;
        }

        g_channels_1_config.on_mask = 0;
    }

//    if( g_channels_3_active_mask != 0 )
//    {
//        u32 channel_mask = 0x00010000;
//        AkaoChannel* channel = g_channels_3;
//        updated_mask |= g_channels_3_on_mask;
//        u32 channels_mask = g_channels_3_active_mask & w[0x80099fd4];
//
//        while( channels_mask != 0 )
//        {
//            if( channels_mask & channel_mask )
//            {
//                system_akao_sound_update_pitch_and_volume( channel, channel_mask );
//
//                if( channel->attr.mask != 0 )
//                {
//                    AkaoUpdateChannelParamsToSpu( channel->attr.voice_id, channel->attr );
//                }
//
//                channels_mask ^= channel_mask;
//            }
//
//            channel_mask <<= 0x1;
//            channel += 1;
//        }
//
//        g_channels_3_on_mask = 0;
//    }

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
//        collect_mask = ~(g_channels_3_active_mask | g_akao_stream_mask);
//        channels_mask = w[0x8009a164 + 0x10] & w[0x80062f68] & collect_mask;
//        if( channels_mask != 0 )
//        {
//            system_akao_collect_channels_voices_mask( g_channels_2, updated_mask, channels_mask, collect_mask );
//        }
//        g_channels_2_config.off_mask = 0;
//    }

    if( g_channels_1_config.active_mask != 0 )
    {
        u32 collect_mask = ~(/*w[0x80062f68] | g_channels_3_active_mask |*/ g_akao_stream_mask);
        u32 channels_mask = collect_mask & g_channels_1_config.off_mask;
        if( channels_mask != 0 )
        {
            AkaoCollectChannelsVoicesMask( g_channels_1, updated_mask, channels_mask, collect_mask );
        }
        g_channels_1_config.off_mask = 0;
    }
//
//    updated_mask |= g_channels_3_off_mask;
//    g_channels_3_off_mask = 0;

    if( updated_mask != 0 )
    {
        PsyqSpuSetKey( SPU_OFF, updated_mask );
    }
}



void AkaoUpdateReverbVoices()
{
    u32 updated_mask = 0x0;

//    collect_mask = ~(g_channels_3_active_mask | g_akao_stream_mask);
//    channels_mask = g_channels_2_config.reverb_mask & w[0x80062f68] & collect_mask;
//
//    if( channels_mask != 0 )
//    {
//        system_akao_collect_channels_voices_mask( g_channels_2, updated_mask, channels_mask, collect_mask );
//    }
//
//    if( g_akao_control_flags & 0x00000010 )
//    {
//        updated_mask = 0x00ffffff;
//    }
//    else
//    {
        u32 collect_mask = ~(/*w[0x80062f68] | g_channels_3_active_mask |*/ g_akao_stream_mask);
        u32 channels_mask = collect_mask & g_channels_1_config.reverb_mask;

        if( channels_mask != 0 )
        {
            AkaoCollectChannelsVoicesMask( g_channels_1, updated_mask, channels_mask, collect_mask );
        }
//    }

//    updated_mask |= g_channels_3_reverb_mask;

    PsyqSpuSetReverbVoice( SPU_ON, updated_mask );

    updated_mask ^= 0x00ffffff;
    PsyqSpuSetReverbVoice( SPU_OFF, updated_mask );
}



void AkaoCollectChannelsVoicesMask( AkaoChannel* channel, u32& ret_mask, u32 channels_mask, u32 collect_mask )
{
    ret_mask |= channels_mask;

//    channel_mask = 0x1;
//    while( channels_mask != 0 )
//    {
//        if( channels_mask & channel_mask )
//        {
//            if( channel->update_flags & AKAO_UPDATE_OVERLAY )
//            {
//                V1 = channel->over_voice_id;
//                if( V1 >= 0x18 ) V1 -= 0x18;
//                if( collect_mask & (1 << V1) )
//                {
//                    ret_mask |= 0x1 << V1;
//                }
//            }
//            else if( channel->update_flags & AKAO_UPDATE_ALTERNATIVE )
//            {
//                if( collect_mask & (0x1 << channel->alt_voice_id) )
//                {
//                    ret_mask |= 0x1 << channel->alt_voice_id;
//                }
//            }
//
//            channels_mask ^= channel_mask;
//        }
//
//        channel_mask <<= 1;
//        channel += 1;
//    }
}



void AkaoMusicUpdateSlideAndDelay( AkaoChannel* channel, AkaoConfig* config, u32 channel_mask )
{
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

//    if( channel->vol_balance_slide_steps != 0 )
//    {
//        channel->vol_balance_slide_steps -= 1;
//
//        A1 = channel->vol_balance + channel->vol_balance_slide_step;
//        if( channel->update_flags & AKAO_UPDATE_OVERLAY )
//        {
//            if( (A1 & 0xff00) != (channel->vol_balance & 0xff00) )
//            {
//                channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
//            }
//        }
//        channel->vol_balance = A1;
//    }

    if( channel->vol_pan_slide_steps != 0 )
    {
        channel->vol_pan_slide_steps == 0x1;

        u16 vol_pan_new = channel->vol_pan + channel->vol_pan_slide_step;
        if( (vol_pan_new & 0xff00) != (channel->vol_pan & 0xff00) )
        {
            channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
        }
        channel->vol_pan = vol_pan_new;
    }

    if( channel->vibrato_delay_cur != 0 )
    {
        channel->vibrato_delay_cur -= 1;
    }

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
//
//    if( channel->pitch_lfo_switch_delay != 0 )
//    {
//        channel->pitch_lfo_switch_delay -= 1;
//
//        if( channel->pitch_lfo_switch_delay == 0 )
//        {
//            config->pitch_lfo_mask ^= channel_mask;
//            system_akao_update_pitch_lfo_voices();
//        }
//    }
//
//    if( channel->vibrato_depth_slide_steps != 0 )
//    {
//        channel->vibrato_depth_slide_steps -= 1;
//        channel->vibrato_depth += channel->vibrato_depth_slide_step;
//
//        A0 = ((channel->vibrato_depth & 0x7f00) >> 0x8) * channel->pitch_base;
//        if( (channel->vibrato_depth & 0x8000) == 0 ) A0 *= 0xf / 0x100;
//        channel->vibrato_base = A0 >> 0x7;
//
//        if( channel->vibrato_delay_cur == 0 )
//        {
//            if( channel->vibrato_rate_cur != 1 )
//            {
//                A0 = channel->vibrato_wave;
//                if( (h[A0 + 0x0] == 0) && (h[A0 + 2] == 0) )
//                {
//                    A0 += h[A0 + 0x4] * 0x2;
//                }
//
//                A1 = (channel->vibrato_base * h[A0 + 0x0]) >> 0x10;
//                if( A1 != channel->vibrato_pitch )
//                {
//                    channel->vibrato_pitch = A1;
//                    channel->attr.mask |= SPU_VOICE_PITCH;
//
//                    if( A1 >= 0 ) channel->vibrato_pitch = A1 * 0x2;
//                }
//            }
//        }
//    }
//
//    if( channel->tremolo_depth_slide_steps != 0 )
//    {
//        channel->tremolo_depth_slide_steps -= 1;
//
//        channel->tremolo_depth += channel->tremolo_depth_slide_step;
//
//        if( channel->tremolo_delay_cur == 0 )
//        {
//            if( channel->tremolo_rate_cur != 1 )
//            {
//                A0 = channel->tremolo_wave;
//                if( (h[A0 + 0x0] == 0) && (h[A0 + 0x2] == 0) )
//                {
//                    A0 += h[A0 + 0x4] * 0x2;
//                }
//
//                A1 = (((((((channel->volume >> 0x10) * channel->vol_master) >> 0x7) * (channel->tremolo_depth >> 0x8)) << 0x9) >> 0x10) * h[A0 + 0x0]) >> 0xf;
//                if( A1 != channel->tremolo_vol )
//                {
//                    channel->tremolo_vol = A1;
//                    channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
//                }
//            }
//        }
//    }
//
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

            s16 vibrato_pitch = (channel->vibrato_base * g_akao_wave_table[channel->vibrato_wave_id + 0x0]) >> 0x10;
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

            s16 tremolo_vol = ((((volume_level * (channel->tremolo_depth >> 0x8)) << 0x9) >> 0x10) * g_akao_wave_table[channel->tremolo_wave_id + 0x0]) >> 0xf;
            if( tremolo_vol != channel->tremolo_vol )
            {
                channel->tremolo_vol = tremolo_vol;
                channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
            }

            channel->tremolo_wave_id += 0x1;
        }
    }

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
//            A3 = ((channel->pan_lfo_depth >> 0x8) * h[V1 + 0x0]) >> 0xf;
//            if( A3 != channel->pan_lfo_vol )
//            {
//                channel->pan_lfo_vol = A3;
//                channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
//            }
//        }
//    }
//
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

        u8 vol_pan = ((channel->vol_pan >> 0x8) /*+ channel->pan_lfo_vol*/) & 0xff;

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
        //ofLog( OF_LOG_NOTICE, "Update pitch channel->pitch_slide: " + ofToString( (channel->pitch_slide >> 0x10) ) );

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
