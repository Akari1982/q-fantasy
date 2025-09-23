#pragma once

#include "../psyq/libgpu.h"



extern DISPENV g_global_dispenv;
extern DRAWENV g_global_drawenv;



void SystemMain();
void SystemInitBase();
void SystemInitAkaoEngine();
void SystemInitDispEnvDrawEnv();
