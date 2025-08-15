#include "akao.h"
#include "../psyq/libapi.h"
#include "../psyq/libspu.h"

#include "akao_opcodes.h"
#include "akao_commands.h"



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

s16 akao_left_volume_table[ 256 ] =
{
    32640, 32384, 32128, 31872, 31616, 31360, 31104, 30848, 30592, 30336, 30080, 29824, 29568, 29312, 29056, 28800,
    28544, 28288, 28032, 27776, 27520, 27264, 27008, 26752, 26496, 26240, 25984, 25728, 25472, 25216, 24960, 24704,
    24448, 24192, 23936, 23680, 23424, 23168, 22912, 22656, 22400, 22144, 21888, 21632, 21376, 21120, 20864, 20608,
    20352, 20096, 19840, 19584, 19328, 19072, 18816, 18560, 18304, 18048, 17792, 17536, 17280, 17024, 16768, 16512,
    16256, 16000, 15744, 15488, 15232, 14976, 14720, 14464, 14208, 13952, 13696, 13440, 13184, 12928, 12672, 12416,
    12160, 11904, 11648, 11392, 11136, 10880, 10624, 10368, 10112,  9856,  9600,  9344,  9088,  8832,  8576,  8320,
     8064,  7808,  7552,  7296,  7040,  6784,  6528,  6272,  6016,  5760,  5504,  5248,  4992,  4736,  4480,  4224,
     3968,  3712,  3456,  3200,  2944,  2688,  2432,  2176,  1920,  1664,  1408,  1152,   896,   640,   384,   128,
        0,    -1,    -4,    -9,   -16,   -25,   -36,   -49,   -64,   -81,  -100,  -121,  -144,  -169,  -196,  -225,
     -256,  -289,  -324,  -361,  -400,  -441,  -484,  -529,  -576,  -625,  -676,  -729,  -784,  -841,  -900,  -961,
    -1024, -1089, -1156, -1225, -1296, -1369, -1444, -1521, -1600, -1681, -1764, -1849, -1936, -2025, -2116, -2209,
    -2304, -2401, -2500, -2601, -2704, -2809, -2916, -3025, -3136, -3249, -3364, -3481, -3600, -3721, -3844, -3969,
     4096,  4225,  4356,  4489,  4624,  4761,  4900,  5041,  5184,  5329,  5476,  5625,  5776,  5929,  6084,  6241,
     6400,  6561,  6724,  6889,  7056,  7225,  7396,  7569,  7744,  7921,  8100,  8281,  8464,  8649,  8836,  9025,
     9216,  9409,  9604,  9801, 10000, 10201, 10404, 10609, 10816, 11025, 11236, 11449, 11664, 11881, 12100, 12321,
    12544, 12769, 12996, 13225, 13456, 13689, 13924, 14161, 14400, 14641, 14884, 15129, 15376, 15625, 15876, 16129
};

s16 akao_right_volume_table[ 256 ] =
{
      128,   384,   640,   896,  1152,  1408,  1664,  1920,  2176,  2432,  2688,  2944,  3200,  3456,  3712,  3968,
     4224,  4480,  4736,  4992,  5248,  5504,  5760,  6016,  6272,  6528,  6784,  7040,  7296,  7552,  7808,  8064,
     8320,  8576,  8832,  9088,  9344,  9600,  9856, 10112, 10368, 10624, 10880, 11136, 11392, 11648, 11904, 12160,
    12416, 12672, 12928, 13184, 13440, 13696, 13952, 14208, 14464, 14720, 14976, 15232, 15488, 15744, 16000, 16256,
    16512, 16768, 17024, 17280, 17536, 17792, 18048, 18304, 18560, 18816, 19072, 19328, 19584, 19840, 20096, 20352,
    20608, 20864, 21120, 21376, 21632, 21888, 22144, 22400, 22656, 22912, 23168, 23424, 23680, 23936, 24192, 24448,
    24704, 24960, 25216, 25472, 25728, 25984, 26240, 26496, 26752, 27008, 27264, 27520, 27776, 28032, 28288, 28544,
    28800, 29056, 29312, 29568, 29824, 30080, 30336, 30592, 30848, 31104, 31360, 31616, 31872, 32128, 32384, 32640,
    16129, 15876, 15625, 15376, 15129, 14884, 14641, 14400, 14161, 13924, 13689, 13456, 13225, 12996, 12769, 12544,
    12321, 12100, 11881, 11664, 11449, 11236, 11025, 10816, 10609, 10404, 10201, 10000,  9801,  9604,  9409,  9216,
     9025,  8836,  8649,  8464,  8281,  8100,  7921,  7744,  7569,  7396,  7225,  7056,  6889,  6724,  6561,  6400,
     6241,  6084,  5929,  5776,  5625,  5476,  5329,  5184,  5084,  4900,  4761,  4624,  4489,  4356,  4225,  4096,
    -3969, -3844, -3721, -3600, -3481, -3364, -3249, -3136, -3025, -2916, -2809, -2704, -2601, -2500, -2401, -2304,
    -2209, -2116, -2025, -1936, -1849, -1764, -1981, -1600, -1521, -1444, -1369, -1296, -1225, -1156, -1089, -1024,
     -961,  -900,  -841,  -784,  -729,  -676,  -625,  -576,  -529,  -484,  -441,  -400,  -361,  -324,  -289,  -256,
     -225,  -196,  -169,  -144,  -121,  -100,   -81,   -64,   -49,   -36,   -25,   -16,    -9,    -4,    -1,     0
};

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

//    // akao channels config
//    g_channels_1_config.stereo_mono = AKAO_STEREO;
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
//        g_channels_1[i].update_flags = 0;
//        [0x80096608 + i * 0x108 + 0x50] = w(0);
//        g_channels_1[i].type = AKAO_MUSIC;
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

//                    func2e478( channel, g_channels_1_config, channel_mask );

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
//                    func2e478( channel, g_channels_2_config, channel_mask );
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
//                        func2e954( channel, channel_mask );
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
//    g_channels_1_config.update_flags = 0;
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
            channel->vol_master = 0x7f;
            channel->length_1 = 0x3;
            channel->length_2 = 0x1;

            AkaoInstrInit( channel, 0x14 );

//            channel->drum_offset = 0x80083580;
//            channel->pitch_slide = 0;
//            channel->update_flags = 0;
            channel->volume = 0x3fff0000;
//            channel->vol_slide_steps = 0;
//            channel->vol_balance_slide_steps = 0;
//            channel->vol_pan = 0x4000;
//            channel->vol_pan_slide_steps = 0;
//            channel->pitch_slide_steps_cur = 0;
//            channel->portamento_steps = 0;
//            channel->sfx_mask = 0;
//            channel->vibrato_depth = 0;
//            channel->vibrato_depth_slide_steps = 0;
//            channel->tremolo_depth = 0;
//            channel->tremolo_depth_slide_steps = 0;
//            channel->pan_lfo_depth = 0;
//            channel->pan_lfo_depth_slide_steps = 0;
//            channel->noise_switch_delay = 0;
//            channel->pitch_lfo_switch_delay = 0;
//            channel->loop_id = 0;
            channel->length_stored = 0;
            channel->length_fixed = 0;
//            channel->vol_balance = 0x4000;
//            channel->transpose = 0;
//            channel->fine_tuning = 0;
//            channel->pitch_slide_dst = 0;
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
    ofLog( OF_LOG_NOTICE, "instr_id: 0x" +  ofToHex( instr_id ) );

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

        if( opcode < 0xa0 ) break;

        //ofLog( OF_LOG_NOTICE, "channel" +  ofToHex( channel->attr.voice_id ) + ": opcode " +  ofToHex( (int)opcode ) );

        akao_opcodes[opcode - 0xa0]( channel, config, mask );
    }

    if( opcode != 0xa0 )
    {
//        u8 next_note = system_akao_get_next_note( channel );
//
//        if( channel->length_fixed != 0 )
//        {
//            channel->length_1 = channel->length_fixed;
//            channel->length_2 = channel->length_fixed;
//        }

        if( channel->length_1 == 0 )
        {
            channel->length_1 = g_akao_length_table[opcode % 0xb] & 0x00ff;
            channel->length_2 = (g_akao_length_table[opcode % 0xb] & 0xff00) >> 0x8;
        }

//        if( ((next_note - 0x84) >= 0xb) && (channel->sfx_mask & (AKAO_SFX_FULL_LENGTH | AKAO_SFX_LEGATO)) == 0) )
//        {
//            channel->length_2 -= 0x2;
//        }

        channel->length_stored = channel->length_1;

//        if( opcode >= 0x8f )
//        {
//            channel->portamento_steps = 0;
//            channel->vibrato_pitch = 0;
//            channel->tremolo_vol = 0;
//            channel->sfx_mask &= ~0x0002;
//        }
/*else*/if( opcode < 0x84 )
        {
//            if( channel->update_flags & AKAO_UPDATE_DRUM_MODE )
//            {
//                if( channel->type == AKAO_MUSIC )
//                {
//                    config->on_mask |= mask;
//                }
//                else
//                {
//                    g_channels_3_on_mask |= mask;
//                }
//
//                S2 = opcode / 0xb;
//                V1 = S2 / 0xc;
//                A0 = S2 % 0xc
//
//                A2 = channel->drum_offset + A0 * 0x5;
//                u8 instr_id = bu[A2 + 0x0];
//
//                if( instr_id != channel->instr_id )
//                {
//                    channel->instr_id = instr_id;
//                    channel->attr.addr = g_akao_instrument[instr_id].addr;
//                    channel->attr.loop_addr = g_akao_instrument[instr_id].loop_addr;
//                    channel->attr.a_mode = g_akao_instrument[instr_id].a_mode;
//                    channel->attr.s_mode = g_akao_instrument[instr_id].s_mode;
//                    channel->attr.ar = g_akao_instrument[instr_id].ar;
//                    channel->attr.dr = g_akao_instrument[instr_id].dr;
//                    channel->attr.sl = g_akao_instrument[instr_id].sl;
//                    channel->attr.sr = g_akao_instrument[instr_id].sr;
//                    channel->attr.mask |= AKAO_UPDATE_SPU_BASE_WOR;
//
//                    if( (channel->update_flags & AKAO_UPDATE_ALTERNATIVE) == 0 )
//                    {
//                        channel->attr.r_mode = g_akao_instrument[instr_id].r_mode;
//                        channel->attr.rr = g_akao_instrument[instr_id].rr;
//                        channel->attr.mask |= SPU_VOICE_ADSR_RMODE | SPU_VOICE_ADSR_RR;
//                    }
//                }
//
//                V1 = bu[A2 + 0x1];
//
//                A1 = S2 / 0xc;
//                A0 = g_akao_instrument[instr_id].pitch[S2 % 0xc];
//
//                if( A1 >= 0x7 )
//                {
//                    A0 <<= A1 - 0x6;
//                }
//                else if( A1 < 0x6 )
//                {
//                    A0 >>= 0x6 - A1;
//                }
//
//                channel->volume = hu[A2 + 0x2] << 0x10;
//                channel->vol_pan = bu[A2 + 0x4] << 0x8;
//            }
//            else
//            {

                u8 S2 = channel->octave * 0xc + opcode / 0xb;

//                if( ( channel->portamento_steps != 0 ) && ( hu[channel + 0x6a] != 0 ) )
//                {
//                    channel->pitch_slide_steps = channel->portamento_steps;
//                    channel->pitch_slide_dst = (S2 & 0xff) + channel->transpose - hu[channel + 0x6a] - hu[channel + 0xd4];
//                    [channel + 0xd0] = h(hu[channel + 0x6a] - channel->transpose - hu[channel + 0xd4]);
//                    S2 = bu[channel + 0x6a] + bu[channel + 0xd4];
//                }
//                else
//                {
//                    [channel + 0xd0] = h(S2 & 0xff);
//                    S2 += channel->transpose & 0xff;
//                }
//
//                if( ( channel->sfx_mask & 0x0002 ) == 0 )
//                {
//                    if( channel->type == AKAO_MUSIC )
//                    {
                        config->on_mask |= mask;
//
//                        if( channel->update_flags & AKAO_UPDATE_OVERLAY )
//                        {
//                            V1 = channel->over_voice_id;
//                            if( channel->over_voice_id >= 0x18 ) V1 -= 0x18;
//                            config->on_mask |= 0x1 << V1;
//                        }
//                    }
//                    else
//                    {
//                        g_channels_3_on_mask |= mask;
//                    }
//                    channel->pitch_slide_steps_cur = 0;
//                }
//
                u8 mod = S2 / 0xc;

                u32 pitch_base = g_akao_instrument[channel->instr_id].pitch[S2 % 0xc];
                if( mod >= 0x7 )
                {
                    pitch_base <<= mod - 0x6;
                }
                else if( mod < 0x6 )
                {
                    pitch_base >>= 0x6 - mod;
                }
//            }
//
//            if( channel->type == AKAO_MUSIC )
//            {
//                [config + 0xc] = w(w[config + 0xc] | mask);
//            }
//            else
//            {
//                [0x80099fd4] = w(w[0x80099fd4] | mask);
//            }

            channel->attr.mask |= AKAO_UPDATE_SPU_VOICE | SPU_VOICE_PITCH;

//            V1 = channel->fine_tuning;
//            if( V1 != 0 )
//            {
//                if( V1 > 0 )
//                {
//                    V0 = (pitch_base * V1) >> 0x7;
//                }
//                else
//                {
//                    V0 = (pitch_base * V1) >> 0x8;
//                }
//
//                pitch_base = (pitch_base + V0) & 0xffff;
//            }
            channel->pitch_base = pitch_base;

//            if( channel->update_flags & AKAO_UPDATE_VIBRATO )
//            {
//                V1 = ((channel->vibrato_depth & 0x7f00) >> 0x8) * pitch_base;
//                if( (channel->vibrato_depth & 0x8000) == 0 ) V1 *= 0xf / 0x100;
//                channel->vibrato_base = h(V1 >> 0x7);
//                channel->vibrato_wave = w[0x8004a5cc + channel->vibrato_type * 0x4];
//                channel->vibrato_delay_cur = channel->vibrato_delay;
//                channel->vibrato_rate_cur = 1;
//            }
//
//            if( channel->update_flags & AKAO_UPDATE_TREMOLO )
//            {
//                channel->tremolo_wave = w[0x8004a5cc + channel->tremolo_type * 0x4];
//                channel->tremolo_delay_cur = channel->tremolo_delay;
//                channel->tremolo_rate_cur = 1;
//            }
//
//            if( channel->update_flags & AKAO_UPDATE_PAN_LFO )
//            {
//                channel->pan_lfo_rate_cur = 1;
//                channel->pan_lfo_wave = w[0x8004a5cc + channel->pan_lfo_type * 0x4];
//            }
//
//            channel->vibrato_pitch = 0;
//            channel->tremolo_vol = 0;
//            channel->pitch_slide = 0;
//        }
//
//        channel->sfx_mask &= ~0x0002;
//        channel->sfx_mask |= (channel->sfx_mask & AKAO_SFX_LEGATO) << 1;
//
//        if( channel->pitch_slide_dst != 0 )
//        {
//            [channel + 0xd0] = h(hu[channel + 0xd0] + channel->pitch_slide_dst);
//
//            S2 = bu[channel + 0xd0] + (channel->transpose & 0xff);
//
//            if( channel->type == AKAO_MUSIC )
//            {
//                A0 = g_akao_instrument[channel->instr_id].pitch[S2 % 0xc];
//
//                s16 V0 = channel->fine_tuning;
//                if( V0 != 0 )
//                {
//                    if( V0 > 0 )
//                    {
//                        V0 = (A0 * V0) >> 0x7;
//                    }
//                    else
//                    {
//                        V0 = (A0 * V0) >> 0x8;
//                    }
//                    A0 = (A0 + V0) & 0xffff;
//                }
//
//                A0 <<= 0x10;
//            }
//            else
//            {
//                A0 = (g_akao_instrument[channel->instr_id].pitch[S2 % 0xc]) << 0x10;
//            }
//
//            S2 = (S2 & 0xff) / 0xc;
//            V1 = S2 & 0xff;
//
//            if( V1 >= 0x7 )
//            {
//                A0 <<= V1 - 0x6;
//            }
//            else if( V1 < 0x6 )
//            {
//                A0 >>= 0x6 - V1;
//            }
//
//            channel->pitch_slide_steps_cur = channel->pitch_slide_steps;
//            channel->pitch_slide_dst = 0;
//            channel->pitch_slide_step = (A0 + channel->pitch_slide - (channel->pitch_base << 0x10)) / channel->pitch_slide_steps;
        }

//        [channel + 0xd4] = h(channel->transpose);
//        [channel + 0x6a] = h(hu[channel + 0xd0]);
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



void AkaoMusicUpdatePitchAndVolume( AkaoChannel* channel, u32 channel_mask, u32 channel_id )
{
    s32 volume_level = ((channel->volume >> 0x10) * channel->vol_master) >> 0x7;

//    if( (channel->update_flags & AKAO_UPDATE_VIBRATO) && (channel->vibrato_delay_cur == 0) )
//    {
//        channel->vibrato_rate_cur -= 1;
//
//        if( channel->vibrato_rate_cur == 0 )
//        {
//            channel->vibrato_rate_cur = channel->vibrato_rate;
//
//            V1 = channel->vibrato_wave;
//            if( (h[V1 + 0x0] == 0) && (h[V1 + 0x2] == 0) )
//            {
//                channel->vibrato_wave = V1 + h[V1 + 0x4] * 0x2;
//            }
//
//            V1 = channel->vibrato_wave;
//            channel->vibrato_wave = V1 + 0x2;
//
//            A3 = (channel->vibrato_base * h[V1 + 0x0]) >> 0x10; // frequency lfo multiplier
//            if( A3 != channel->vibrato_pitch )
//            {
//                channel->vibrato_pitch = A3;
//                channel->attr.mask |= SPU_VOICE_PITCH;
//
//                if( A3 >= 0 ) channel->vibrato_pitch = A3 * 0x2;
//            }
//        }
//    }
//
//    if( (channel->update_flags & AKAO_UPDATE_TREMOLO) && (channel->tremolo_delay_cur == 0) )
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
//            A3 = ((((volume_level * (channel->tremolo_depth >> 0x8)) << 0x9) >> 0x10) * h[V1 + 0x0]) >> 0xf;
//            if( A3 != channel->tremolo_vol )
//            {
//                channel->tremolo_vol = A3;
//                channel->attr.mask |= AKAO_UPDATE_SPU_VOICE;
//            }
//        }
//    }
//
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
//        volume_level = ((volume_level + channel->tremolo_vol) * ((g_akao_vol_mul_music >> 0x10) & 0x7f)) >> 0x7;
//
//        if( hu[0x8009c5a0 + 0x0 * 0xc + 0x8] != 0 )
//        {
//            volume_level = (volume_level * h[0x8009c5a0 + channel_id * 0xc + 0x2]) >> 0x7;
//        }
//
//        u8 vol_pan = ((channel->vol_pan >> 0x8) + channel->pan_lfo_vol) & 0xff;
//
//        if( g_channels_1_config.stereo_mono == AKAO_STEREO )
//        {
//            channel->attr.vol_l = (volume_level * h[0x80049c44 + vol_pan * 0x2]) >> 0xf;
//            channel->attr.vol_r = (volume_level * h[0x80049e44 + vol_pan * 0x2]) >> 0xf;
//        }
//        else if( g_channels_1_config.stereo_mono == AKAO_STEREO_CHANNELS )
//        {
//            channel->attr.vol_l = (volume_level * h[0x80049c44 + vol_pan * 0x2]) >> 0xf;
//            channel->attr.vol_r = (volume_level * h[0x80049e44 + vol_pan * 0x2]) >> 0xf;
//
//            if( channel_mask & 0x00aaaaaa )
//            {
//                channel->attr.vol_r = ~channel->attr.vol_r;
//            }
//            else
//            {
//                channel->attr.vol_l = ~channel->attr.vol_l;
//            }
//        }
//        else // mono
//        {
            channel->attr.vol_l = (volume_level * akao_left_volume_table[0x40]) >> 0xf;
            channel->attr.vol_r = (volume_level * akao_left_volume_table[0x40]) >> 0xf;
//        }
    }
//
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
        u32 pitch_base = channel->pitch_base /*+ channel->vibrato_pitch + (channel->pitch_slide >> 0x10)*/;
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
