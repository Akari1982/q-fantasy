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



void system_akao_init( FILE* instr_all, FILE* instr_dat );
void system_akao_load_effect_file( FILE* effect_all );
void system_akao_load_instr_files( FILE* instr_all, FILE* instr_dat );
