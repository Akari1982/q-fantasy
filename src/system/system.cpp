#include "system.h"
#include "../field/field.h"
#include "../psyq/libgte.h"



DISPENV global_dispenv;
DRAWENV global_drawenv;



void system_main()
{
    system_init_base();

    system_init_dispenv_drawenv();

    field_main();
}



void system_init_base()
{
    InitGeom();
}



void system_init_dispenv_drawenv()
{
    SetDefDispEnv( &global_dispenv, 0x0, 0x0, 0x140, 0xe0 );
    SetDefDrawEnv( &global_drawenv, 0x0, 0x8, 0x140, 0xe0 );
    global_drawenv.dtd = 1;
    global_drawenv.isbg = 0;

    PutDispEnv( &global_dispenv );
    PutDrawEnv( &global_drawenv );
}