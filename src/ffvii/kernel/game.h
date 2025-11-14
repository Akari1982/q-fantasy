#pragma once

#include "psyq/typedef.h"



#define GAME_STATE_NONE 0x0
#define GAME_STATE_FIELD 0x1
#define GAME_STATE_BATTLE 0x2
#define GAME_STATE_WORLDMAP 0x3



void GameMain();



extern u16 g_gamestate;
extern u16 g_gamestate_prev;
