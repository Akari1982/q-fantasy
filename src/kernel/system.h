#pragma once

#include "../psyq/libgpu.h"



extern DISPENV global_dispenv;
extern DRAWENV global_drawenv;



void system_main();
void system_init_base();
void system_init_akao_engine();
void system_init_dispenv_drawenv();
