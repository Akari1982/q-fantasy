#pragma once

#include "psyq/typedef.h"
#include "psyq/libgpu.h"

#include <array>



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



void FieldMain();
void FieldMainLoop();

void FieldUpdateDrawEnv();

void FieldLoadMimDatFiles();

void FieldRainInit( sFieldRainPrim* prim );
void FieldRainUpdate();
void FieldRainAddToRender( sTag* ot, MATRIX* m );



extern u32 g_field_dat_base_addr;
