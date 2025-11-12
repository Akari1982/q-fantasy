#pragma once

#include "psyq/typedef.h"



#define BG_RENDER_NONE 0x0
#define BG_RENDER_FADE 0x1

#define GAME_STATE_NONE 0x0
#define GAME_STATE_FIELD 0x1
#define GAME_STATE_BATTLE 0x2
#define GAME_STATE_WORLDMAP 0x3



void GameMain();



extern u16 g_bg_render;
extern u16 g_gamestate;
extern u16 g_gamestate_prev;
