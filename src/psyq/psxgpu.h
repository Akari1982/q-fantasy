#pragma once

#include "typedef.h"
#include "system/application.h"



#define VRAM_W 2048
#define VRAM_H 512



void GPUUpdateScreenFromVram();
void GPUUpdateRenderFromVram();
void GPUUpdateVramFromRender();
void GPUUpdateTextureFromVram();



extern u32 g_rendering_draw_x;
extern u32 g_rendering_draw_y;
extern u32 g_rendering_disp_x;
extern u32 g_rendering_disp_y;
extern u16 g_rendering_tpage;
extern u32 g_rendering_dtd;

extern std::array<u8, 2048 * 512> g_vram;
extern ofTexture g_screen;
extern ofFbo g_render;
