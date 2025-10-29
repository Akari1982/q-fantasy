#pragma once

#include "typedef.h"



#define BUTTON_L2       0x0001
#define BUTTON_R2       0x0002
#define BUTTON_L1       0x0004
#define BUTTON_R1       0x0008
#define BUTTON_TRIANGLE 0x0010
#define BUTTON_CIRCLE   0x0020
#define BUTTON_CROSS    0x0040
#define BUTTON_SQUARE   0x0080
#define BUTTON_SELECT   0x0100
#define BUTTON_START    0x0800
#define BUTTON_UP       0x1000
#define BUTTON_RIGHT    0x2000
#define BUTTON_DOWN     0x4000
#define BUTTON_LEFT     0x8000



void ButtonsPressed1( u32 buttons );
void ButtonsReleased1( u32 buttons );



extern u16 g_buttons_state_1;
extern u16 g_buttons_state_2;
