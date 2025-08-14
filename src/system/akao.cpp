#include "akao.h"
#include "../psyq/libspu.h"

#include "akao_opcodes.h"
#include "akao_commands.h"

u16 g_akao_reverb_pan;

AkaoInstrument g_akao_instrument[0x80];
u8 g_effect_all[0xc800];

AkaoChannel g_channels_1[0x18];
AkaoConfig g_channels_1_config;
u8* g_akao_music;

SpuReverbAttr g_spu_reverb_attr;

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
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_10,   system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_14,   system_akao_command_15,   system_akao_command_null, system_akao_command_null,
    system_akao_command_18,   system_akao_command_19,   system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_20,   system_akao_command_21,   system_akao_command_22,   system_akao_command_23,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_20,   system_akao_command_29,   system_akao_command_2a,   system_akao_command_2b,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_30,   system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_34,   system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_80,   system_akao_command_81,   system_akao_command_82,   system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_90,   system_akao_command_null, system_akao_command_92,   system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_9a,   system_akao_command_9b,
    system_akao_command_9c,   system_akao_command_9d,   system_akao_command_null, system_akao_command_null,
    system_akao_command_a0,   system_akao_command_a1,   system_akao_command_a2,   system_akao_command_a3,
    system_akao_command_a4,   system_akao_command_a5,   system_akao_command_a6,   system_akao_command_a7,
    system_akao_command_a8,   system_akao_command_a9,   system_akao_command_aa,   system_akao_command_ab,
    system_akao_command_ac,   system_akao_command_ad,   system_akao_command_ae,   system_akao_command_af,
    system_akao_command_b0,   system_akao_command_b1,   system_akao_command_b2,   system_akao_command_b3,
    system_akao_command_b4,   system_akao_command_b5,   system_akao_command_b6,   system_akao_command_b7,
    system_akao_command_b8,   system_akao_command_b9,   system_akao_command_ba,   system_akao_command_bb,
    system_akao_command_bc,   system_akao_command_bd,   system_akao_command_null, system_akao_command_null,
    system_akao_command_c0,   system_akao_command_c1,   system_akao_command_c2,   system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_c8,   system_akao_command_c9,   system_akao_command_ca,   system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_d0,   system_akao_command_d1,   system_akao_command_d2,   system_akao_command_null,
    system_akao_command_d4,   system_akao_command_d5,   system_akao_command_d6,   system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_e0,   system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_e4,   system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null,
    system_akao_command_f0,   system_akao_command_f1,   system_akao_command_f2,   system_akao_command_f3,
    system_akao_command_f4,   system_akao_command_f5,   system_akao_command_null, system_akao_command_null,
    system_akao_command_f8,   system_akao_command_f9,   system_akao_command_fa,   system_akao_command_null,
    system_akao_command_null, system_akao_command_null, system_akao_command_null, system_akao_command_null
};

AkaoOpcode akao_opcodes[] =
{
    system_akao_opcode_a0_finish_channel,         system_akao_opcode_a1_load_instrument,         system_akao_opcode_a2_next_note_length,            system_akao_opcode_a3_master_volume,
    system_akao_opcode_a4_pitch_bend_slide,       system_akao_opcode_a5_set_octave,              system_akao_opcode_a6_increase_octave,             system_akao_opcode_a7_discrease_octave,
    system_akao_opcode_a8_set_volume,             system_akao_opcode_a9_set_volume_slide,        system_akao_opcode_aa_set_pan,                     system_akao_opcode_ab_set_pan_slide,
    system_akao_opcode_ac_noise_clock_freq,       system_akao_opcode_ad_set_ar,                  system_akao_opcode_ae_set_dr,                      system_akao_opcode_af_set_sl,
    system_akao_opcode_b0_set_voice_dr_sl,        system_akao_opcode_b1_set_sr,                  system_akao_opcode_b2_set_rr,                      system_akao_opcode_b3_reset_adsr,
    system_akao_opcode_b4_vibrato,                system_akao_opcode_b5_vibrato_depth,           system_akao_opcode_b6_vibrato_off,                 system_akao_opcode_b7_attack_mode,
    system_akao_opcode_b8_tremolo,                system_akao_opcode_b9_tremolo_depth,           system_akao_opcode_ba_tremolo_off,                 system_akao_opcode_bb_sustain_mode,
    system_akao_opcode_bc_pan_lfo,                system_akao_opcode_bd_pan_lfo_depth,           system_akao_opcode_be_pan_lfo_off,                 system_akao_opcode_bf_release_mode,
    system_akao_opcode_c0_transpose_absolute,     system_akao_opcode_c1_transpose_relative,      system_akao_opcode_c2_reverb_on,                   system_akao_opcode_c3_reverb_off,
    system_akao_opcode_c4_noise_on,               system_akao_opcode_c5_noise_off,               system_akao_opcode_c6_frequency_modulation_on,     system_akao_opcode_c7_frequency_modulation_off,
    system_akao_opcode_c8_loop_point,             system_akao_opcode_c9_loop_return_times,       system_akao_opcode_ca_loop_return,                 system_akao_opcode_cb_sfx_reset,
    system_akao_opcode_cc_legato_on,              system_akao_opcode_cd_legato_off,              system_akao_opcode_ce_noise_switch,                system_akao_opcode_cf_noise_switch,
    system_akao_opcode_d0_full_length_on,         system_akao_opcode_d1_full_length_off,         system_akao_opcode_d2_frequency_modulation_switch, system_akao_opcode_d3_frequency_modulation_switch,
    system_akao_opcode_d4_side_chain_playback_on, system_akao_opcode_d5_side_chain_playback_off, system_akao_opcode_d6_side_chain_pitch_vol_on,     system_akao_opcode_d7_side_chain_pitch_vol_off,
    system_akao_opcode_d8_fine_tuning_absolute,   system_akao_opcode_d9_fine_tuning_relative,    system_akao_opcode_da_portamento_on,               system_akao_opcode_db_portamento_off,
    system_akao_opcode_dc_fix_note_length,        system_akao_opcode_dd_vibrato_depth_slide,     system_akao_opcode_de_tremolo_depth_slide,         system_akao_opcode_df_pan_lfo_depth_slide,
    system_akao_opcode_null,                      system_akao_opcode_null,                       system_akao_opcode_null,                           system_akao_opcode_null,
    system_akao_opcode_null,                      system_akao_opcode_null,                       system_akao_opcode_null,                           system_akao_opcode_null,
    system_akao_opcode_e8_tempo,                  system_akao_opcode_e9_tempo_slide,             system_akao_opcode_ea_reverb_depth,                system_akao_opcode_eb_reverb_depth_slide,
    system_akao_opcode_ec_drum_mode_on,           system_akao_opcode_ed_drum_mode_off,           system_akao_opcode_ee_jump,                        system_akao_opcode_ef_jump_conditional,
    system_akao_opcode_f0_loop_jump_times,        system_akao_opcode_f1_loop_break_times,        system_akao_opcode_f2_load_instrument,             system_akao_opcode_f3,
    system_akao_opcode_f4_overlay_voice_on,       system_akao_opcode_f5_overlay_voice_off,       system_akao_opcode_f6_overlay_volume_balance,      system_akao_opcode_f7_overlay_volume_balance_slide,
    system_akao_opcode_f8_alt_voice_on,           system_akao_opcode_f9_alt_voice_off,           system_akao_opcode_null,                           system_akao_opcode_null,
    system_akao_opcode_null,                      system_akao_opcode_fd_time_signature,          system_akao_opcode_fe_measure_number,              system_akao_opcode_null
};



void system_akao_init( u8* instr_all, u8* instr_dat )
{
    system_akao_load_instr_files( instr_all, instr_dat );

    PsyqSpuSetTransferStartAddr( 0x76fe0 );
    PsyqSpuWrite( g_akao_default_sound, 0x20 );

    system_akao_init_data();

/*
    do
    {
        V0 = system_bios_open_event( RCntCNT2, EvSpINT, EvMdINTR, 0x80030234 ); // system_akao_main() execute function/stay busy
        [GP + 0xbc] = w(V0);
    } while( V0 == -1 )

    while( system_bios_enable_event( w[GP + 0xbc] ) == 0 ) {}

    while( system_psyq_set_r_cnt( RCntCNT2, 0x43d1, EvMdINTR ) == 0 ) {}

    while( system_psyq_start_r_cnt( RCntCNT2 ) == 0 ) {}
*/
}



void system_akao_load_effect_file( u8* effect_all )
{
    memcpy( g_effect_all, effect_all, 0xc800 );
}



void system_akao_load_instr_files( u8* instr_all, u8* instr_dat )
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



void system_akao_init_data()
{
    g_akao_reverb_pan = 0x40;

    g_channels_1_config.active_mask = 0;
    g_channels_1_config.on_mask = 0;
    g_channels_1_config.off_mask = 0;
    g_channels_1_config.reverb_depth = 0;

    for( int i = 0; i < 0x18; ++i )
    {
        g_channels_1[i].attr.voice_id = i;
        PsyqSpuSetVoiceVolumeAttr( i, 0, 0, 0, 0 );
    }
}



void system_akao_main()
{
    static int timer = 0;

    //if( timer == 0x10 )
    {
        timer = 0;
        system_akao_main_update();
    }
    ++timer;
}



void system_akao_main_update()
{
    system_akao_update_keys_on();

    if( g_channels_1_config.active_mask != 0 )
    {
        g_channels_1_config.tempo_update += g_channels_1_config.tempo >> 0x10;

        //ofLog( OF_LOG_NOTICE, "tempo_update" +  ofToHex( g_channels_1_config.tempo_update ) );

        if( g_channels_1_config.tempo_update & 0xffff0000 )
        {
            g_channels_1_config.tempo_update &= 0x0000ffff;

            u32 channel_mask = 0x1;
            u32 channels_mask = g_channels_1_config.active_mask;
            AkaoChannel* channel = g_channels_1;
            while( channels_mask != 0 )
            {
                if( channels_mask & channel_mask )
                {
                    channel->length_1 -= 0x1;
                    channel->length_2 -= 0x1;

                    //ofLog( OF_LOG_NOTICE, "channel" +  ofToHex( channel->attr.voice_id ) + ": length_1 " +  ofToHex( (int)channel->length_1 ) );

                    if( channel->length_1 == 0 )
                    {
                        system_akao_execute_sequence( channel, &g_channels_1_config, channel_mask );
                    }
                    else if( channel->length_2 == 0 )
                    {
                        channel->length_2 |= 0x1;
                        g_channels_1_config.off_mask |= channel_mask;
                    }

                    channels_mask ^= channel_mask;
                }

                channel += 1;
                channel_mask <<= 1;
            }
        }
    }

    system_akao_update_keys_off();
}



void system_akao_execute_sequence( AkaoChannel* channel, AkaoConfig* config, u32 mask )
{
    //ofLog( OF_LOG_NOTICE, "system_akao_execute_sequence" );

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
        //ofLog( OF_LOG_NOTICE, "channel" +  ofToHex( channel->attr.voice_id ) + ": note " +  ofToHex( (int)opcode ) );

        if( channel->length_1 == 0 )
        {
            //ofLog( OF_LOG_NOTICE, "opcode:0x" +  ofToHex( opcode ) + " note length:0x" +  ofToHex( opcode % 0xb ) );

            channel->length_1 = g_akao_length_table[opcode % 0xb] & 0x00ff;
            channel->length_2 = (g_akao_length_table[opcode % 0xb] & 0xff00) >> 0x8;
        }

        if( opcode < 0x84 )
        {
            config->on_mask |= mask;

            u8 S2 = opcode / 0xb;
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
            channel->pitch_base = pitch_base;

            channel->attr.mask |= AKAO_UPDATE_SPU_VOICE | SPU_VOICE_PITCH;
        }
    }
}



void system_akao_update_keys_on()
{
    u32 updated_mask = 0;

    if( g_channels_1_config.update_flags & AKAO_UPDATE_REVERB )
    {
        s16 reverb_depth = g_channels_1_config.reverb_depth >> 0x10;

        g_spu_reverb_attr.mask = SPU_REV_DEPTHL | SPU_REV_DEPTHR;

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

    if( g_channels_1_config.active_mask != 0 )
    {
        updated_mask |= g_channels_1_config.on_mask;

        u32 channel_id = 0;

        u32 channel_mask = 1;
        u32 channels_mask = g_channels_1_config.active_mask;
        AkaoChannel* channel = g_channels_1;
        while( channels_mask != 0 )
        {
            if( channels_mask & channel_mask )
            {
                system_akao_music_update_pitch_and_volume( channel, channel_mask, channel_id );

                if( channel->attr.mask != 0 )
                {
                    system_akao_update_channel_params_to_spu( channel->attr.voice_id, channel->attr );
                }

                channels_mask ^= channel_mask;
            }

            channel_mask <<= 1;
            channel += 1;
            channel_id += 1;
        }

        g_channels_1_config.on_mask = 0;
    }

    if( updated_mask != 0 )
    {
        PsyqSpuSetKey( SPU_ON, updated_mask );
    }
}



void system_akao_update_keys_off()
{
    u32 updated_mask = 0;

    if( g_channels_1_config.active_mask != 0 )
    {
        updated_mask = g_channels_1_config.off_mask;
        g_channels_1_config.off_mask = 0;
    }

    if( updated_mask != 0 )
    {
        PsyqSpuSetKey( SPU_OFF, updated_mask );
    }
}



void system_akao_music_update_pitch_and_volume( AkaoChannel* channel, u32 channel_mask, u32 channel_id )
{
    s32 volume_level = ((channel->volume >> 0x10) * channel->vol_master) >> 0x7;

    if( channel->attr.mask & AKAO_UPDATE_SPU_VOICE )
    {
        channel->attr.vol_l = (volume_level * akao_left_volume_table[0x40]) >> 0xf;
        channel->attr.vol_r = (volume_level * akao_left_volume_table[0x40]) >> 0xf;
    }

    if( channel->attr.mask & SPU_VOICE_PITCH )
    {
        channel->attr.pitch = channel->pitch_base & 0x3fff;
    }
}



void system_akao_update_channel_params_to_spu( u32 voice_id, AkaoVoiceAttr& attr )
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



void system_akao_copy_music( u8* src, u32 size )
{
    if( g_akao_music != 0 )
    {
        free( g_akao_music );
    }
    g_akao_music = (u8*)malloc( size );
    memcpy( g_akao_music, src, size );
}



void system_akao_instr_init( AkaoChannel* channel, u16 instr_id )
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



void system_akao_music_channels_init()
{
    u32 channels_mask = READ_LE_U32( g_akao_music + 0x0 ) & 0x00ffffff & 1;
    u8* akao = g_akao_music + 0x4;

    g_channels_1_config.active_mask = channels_mask;
    g_channels_1_config.on_mask = 0;
    g_channels_1_config.off_mask |= 0x00ffffff;
    g_channels_1_config.tempo = 0xffff0000;
    g_channels_1_config.tempo_update = 0x1;
    g_channels_1_config.reverb_depth = 0;

    AkaoChannel* channel = g_channels_1;
    u32 channel_mask = 0x1;
    while( channels_mask != 0 )
    {
        if( channels_mask & channel_mask )
        {
            channel->seq = akao + 0x2 + READ_LE_U16( akao );
            channel->length_1 = 0x3;
            channel->length_2 = 0x1;

            system_akao_instr_init( channel, 0x14 );

            channel->volume = 0x3fff0000;
            channel->vol_master = 0x7f;

            akao += 0x2;
            channels_mask ^= channel_mask;
        }

        channel += 1;
        channel_mask <<= 1;
    }
}



void DumpSequenceData( u8* music )
{
    std::ofstream oF;
    oF.open( "dump_seq.txt", std::ios_base::binary | std::ios_base::out );
    if ( !oF.is_open() )
    {
        return;
    }

    u16 offset_to_akao = 0;

    oF << std::setfill( '0' );
    oF << std::hex;
    oF << "*** Frame ID: 0x" << std::setw( 4 ) << READ_LE_U16( music + 4 ) << std::endl;
    oF << std::dec;
    oF << "*** Frame Length: " << READ_LE_U16( music + 6 ) << " bytes" << std::endl;
    oF << std::hex;
    oF << "*** Reverb Mode: 0x" << std::setw( 4 ) << READ_LE_U16( music + 8 ) << std::endl;
    oF << std::endl;

    oF << std::hex;
    oF << "*** Channels mask: " <<std:: setw( 8 ) << READ_LE_U32( music + 16 ) << std::endl << std::endl;

    for ( int i = 0; i < 24; ++i )
    {
        if ( ( ( READ_LE_U32( music + 16 ) ) & ( 1 << i ) ) == 0 )
        {
            continue;
        }

        oF << std::dec;
        oF << "*** Channel " << std::setw( 2 ) << ( int )i << " sequence: " << std::endl;

        oF << std::hex;
        bool work = true;

        u32 j = 20 + offset_to_akao + 2 + READ_LE_U16( music + 20 + offset_to_akao );
        offset_to_akao += 2;

        while ( work )
        {
            u8 opcode = READ_LE_U8( music + j );
            switch ( opcode )
            {
                case 0xa0:
                    oF << std::setw( 2 ) << ( int )READ_LE_U8( music + j + 0 );
                    oF << std::endl;
                    work = false;
                    break;
                // no parameters
                case 0xa6: case 0xa7: case 0xb3: case 0xb6: case 0xba: case 0xbe: case 0xc2: case 0xc3:
                case 0xc4: case 0xc5: case 0xc6: case 0xc7: case 0xc8: case 0xca: case 0xcb: case 0xcc:
                case 0xcd: case 0xd0: case 0xd1: case 0xd4: case 0xd5: case 0xd6: case 0xd7: case 0xdb:
                case 0xe0: case 0xe1: case 0xe2: case 0xe3: case 0xe4: case 0xe5: case 0xe6: case 0xe7:
                case 0xed: case 0xf3: case 0xf5: case 0xf9: case 0xfa: case 0xfb: case 0xfc: case 0xff:
                    oF << std::setw( 2 ) << ( int )READ_LE_U8( music + j + 0 );
                    oF << std::endl;
                    ++j;
                    break;
                // one parameter
                case 0xa1: case 0xa2: case 0xa3: case 0xa5: case 0xa8: case 0xaa: case 0xac: case 0xad:
                case 0xae: case 0xaf: case 0xb1: case 0xb2: case 0xb5: case 0xb7: case 0xb9: case 0xbb:
                case 0xbd: case 0xbf: case 0xc0: case 0xc1: case 0xc9: case 0xce: case 0xcf: case 0xd2:
                case 0xd3: case 0xd8: case 0xd9: case 0xda: case 0xdc: case 0xf2: case 0xf6: case 0xf8:
                    oF << std::setw( 2 ) << ( int )READ_LE_U8( music + j + 0 ) << ' ';
                    oF << std::setw( 2 ) << ( int )READ_LE_U8( music + j + 1 );
                    oF << std::endl;
                    j += 2;
                    break;
                // two parameters
                case 0xa4: case 0xa9: case 0xab: case 0xb0: case 0xbc: case 0xdd: case 0xde: case 0xdf:
                case 0xe8: case 0xea: case 0xec: case 0xee: case 0xf4: case 0xf7: case 0xfd: case 0xfe:
                    oF << std::setw( 2 ) << ( int )READ_LE_U8( music + j + 0 ) << ' ';
                    oF << std::setw( 2 ) << ( int )READ_LE_U8( music + j + 1 ) << ' ';
                    oF << std::setw( 2 ) << ( int )READ_LE_U8( music + j + 2 );
                    oF << std::endl;
                    j += 3;
                    break;
                // three parameters
                case 0xb4: case 0xb8: case 0xe9: case 0xeb: case 0xef: case 0xf0: case 0xf1:
                    oF << std::setw( 2 ) << ( int )READ_LE_U8( music + j + 0 ) << ' ';
                    oF << std::setw( 2 ) << ( int )READ_LE_U8( music + j + 1 ) << ' ';
                    oF << std::setw( 2 ) << ( int )READ_LE_U8( music + j + 2 ) << ' ';
                    oF << std::setw( 2 ) << ( int )READ_LE_U8( music + j + 3 );
                    oF << std::endl;
                    j += 4;
                    break;
                default:
                    oF << std::setw( 2 ) << ( int )READ_LE_U8( music + j + 0 );
                    if ( READ_LE_U8( music + j + 1 ) < 0xa0)
                    {
                        oF << ' ';
                    }
                    else
                    {
                        oF << std::endl;
                    }
                    ++j;
            }
        }

        oF << std::endl;
    }

    oF.close();
}
