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
u8 l_memory_temp[0x100];

u8 FieldEventReadMemoryU8(s16 bank_id, s16 ofs);
s16 FieldEventReadMemoryS16(s16 bank_id, s16 ofs);

#include "event_opcodes.h"
FieldEventOpcode event_opcodes[] =
{
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FEOpcode_7e_tlkon, FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FEOpcode_a0_pc, FEOpcode_a1_char, FEOpcode_a2_dfanm, FENull,          FEOpcode_a4_visi, FEOpcode_a5_xyzi, FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FEOpcode_b3_dir, FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FEOpcode_c6_slidr, FEOpcode_c7_solid,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull,
    FENull,         FENull,           FENull,            FENull,          FENull,           FENull,           FENull,            FENull
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



u8 FieldEventReadMemoryU8(s16 bank_id, s16 ofs)
{
    u16 script_cur = l_actor_script[l_actor_id_cur];

    u8 bank = -1;

    switch (bank_id)
    {
        case 0x0: bank = READ_LE_U8(g_field_events + script_cur + 0x1) >> 0x4; break;
        case 0x1: bank = READ_LE_U8(g_field_events + script_cur + 0x1) & 0xf;  break;
        case 0x2: bank = READ_LE_U8(g_field_events + script_cur + 0x2) >> 0x4; break;
        case 0x3: bank = READ_LE_U8(g_field_events + script_cur + 0x2) & 0xf;  break;
        case 0x4: bank = READ_LE_U8(g_field_events + script_cur + 0x3) >> 0x4; break;
        case 0x5: bank = READ_LE_U8(g_field_events + script_cur + 0x3) & 0xf;  break;
    }

    switch(bank)
    {
        case 0x0: // memory bank 0
        {
            u8 val = READ_LE_U8(g_field_events + script_cur + ofs);

//            if (bu[0x8009d820] & 0x3)
//            {
//                field_debug_add_parse_value_to_page2("G cons=", val, 0x2);
//            }

            return val;
        }

        case 0x1: // memory bank 1
        case 0x2: // memory bank 2
        {
            u16 indx = READ_LE_U8(g_field_events + script_cur + ofs);
            u8 val = READ_LE_U8(&g_save.memory[indx]);

//            if (bu[0x8009d820] & 0x3) // debug
//            {
//                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
//                field_debug_add_parse_value_to_page2("G glov=", val, 0x2);
//            }

            return val;
        }

        case 0x3: // memory bank 3
        case 0x4: // memory bank 4
        {
            u8 indx = READ_LE_U8(g_field_events + script_cur + ofs);
            u8 val = READ_LE_U8(&g_save.memory[0x100 + indx]);

//            if (bu[0x8009d820] & 0x3) // debug
//            {
//                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
//                field_debug_add_parse_value_to_page2("G glov=", val, 0x2);
//            }

            return val;
        }

        case 0xb: // memory bank B
        case 0xc: // memory bank C
        {
            u8 indx = READ_LE_U8(g_field_events + script_cur + ofs);
            u8 val = READ_LE_U8(&g_save.memory[0x200 + indx]);

//            if (bu[0x8009d820] & 0x3) // debug
//            {
//                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
//                field_debug_add_parse_value_to_page2("G glov=", val, 0x2);
//            }

            return val;
        }

        case 0xd: // memory bank D
        case 0xe: // memory bank E
        {
            u8 indx = READ_LE_U8(g_field_events + script_cur + ofs);
            u8 val = READ_LE_U8(&g_save.memory[0x300 + indx]);

//            if (bu[0x8009d820] & 0x3) // debug
//            {
//                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
//                field_debug_add_parse_value_to_page2("G glov=", val, 0x2);
//            }

            return val;
        }

        case 0x7: // memory bank 7
        case 0xf: // memory bank F
        {
            u8 indx = READ_LE_U8(g_field_events + script_cur + ofs);
            u8 val = READ_LE_U8(&g_save.memory[0x400 + indx]);

//            if (bu[0x8009d820] & 0x3) // debug
//            {
//                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
//                field_debug_add_parse_value_to_page2("G glov=", val, 0x2);
//            }

            return val;
        }

        case 0x5: // memory bank 5
        case 0x6: // memory bank 6
        {
            u8 indx = READ_LE_U8(g_field_events + script_cur + ofs);
            u8 val = READ_LE_U8(l_memory_temp + indx);

//            if (bu[0x8009d820] & 0x3) // debug
//            {
//                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
//                field_debug_add_parse_value_to_page2("G mapv=", val, 0x2);
//            }

            return val;
        }
    }

//    if (bu[0x8009d820] & 0x3) // debug
//    {
//        field_debug_add_parse_value_to_page2("G data err=", bank, 0x2);
//    }

//    field_event_debug_error("Bad Event arg!");

    return 0;
}



s16 FieldEventReadMemoryS16(s16 bank_id, s16 ofs)
{
    u16 script_cur = l_actor_script[l_actor_id_cur];

    u8 bank = -1;

    switch (bank_id)
    {
        case 0x1: bank = READ_LE_U8(g_field_events + script_cur + 0x1) >> 0x4; break;
        case 0x2: bank = READ_LE_U8(g_field_events + script_cur + 0x1) & 0xf;  break;
        case 0x3: bank = READ_LE_U8(g_field_events + script_cur + 0x2) >> 0x4; break;
        case 0x4: bank = READ_LE_U8(g_field_events + script_cur + 0x2) & 0xf;  break;
        case 0x5: bank = READ_LE_U8(g_field_events + script_cur + 0x3) >> 0x4; break;
        case 0x6: bank = READ_LE_U8(g_field_events + script_cur + 0x3) & 0xf;  break;
    }

    switch (bank)
    {
        case 0x0: // memory bank 0
        {
            u16 val = READ_LE_U16(g_field_events + script_cur + ofs);

//            if (bu[0x8009d820] & 0x3)
//            {
//                field_debug_add_parse_value_to_page2("G cons=", val, 0x4);
//            }

            return val;
        }
        break;

        case 0x1: // memory bank 1
        {
            u8 indx = READ_LE_U8(g_field_events + script_cur + ofs);
            s16 val = READ_LE_S8(&g_save.memory[indx]);

//            if (bu[0x8009d820] & 0x3)
//            {
//                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
//                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
//            }

            return val;
        }
        break;

        case 0x2: // memory bank 2
        {
            u8 indx = READ_LE_U8(g_field_events + script_cur + ofs);
            s16 val = READ_LE_S16(&g_save.memory[indx]);

//            if (bu[0x8009d820] & 0x3)
//            {
//                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
//                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
//            }

            return val;
        }
        break;

        case 0x3: // memory bank 3
        {
            u8 indx = READ_LE_U8(g_field_events + script_cur + ofs);
            s16 val = READ_LE_S8(&g_save.memory[0x100 + indx]);

//            if (bu[0x8009d820] & 0x3)
//            {
//                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
//                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
//            }

            return val;
        }
        break;

        case 0x4: // memory bank 4
        {
            u8 indx = READ_LE_U8(g_field_events + script_cur + ofs);
            s16 val = READ_LE_S16(&g_save.memory[0x100 + indx]);

//            if (bu[0x8009d820] & 0x3)
//            {
//                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
//                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
//            }

            return val;
        }
        break;

        case 0xb: // memory bank B
        {
            u8 indx = READ_LE_U8(g_field_events + script_cur + ofs);
            s16 val = READ_LE_S8(&g_save.memory[0x200 + indx]);

//            if (bu[0x8009d820] & 0x3)
//            {
//                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
//                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
//            }

            return val;
        }
        break;

        case 0xc: // memory bank C
        {
            u8 indx = READ_LE_U8(g_field_events + script_cur + ofs);
            s16 val = READ_LE_S16(&g_save.memory[0x200 + indx]);

//            if (bu[0x8009d820] & 0x3)
//            {
//                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
//                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
//            }

            return val;
        }
        break;

        case 0xd: // memory bank D
        {
            u8 indx = READ_LE_U8(g_field_events + script_cur + ofs);
            s16 val = READ_LE_S8(&g_save.memory[0x300 + indx]);

//            if (bu[0x8009d820] & 0x3)
//            {
//                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
//                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
//            }

            return val;
        }
        break;

        case 0xe: // memory bank E
        {
            u8 indx = READ_LE_U8(g_field_events + script_cur + ofs);
            s16 val = READ_LE_S16(&g_save.memory[0x300 + indx]);

//            if (bu[0x8009d820] & 0x3)
//            {
//                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
//                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
//            }

            return val;
        }
        break;

        case 0xf: // memory bank F
        {
            u8 indx = READ_LE_U8(g_field_events + script_cur + ofs);
            s16 val = READ_LE_S8(&g_save.memory[0x400 + indx]);

//            if (bu[0x8009d820] & 0x3)
//            {
//                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
//                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
//            }

            return val;
        }
        break;

        case 0x7: // memory bank 7
        {
            u8 indx = READ_LE_U8(g_field_events + script_cur + ofs);
            s16 val = READ_LE_S16(&g_save.memory[0x400 + indx]);

//            if (bu[0x8009d820] & 0x3)
//            {
//                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
//                field_debug_add_parse_value_to_page2("G glov=", val, 0x4);
//            }

            return val;
        }
        break;

        case 0x5: // memory bank 5
        {
            u8 indx = READ_LE_U8(g_field_events + script_cur + ofs);
            s16 val = READ_LE_S8(l_memory_temp + indx);

//            if (bu[0x8009d820] & 0x3)
//            {
//                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
//                field_debug_add_parse_value_to_page2("G mapv=", val, 0x4);
//            }

            return val;
        }
        break;

        case 0x6: // memory bank 6
        {
            u8 indx = READ_LE_U8(g_field_events + script_cur + ofs);
            s16 val = READ_LE_S16(l_memory_temp + indx);

//            if (bu[0x8009d820] & 0x3)
//            {
//                field_debug_add_parse_value_to_page2("G indx=", indx, 0x4);
//                field_debug_add_parse_value_to_page2("G mapv=", val, 0x4);
//            }

            return val;
        }
    }

    // memory bank 8, 9, A, >F
//    if (bu[0x8009d820] & 0x3)
//    {
//        field_debug_add_parse_value_to_page2("G data err=", access, 0x2);
//    }

//    field_event_debug_error("Bad Event arg!");

    return 0;
}
