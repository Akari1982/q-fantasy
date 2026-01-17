#pragma once

#include "psyq/typedef.h"



#define GAME_CHARACTERS_N 0x9



struct Save
{
    u8 memory[0x500];
};



extern Save g_save;