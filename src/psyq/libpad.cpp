#include "libpad.h"



u16 g_buttons_state_1;
u16 g_buttons_state_2;



void ButtonsPressed1( u32 buttons )
{
    g_buttons_state_1 |= buttons;
}



void ButtonsReleased1( u32 buttons )
{
    g_buttons_state_1 &= ~buttons;
}
