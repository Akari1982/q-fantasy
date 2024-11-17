#pragma once

#include <array>

#include "../psyq/typedef.h"
#include "../psyq/libgpu.h"



struct sFieldRain
{
    SVECTOR m0_p1;
    SVECTOR m8_p2;
    u16 m10_wait;
    s16 m12_seed;
    s16 m14;
    bool m16_render;
};

struct sFieldRainPrim
{
    std::array< LINE_F2, 0x40 > poly;
};



void field_main();
void field_main_loop();

void field_load_mim_dat_files();

void field_rain_init( sFieldRainPrim* prim );
void field_rain_update();
void field_rain_add_to_render( sTag* ot, MATRIX* m );
