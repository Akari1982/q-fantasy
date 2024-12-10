#include "system.h"
#include "../field/field.h"
#include "../psyq/libgte.h"



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
    A0 = 8007eb68 + 0 * 14; // DISPENV
    A1 = 0;
    A2 = e8;
    A3 = 140;
    A4 = f0;
    system_psyq_set_def_dispenv();

    A0 = 8007eb68 + 1 * 14; // DISPENV
    A1 = 0;
    A2 = 0;
    A3 = 140;
    A4 = f0;
    system_psyq_set_def_dispenv();

    A0 = 8007eaac + 0 * 5c; // DRAWENV
    A1 = 0;
    A2 = 8;
    A3 = 140;
    A4 = e0;
    system_psyq_set_def_drawenv();

    A0 = 8007eaac + 1 * 5c; // DRAWENV
    A1 = 0;
    A2 = f0;
    A3 = 140;
    A4 = e0;
    system_psyq_set_def_drawenv();

    [8007eac2] = b(1);
    [8007eb1e] = b(1);
    [8007eac4] = b(0);
    [8007eb20] = b(0);

    A0 = 8007eb68;
    system_psyq_put_dispenv();


    A0 = 8007eaac;
    system_psyq_put_drawenv();
}