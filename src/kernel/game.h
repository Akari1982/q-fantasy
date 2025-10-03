#pragma once

#include "../psyq/libgpu.h"



extern DISPENV g_global_dispenv;
extern DRAWENV g_global_drawenv;



void GameMain();
void GameInitBase();
void GameInitAkaoEngine();
void GameInitDispEnvDrawEnv();
