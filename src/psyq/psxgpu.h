#pragma once

#include "typedef.h"
#include "system/application.h"



#define VRAM_W 2048
#define VRAM_H 512



void GPUInit();
void GPUUpdateScreenFromVram();
void GPUUpdateRenderFromVram();
void GPUUpdateVramFromRender();
void GPUUpdateTextureFromVram();



extern u32 g_rendering_disp_x;
extern u32 g_rendering_disp_y;

extern std::array<u8, 2048 * 512> g_vram;
extern ofTexture g_screen;
