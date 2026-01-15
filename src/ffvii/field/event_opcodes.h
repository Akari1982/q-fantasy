#include "system/logger.h"



typedef int (*FieldEventOpcode)();



int FieldEventNull()
{
    LOG_ERROR("Opcode 0x%02x not implemented.", l_opcode_cur);
    return -1;
}



int FieldEventOpcode_a0_pc()
{
    u16 script_cur = l_actor_script[l_actor_id_cur];
    u16 entity_cur = l_actor_entity[l_actor_id_cur];

//    if (bu[0x8009d820] & 0x3) field_debug_event_opcode("pc", 0x1);

    u8 char_id = READ_LE_U8(g_field_events + script_cur + 0x1);
    l_char_actor[char_id] = l_actor_id_cur;

    for (int i = 0; i < 0x3; ++i)
    {
//        if (bu[0x8009c6e4 + 0xcad + i] == char_id) // party member in slot 1-3
        {
            if (i != 0) // hide not party leader
            {
                g_field_entities[entity_cur].visible = 0;
                g_field_entities[entity_cur].solid_off = 0x1;
                g_field_entities[entity_cur].talk_off = 0x1;
            }
            else
            {
                g_field_control.player_id = entity_cur;
            }

            l_actor_script[l_actor_id_cur] += 0x2;
            return 0;
        }
    }

    g_field_entities[entity_cur].visible = 0;
    g_field_entities[entity_cur].solid_off = 0x1;
    g_field_entities[entity_cur].talk_off = 0x1;

    l_actor_script[l_actor_id_cur] += 0x2;
    return 0;
}



int FieldEventOpcode_a1_char()
{
    u16 script_cur = l_actor_script[l_actor_id_cur];

//    if (bu[0x8009d820] & 0x3) field_debug_event_opcode("char", 0x1);

    u8 entity_id = l_entity_id_cur;
    l_entity_id_cur += 0x1;
    l_actor_entity[l_actor_id_cur] = entity_id;

    g_field_entities[entity_id].actor_id = l_actor_id_cur;
    g_field_entities[entity_id].visible = 0x1;
    g_field_entities[entity_id].char_id = READ_LE_U8(g_field_events + script_cur + 0x1);

    l_actor_script[l_actor_id_cur] += 0x2;
    return 0;
}



int FieldEventOpcode_a5_xyzi()
{
    u16 entity_cur = l_actor_entity[l_actor_id_cur];

    if (entity_cur != 0xff)
    {
//        if (bu[0x8009d820] & 0x3) field_debug_event_opcode("xyzi", 0x8);

        g_field_entities[entity_cur].pos_x = FieldEventReadMemoryS16(0x1, 0x3) << 0xc;
        g_field_entities[entity_cur].pos_y = FieldEventReadMemoryS16(0x2, 0x5) << 0xc;
        g_field_entities[entity_cur].pos_z = FieldEventReadMemoryS16(0x3, 0x7) << 0xc;
        g_field_entities[entity_cur].pos_i = FieldEventReadMemoryS16(0x4, 0x9);
    }

    l_actor_script[l_actor_id_cur] += 0xb;
    return 0x1;
}
