#pragma once

#include "background.h"
#include "psyq/typedef.h"
#include "psyq/libgpu.h"

#include <array>



#define FIELD_CMD_NONE 0x0
#define FIELD_CMD_LOAD 0x1



struct FieldControl
{
    u8 cmd = 0;
    u16 map_id = 0;
};

struct FieldCamera
{
    MATRIX m;
    u16 x;
    u16 y;
    s16 h;
};

struct FieldRenderData
{
    // scene
    OTag ot_scene[0x1000]; // main scene
    OTag ot_scene_drenv;
    DR_ENV scene_drenv;

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
void FieldInitEnv();



extern FieldControl g_field_control;
extern std::vector<u8> g_field_dat;
extern u32 g_field_dat_base_addr;
extern u16 g_field_map_id;
extern std::vector<u8> g_field_random;
extern u16 g_field_rb;
