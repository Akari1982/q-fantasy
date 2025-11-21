#pragma once

#include "background.h"
#include "fade.h"
#include "psyq/typedef.h"
#include "psyq/libgpu.h"

#include <array>
#include <string>



#define FIELD_CMD_NONE 0x0
#define FIELD_CMD_MAP 0x1
#define FIELD_CMD_RESET 0xa



struct FieldControl
{
    // command
    u8 cmd = 0;
    u16 arg = 0;

    u8 disable_render;

    // fade
    u16 fade_type = FADE_TYPE_NONE;
    s16 fade_steps;
    u16 fade_step;
    u8 fade_r;
    u8 fade_g;
    u8 fade_b;
    u8 nfade_r_from;
    u8 nfade_g_from;
    u8 nfade_b_from;
    u8 nfade_r_to;
    u8 nfade_g_to;
    u8 nfade_b_to;

    // controller
    u32 btn_pressed;
    u32 btn_prev;
    u32 btn_new;
    u32 btn_released;
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
    OTag ot_scene_drawenv;
    DR_ENV scene_drawenv;
    OTag ot_fade_drawenv;
    DR_ENV fade_drawenv;

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
void FieldFadeBgDraw();
void FieldInitEnv();



extern std::vector<std::string> g_field_files;
extern FieldControl g_field_control;
extern std::vector<u8> g_field_dat;
extern u32 g_field_dat_base_addr;
extern u16 g_field_map_id;
extern std::vector<u8> g_field_random;
extern u16 g_field_rb;
extern DISPENV g_main_dispenv[0x2];
