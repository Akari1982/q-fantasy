#pragma once

#include "psyq/typedef.h"
#include "psyq/libgpu.h"



#define FADE_TYPE_NONE              0x0
#define FADE_TYPE_DIS_GRAD_SUB      0x1
#define FADE_TYPE_INC_GRAD_SUB      0x2
#define FADE_TYPE_BG_SLOW_SUB       0x3
#define FADE_TYPE_DISABLE_RENDER    0x4
#define FADE_TYPE_DIS_GRAD_ADD      0x5
#define FADE_TYPE_INC_GRAD_ADD      0x6
#define FADE_TYPE_DIS_DIR_SUB       0x7
#define FADE_TYPE_INC_DIR_SUB       0x8
#define FADE_TYPE_DIS_DIR_ADD       0x9
#define FADE_TYPE_INC_DIR_ADD       0xa
#define FADE_TYPE_ADD_INTERPOLATE   0xb
#define FADE_TYPE_SUB_INTERPOLATE   0xc
#define FADE_TYPE_BG_FAST_SUB       0xd



void FadeInitPoly();
void FadeCopyScreen();
void FadeUpdate();



extern OTag g_fade_ot[0x2];
