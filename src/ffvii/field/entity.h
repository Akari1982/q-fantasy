#pragma once

#include "psyq/typedef.h"
#include "psyq/libgpu.h"



struct FieldEntity
{
                                // 0x00 [][]     store 1 here in KAWAI opcode. Store 2 here if run_kawai function returns 1 (maybe end kawai)
                                // 0x02 [][]     store 0 here in KAWAI opcode.
                                // 0x04 [][][][] kawai settings. Offset to KAWAI opcode data in script (like offset to opcode + 3).
                                // 0x08 []       blinking. 0 - on, 1 - off.
                                // 0x09 []       store 0 here in KAWAI opcode under some curcumstances.
    s32 pos_x;                  // 0x0c
    s32 pos_y;                  // 0x10
    s32 pos_z;                  // 0x14
    s32 move_start_x;           // 0x18
    s32 move_start_y;           // 0x1c
    s32 move_start_z;           // 0x20
    s16 move_b;                 // 0x2c
    s16 move_steps;             // 0x30
    s16 move_step;              // 0x32
    u8 move_dir_add;            // 0x35
    u8 move_dir;                // 0x36
    u8 dir_lock;                // 0x37
    u8 dir;                     // 0x38
    u8 turn_steps;              // 0x39
    u8 turn_step;               // 0x3a
    u8 turn_type;               // 0x3b
    s16 turn_start;             // 0x3c
    s16 turn_end;               // 0x3e
    s16 ofs_x;                  // 0x40
    s16 ofs_start_x;            // 0x42
    s16 ofs_end_x;              // 0x44
    s16 ofs_y;                  // 0x46
    s16 ofs_start_y;            // 0x48
    s16 ofs_end_y;              // 0x4a
    s16 ofs_z;                  // 0x4c
    s16 ofs_start_z;            // 0x4e
    s16 ofs_end_z;              // 0x50
    u16 ofs_steps;              // 0x52
    u16 ofs_step;               // 0x54
    u8 ofs_type;                // 0x56
                                // 0x57 []       actor id that controls this model (set in CHAR opcode).
                                // 0x58 []       pc entity collide with this entity. (1 - true/0 - false).
                                // 0x59 []       model solidity (1 - off, 0 - on).
                                // 0x5a []       pc entity talk with this entity. (1 - true/0 - false).
                                // 0x5b []       model talkability (1 - off, 0 - on).
    u8 visible;                 // 0x5c
    u8 action;                  // 0x5d
    u8 anim_id;                 // 0x5e
    u16 anim_speed;             // 0x60
    u16 anim_frame;             // 0x62
    u16 anim_frames_n;          // 0x64
                                // 0x66 [][]     char id. Set with CHAR opcode.
    s16 action_arg;             // 0x68
    s16 action_state;           // 0x6a
    u16 solid_range;            // 0x6c
    u16 talk_range;             // 0x6e
    u16 move_speed;             // 0x70
    s16 pos_i;                  // 0x72
    s16 move_end_i;             // 0x74
    s32 move_end_x;             // 0x78
    s32 move_end_y;             // 0x7c
    s32 move_end_z;             // 0x80
};

struct FieldLine
{
    s16 ax;                     // 0x00
    s16 ay;                     // 0x02
    s16 az;                     // 0x04
    s16 bx;                     // 0x06
    s16 by;                     // 0x08
    s16 bz;                     // 0x0a
    u8 on;                      // 0x0c
    u8 actor_id;                // 0x0d
    u8 ev_inside;               // 0x0e
    u8 ev_cross;                // 0x0f
    u8 ev_towards;              // 0x10
    u8 ev_talk;                 // 0x11
    u8 ev_enter;                // 0x12
    u8 ev_leave;                // 0x13
    u8 dir_to_line;             // 0x14
    u8 can_talk;                // 0x15
    u8 slip;                    // 0x16
};

struct FieldWalkmesh
{
    SVECTOR p1;                 // 0x00
    SVECTOR p2;                 // 0x08
    SVECTOR p3;                 // 0x10
};

struct FieldWalkmeshLink
{
    s16 p1_id;                  // 0x00
    s16 p2_id;                  // 0x02
    s16 p3_id;                  // 0x04
};



void FieldEntityDebugAddToRender(OTag* ot, MATRIX* m);

void FieldEntityInitPos();



extern std::vector<FieldWalkmesh> g_field_wm;
extern std::vector<FieldWalkmeshLink> g_field_wm_link;
