#pragma once

#include "typedef.h"
#include "system/application.h"



#define VRAM_W 1028
#define VRAM_H 512



void GPUInit();
void GPUUpdateScreenFromVram();



extern u32 g_rendering_disp_enable;
extern u32 g_rendering_disp_x;
extern u32 g_rendering_disp_y;
extern u32 g_rendering_disp_w;
extern u32 g_rendering_disp_h;

extern ofTexture g_vram_texture;
extern ofShader g_display_shader;
