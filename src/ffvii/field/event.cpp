#include "event.h"



u8* g_field_events;



void FieldEventInitDefault()
{
    g_field_control.cmd = FIELD_CMD_NONE;
    g_field_control.arg = 0;

    g_field_control.scale = READ_LE_U16(g_field_events + 0x8);
    g_field_control.entities_n = READ_LE_U8(g_field_events + 0x3);
    g_field_control.player_id = 0;
    g_field_control.anim_stand = 0;
    g_field_control.anim_walk = 0x1;
    g_field_control.anim_run = 0x2;
    g_field_control.control_lock = 0;
    g_field_control.move_lock = 0;
    g_field_control.gateway_lock = 0;
    g_field_control.speed_up = 0;
}



void FieldEventInit(u8* events)
{
    g_field_events = events;

    FieldEventInitDefault();
}
