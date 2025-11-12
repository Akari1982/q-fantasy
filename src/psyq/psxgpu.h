#pragma once

#include "typedef.h"
#include "system/application.h"



#define VRAM_W 1028
#define VRAM_H 512


struct OTag;



void GPUInit();
void GPUExecute();



extern u32 g_rendering_disp_enable;
extern u32 g_rendering_disp_x;
extern u32 g_rendering_disp_y;
extern u32 g_rendering_disp_w;
extern u32 g_rendering_disp_h;

extern ofTexture g_vram_texture;
extern ofShader g_display_shader;
extern ofShader g_render_shader;
extern GLuint g_fbo_id;

extern std::vector<std::unique_ptr<OTag>> g_draw;
extern std::vector<OTag*> g_gpu_queue;
