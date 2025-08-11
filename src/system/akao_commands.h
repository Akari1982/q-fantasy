typedef void (*AkaoCommand) ( AkaoCommandData* data );

// 0x80049548
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



void system_akao_command_null( AkaoCommandData* data )
{
}



void system_akao_command_10( AkaoCommandData* data )
{
    offset = data->param[1];
    size = data->param[2];
    music_id = data->param[3];

    system_akao_copy_music( offset, size );

    system_akao_music_channels_init();
}



void system_akao_command_14( AkaoCommandData* data )
{
}



// field_event_opcode_f4_musvm
void system_akao_command_15( AkaoCommandData* data )
{
}



// called when field starts from battle
// continue music?
void system_akao_command_18( AkaoCommandData* data )
{
}



void system_akao_command_19( AkaoCommandData* data )
{
}



// field_event_opcode_f1_se
void system_akao_command_20( AkaoCommandData* data )
{
}



void system_akao_command_21( AkaoCommandData* data )
{
}



void system_akao_command_22( AkaoCommandData* data )
{
}



void system_akao_command_23( AkaoCommandData* data )
{
}



void system_akao_command_29( AkaoCommandData* data )
{
}



void system_akao_command_2a( AkaoCommandData* data )
{
}



void system_akao_command_2b( AkaoCommandData* data )
{
}



// sound from menu
void system_akao_command_30( AkaoCommandData* data )
{
}



void system_akao_command_34( AkaoCommandData* data )
{
}



void system_akao_command_80( AkaoCommandData* data )
{
}



void system_akao_command_81( AkaoCommandData* data )
{
}



void system_akao_command_82( AkaoCommandData* data )
{
}



void system_akao_command_90( AkaoCommandData* data )
{
}



void system_akao_command_92( AkaoCommandData* data )
{
}



void system_akao_command_9a( AkaoCommandData* data )
{
}



void system_akao_command_9b( AkaoCommandData* data )
{
}



void system_akao_command_9c( AkaoCommandData* data )
{
}



void system_akao_command_9d( AkaoCommandData* data )
{
}



void system_akao_command_a0( AkaoCommandData* data )
{
}



void system_akao_command_a1( AkaoCommandData* data )
{
}



void system_akao_command_a2( AkaoCommandData* data )
{
}



void system_akao_command_a3( AkaoCommandData* data )
{
}



void system_akao_command_a4( AkaoCommandData* data )
{
}


void system_akao_command_a5( AkaoCommandData* data )
{
}



void system_akao_command_a6( AkaoCommandData* data )
{
}



void system_akao_command_a7( AkaoCommandData* data )
{
}



void system_akao_command_a8( AkaoCommandData* data )
{
}



void system_akao_command_a9( AkaoCommandData* data )
{
}



void system_akao_command_aa( AkaoCommandData* data )
{
}



void system_akao_command_ab( AkaoCommandData* data )
{
}



void system_akao_command_ac( AkaoCommandData* data )
{
}



void system_akao_command_ad( AkaoCommandData* data )
{
}



void system_akao_command_ae( AkaoCommandData* data )
{
}



void system_akao_command_af( AkaoCommandData* data )
{
}



void system_akao_command_b0( AkaoCommandData* data )
{
}



void system_akao_command_b1( AkaoCommandData* data )
{
}



void system_akao_command_b2( AkaoCommandData* data )
{
}



void system_akao_command_b3( AkaoCommandData* data )
{
}



void system_akao_command_b4( AkaoCommandData* data )
{
}



void system_akao_command_b5( AkaoCommandData* data )
{
}



void system_akao_command_b6( AkaoCommandData* data )
{
}



void system_akao_command_b7( AkaoCommandData* data )
{
}



void system_akao_command_b8( AkaoCommandData* data )
{
}



void system_akao_command_b9( AkaoCommandData* data )
{
}



void system_akao_command_ba( AkaoCommandData* data )
{
}



void system_akao_command_bb( AkaoCommandData* data )
{
}



void system_akao_command_bc( AkaoCommandData* data )
{
}



void system_akao_command_bd( AkaoCommandData* data )
{
}



void system_akao_command_c0( AkaoCommandData* data )
{
}



void system_akao_command_c1( AkaoCommandData* data )
{
}



void system_akao_command_c2( AkaoCommandData* data )
{
}



void system_akao_command_c8( AkaoCommandData* data )
{
}



void system_akao_command_c9( AkaoCommandData* data )
{
}



void system_akao_command_ca( AkaoCommandData* data )
{
}



void system_akao_command_d0( AkaoCommandData* data )
{
}



void system_akao_command_d1( AkaoCommandData* data )
{
}



void system_akao_command_d2( AkaoCommandData* data )
{
}



void system_akao_command_d4( AkaoCommandData* data )
{
}



void system_akao_command_d5( AkaoCommandData* data )
{
}



void system_akao_command_d6( AkaoCommandData* data )
{
}



void system_akao_command_e0( AkaoCommandData* data )
{
}



void system_akao_command_e4( AkaoCommandData* data )
{
}



void system_akao_command_f0( AkaoCommandData* data )
{
}



void system_akao_command_f1( AkaoCommandData* data )
{
}



void system_akao_command_f2( AkaoCommandData* data )
{
}



void system_akao_command_f3( AkaoCommandData* data )
{
}



void system_akao_command_f4( AkaoCommandData* data )
{
}



void system_akao_command_f5( AkaoCommandData* data )
{
}



void system_akao_command_f8( AkaoCommandData* data )
{
}



void system_akao_command_f9( AkaoCommandData* data )
{
}



void system_akao_command_fa( AkaoCommandData* data )
{
}
