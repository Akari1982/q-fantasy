#include "buttons.h"

u16 l_buttons_state_1;
u16 l_buttons_state_2;



u32 ButtonsGetCurrent()
{
    u16 buttons_1 = 0;

//    if( l_tutorial_on == 0 )
//    {
        buttons_1 = l_buttons_state_1;
//    }
//    else // for tutorial
//    {
//        buttons_1 = ButtonsGetTutorial();
//    }

    return l_buttons_state_2 << 0x8 | l_buttons_state_1;
}



void ButtonsPressed1( u32 buttons )
{
    l_buttons_state_1 |= buttons;
}



void ButtonsReleased1( u32 buttons )
{
    l_buttons_state_1 &= ~buttons;
}
