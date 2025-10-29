#include "buttons.h"



u32 ButtonsGetCurrent()
{
    u16 buttons_1 = 0;

//    if( l_tutorial_on == 0 )
//    {
        buttons_1 = g_buttons_state_1;
//    }
//    else // for tutorial
//    {
//        buttons_1 = ButtonsGetTutorial();
//    }

    return g_buttons_state_2 << 0x8 | g_buttons_state_1;
}
