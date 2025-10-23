#pragma once

#include "typedef.h"
#include "system/application.h"



void GPUUpdateVram();



extern u32 g_rendering_draw_x;
extern u32 g_rendering_draw_y;
extern u32 g_rendering_disp_x;
extern u32 g_rendering_disp_y;

extern std::array<u8, 2048 * 512> g_vram;
extern ofTexture g_screen;
extern ofFbo g_render;
