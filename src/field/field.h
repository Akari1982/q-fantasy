#pragma once

#include "background.h"
#include "psyq/typedef.h"
#include "psyq/libgpu.h"

#include <array>



struct FieldRenderData
{
    // rendering tags
    OTag ot_scene[0x1000]; // main scene

    // background
    SPRT_16 bg_1[0x9c4]; // poly 1st and 2nd layer
    SPRT bg_2[0x200]; // poly 3rd and 4th layer
    BgAnim bg_anim[0xbc4];
    DR_MODE bg_dm[0x6a4];

    // rain
    std::array< LINE_F2, 0x40 > rain;
    DR_MODE rain_dm;
};



void FieldMain();



extern u32 g_field_dat_base_addr;
extern std::vector<u8> g_field_random;
