#pragma once

#include "psyq/typedef.h"



#define GAME_STATE_NONE         0x0
#define GAME_STATE_FIELD        0x1
#define GAME_STATE_BATTLE       0x2
#define GAME_STATE_WORLDMAP     0x3
#define GAME_STATE_MENU         0x5
#define GAME_STATE_MENU_FUNC    0xd



void GameMain();



extern u16 g_gamestate;
extern u16 g_gamestate_prev;
