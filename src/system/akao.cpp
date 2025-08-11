#include "akao.h"
#include "../psyq/libspu.h"

#include "akao_opcodes.h"
#include "akao_commands.h"

AkaoInstrument g_akao_instrument[0x80];
u8 g_effect_all[0xc800];

ChannelData g_channels_1[0x18];
ChannelConfig g_channels_1_config;
u8* g_akao_music;

const u8 g_akao_default_sound[0x20] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
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

    memcpy( g_akao_instrument, instr_dat, 0x2000 );

    // FILE* check = fopen( "check.dat", "ab" );
    // fwrite( g_akao_instrument, 1, 0x2000, check);
    // fclose( check );
}



void system_akao_init_data()
{
    g_channels_1_config.active_mask = 0;
    g_channels_1_config.on_mask = 0;
    g_channels_1_config.off_mask = 0;

    for( int i = 0; i < 0x18; ++i )
    {
        g_channels_1[i].attr.voice_id = i;
        PsyqSpuSetVoiceVolumeAttr( i, 0, 0, 0, 0 );
    }
}



void system_akao_main()
{
    system_akao_main_update();
}



void system_akao_main_update()
{
    system_akao_update_keys_on();

    if( g_channels_1_config.active_mask != 0 )
    {
        g_channels_1_config.tempo_update += g_channels_1_config.tempo >> 0x10;

        if( (g_channels_1_config.tempo_update & 0xffff0000) || ((g_akao_control_flags & 0x00000004) != 0) )
        {
            g_channels_1_config.tempo_update &= 0x0000ffff;

            channel_mask = 0x1;
            u32 channels_mask = g_channels_1_config.active_mask;
            ChannelData* channel = &g_channels_1;
            while( channels_mask != 0 )
            {
                if( channels_mask & channel_mask )
                {
                    channel->length_1 -= 0x1;
                    channel->length_2 -= 0x1;

                    if( channel->length_1 == 0 )
                    {
                        system_akao_execute_sequence( channel, g_channels_1_config, channel_mask );
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



void system_akao_execute_sequence( ChannelData* channel, AkaoConfig* config, u32 mask )
{
    do
    {
        u32 akao = channel->seq;
        channel->seq = akao + 0x1;

        opcode = bu[akao];

        if( opcode < 0xa0 ) break;

        akao_opcodes[opcode - 0xa0]( channel, config, mask );

    } while( opcode != 0xa0 )

    if( opcode != 0xa0 )
    {
        if( channel->length_1 == 0 )
        {
            channel->length_1 = g_akao_length_table[opcode % 0xb] & 0x00ff;
            channel->length_2 = (g_akao_length_table[opcode % 0xb] & 0xff00) >> 0x8;
        }

        if( opcode < 0x84 )
        {
            config->on_mask |= mask;

            S2 = opcode / 0xb;
            V1 = S2 / 0xc;

            u32 pitch_base = g_akao_instrument[channel->instr_id].pitch[S2 % 0xc];
            if( V1 >= 0x7 )
            {
                pitch_base <<= V1 - 0x6;
            }
            else if( V1 < 0x6 )
            {
                pitch_base >>= 0x6 - V1;
            }
            channel->pitch_base = pitch_base;

            channel->attr.mask |= AKAO_UPDATE_SPU_VOICE | SPU_VOICE_PITCH;
        }
    }
}



void system_akao_update_keys_on()
{
    u32 updated_mask = 0;

    if( g_channels_1_config.active_mask != 0 )
    {
        updated_mask |= g_channels_1_config.on_mask;

        u32 channel_id = 0;

        u32 channel_mask = 1;
        channels_mask = g_channels_1_config.active_mask;
        ChannelData* channel = &g_channels_1;
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



void system_akao_music_update_pitch_and_volume( ChannelData* channel, channel_mask, channel_id )
{
    volume_level = channel->volume >> 0x10;

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
        PsyqSpuSetVoiceStartAddr( voice_id. attr.addr );
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
    g_akao_music = malloc( size );
    memcpy( g_akao_music, src, size );
}



void system_akao_instr_init( ChannelData* channel, u16 instr_id )
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
    u32 channels_mask = w[0x80083580] & 0x00ffffff;
    u32 akao = 0x80083580 + 0x4;

    g_channels_1_config.active_mask = channels_mask;
    g_channels_1_config.on_mask = 0;
    g_channels_1_config.off_mask |= 0x00ffffff;
    g_channels_1_config.tempo = 0xffff0000;
    g_channels_1_config.tempo_update = 0x1;

    ChannelData* channel = &g_channels_1;
    channel_mask = 0x1;
    while( channels_mask != 0 )
    {
        if( channels_mask & channel_mask )
        {
            channel->seq = akao + 0x2 + hu[akao];
            channel->length_1 = 0x3;
            channel->length_2 = 0x1;

            system_akao_instr_init( channel, 0x14 );

            channel->volume = 0x3fff0000;

            akao += 0x2;
            channels_mask ^= channel_mask;
        }

        channel += 1;
        channel_mask <<= 1;
    }
}
