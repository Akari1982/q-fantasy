#include "event.h"



void FieldEventInitDefault()
{
    g_field_control.cmd = FIELD_CMD_NONE;
    g_field_control.map_id = 0;
}



void FieldEventInit()
{
    FieldEventInitDefault();
}
