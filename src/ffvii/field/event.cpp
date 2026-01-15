#include "event.h"
#include "entity.h"
#include "kernel/save.h"

#define FIELD_ACTORS_N 0x30

u8* g_field_events;
u8 l_actor_id_cur;
u8 l_entity_id_cur;
u16 l_actor_script[FIELD_ACTORS_N];
u16 l_actor_entity[FIELD_ACTORS_N];
u16 l_char_actor[GAME_CHARACTERS_N];
u8 l_opcode_cur;

#include "event_opcodes.h"
FieldEventOpcode event_opcodes[] =
{
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventOpcode_a0_pc, FieldEventOpcode_a1_char, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull,
    FieldEventNull,         FieldEventNull,           FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull, FieldEventNull
};



void FieldEventInitRun();



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
    FieldEventInitRun();
}



void FieldEventInitRun()
{
    u8 actors_n = READ_LE_U8(g_field_events + 0x2);
    u16 akao_n = READ_LE_U16(g_field_events + 0x6);

    l_entity_id_cur = 0;

    for (int i = 0; i < actors_n; ++i)
    {
        l_actor_id_cur = i;

        l_actor_script[i] = READ_LE_U16(g_field_events + 0x20 + actors_n * 0x8 + akao_n * 0x4 + i * 0x40); // get offset to init script (1st)

        // Init scripts run all opcodes until 0x00 met.
        // Default scripts starts right after.
        while (l_opcode_cur = READ_LE_U8(g_field_events + l_actor_script[i]))
        {
            if (event_opcodes[l_opcode_cur]() == -1) break;
        }

        // skip ret opcode for next run
        l_actor_script[i] += 0x1;
    }

    l_actor_id_cur = 0; // clear current actor
}



s16 FieldEventReadMemoryS16(s16 bank_id, s16 ofs)
{
    actor_id_cur = bu[0x800722c4];
    events_data = w[0x8009c6dc];
    script_cur = hu[0x800831fc + actor_id_cur * 0x2];

    u8 bank = -1;

    switch (bank_id - 0x1)
    {
        case 0x0: bank = bu[events_data + script_cur + 0x1] >> 0x4; break;
        case 0x1: bank = bu[events_data + script_cur + 0x1] & 0xf;  break;
        case 0x2: bank = bu[events_data + script_cur + 0x2] >> 0x4; break;
        case 0x3: bank = bu[events_data + script_cur + 0x2] & 0xf;  break;
        case 0x4: bank = bu[events_data + script_cur + 0x3] >> 0x4; break;
        case 0x5: bank = bu[events_data + script_cur + 0x3] & 0xf;  break;
    }

    switch (bank)
    {
        case 0x0: // memory bank 0
        {
            u16 val = hu[events_data + script_cur + ofs];

            if (bu[0x8009d820] & 0x3)
            {
                field_debug_add_parse_value_to_page2("G cons=", val, 0x4);
            }

            return val;
        }
        break;

        case 0x1: // memory bank 1
        {
            u32 indx = bu[events_data + script_cur + ofs];
            u16 val = bu[0x8009d288 + indx];

            if (bu[0x8009d820] & 0x3)
            {
                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
            }

            return val;
        }
        break;

        case 0x2: // memory bank 2
        {
            u32 indx = bu[events_data + script_cur + ofs];
            u16 val = hu[0x8009d288 + indx];

            if (bu[0x8009d820] & 0x3)
            {
                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
            }

            return val;
        }
        break;

        case 0x3: // memory bank 3
        {
            u32 indx = 0x100 | bu[events_data + script_cur + ofs];
            u16 val = bu[0x8009d288 + indx];

            if (bu[0x8009d820] & 0x3)
            {
                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
            }

            return val;
        }
        break;

        case 0x4: // memory bank 4
        {
            u32 indx = 0x100 | bu[events_data + script_cur + ofs];
            u16 val = h[0x8009d288 + indx];

            if (bu[0x8009d820] & 0x3)
            {
                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
            }

            return val;
        }
        break;

        case 0xb: // memory bank B
        {
            u32 indx = 0x200 | bu[events_data + script_cur + ofs];
            u16 val = bu[0x8009d288 + indx];

            if (bu[0x8009d820] & 0x3)
            {
                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
            }

            return val;
        }
        break;

        case 0xc: // memory bank C
        {
            u32 indx = 0x200 | bu[events_data + script_cur + ofs];
            u16 val = hu[0x8009d288 + indx];

            if (bu[0x8009d820] & 0x3)
            {
                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
            }

            return val;
        }
        break;

        case 0xd: // memory bank D
        {
            u32 indx = 0x300 | bu[events_data + script_cur + ofs];
            u16 val = bu[0x8009d288 + indx];

            if (bu[0x8009d820] & 0x3)
            {
                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
            }

            return val;
        }
        break;

        case 0xe: // memory bank E
        {
            u32 indx = 0x300 | bu[events_data + script_cur + ofs];
            u16 val = h[0x8009d288 + indx];

            if (bu[0x8009d820] & 0x3)
            {
                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
            }

            return val;
        }
        break;

        case 0xf: // memory bank F
        {
            u32 indx = 0x400 | bu[events_data + script_cur + ofs];
            u16 val = bu[0x8009d288 + indx];

            if (bu[0x8009d820] & 0x3)
            {
                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
            }

            return val;
        }
        break;

        case 0x7: // memory bank 7
        {
            u32 indx = 0x400 | bu[events_data + script_cur + ofs];
            u16 val = hu[0x8009d288 + indx];

            if (bu[0x8009d820] & 0x3)
            {
                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
            }

            return val;
        }
        break;

        case 0x5: // memory bank 5
        {
            u32 indx = bu[events_data + script_cur + ofs];
            u16 val = bu[0x80075e24 + indx];

            if (bu[0x8009d820] & 0x3)
            {
                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
                field_debug_add_parse_value_to_page2("G mapv=", val, 0x4);
            }

            return val;
        }
        break;

        case 0x6: // memory bank 6
        {
            u32 indx = bu[events_data + script_cur + ofs];
            u16 val = hu[0x80075e24 + indx];

            if (bu[0x8009d820] & 0x3)
            {
                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
                field_debug_add_parse_value_to_page2("G mapv=", val, 0x4);
            }

            return val;
        }
    }

    // memory bank 8, 9, A, >F
    if (bu[0x8009d820] & 0x3)
    {
        field_debug_add_parse_value_to_page2("G data err=", access, 0x2);
    }

    field_event_debug_error("Bad Event arg!");

    return 0;
}
