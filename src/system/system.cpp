#include "system.h"
#include "../field/field.h"
#include "../psyq/libgte.h"



void
system_main()
{
    system_init_base();

    field_main();
}



void
system_init_base()
{
    InitGeom();
}
