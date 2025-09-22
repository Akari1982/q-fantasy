#include "browser_field_opcodes.h"

#include <array>



std::array<OpcodeInfo, 256> g_field_opcodes;



void FieldBrowserInitOpcode()
{
    g_field_opcodes[0x16]
        .set_size( 0x8 )
        .set_short_name( "if2" )
        .set_full_name( "IF:S16" )
        .set_desc( "Checks condition on signed 2-bytes value and jump if it fails." )
        .add_argument_read_memory16( "a", 0x1, 0x2 )
        .add_argument_read_memory16( "b", 0x2, 0x4 )
        .add_argument_u8( "cond", 0x6 )
        .add_argument_jump8( "jump", 0x7 )
        .end();

    g_field_opcodes[0x64]
        .set_size( 0x6 )
        .set_short_name( "scr2d" )
        .set_full_name( "BACKGROUND:SET_POSITION" )
        .set_desc( "Scroll to given coordinates. Instant." )
        .add_argument_read_memory16( "x", 0x1, 0x2 )
        .add_argument_read_memory16( "y", 0x2, 0x4 )
        .end();

    g_field_opcodes[0x6b]
        .set_size( 0x9 )
        .set_short_name( "fade" )
        .set_full_name( "FADE:INIT" )
        .set_desc( "Set fade." )
        .add_argument_u8( "steps", 0x6 )
        .add_argument_u8( "type", 0x7 )
        .add_argument_u8( "start_val", 0x8 )
        .add_argument_read_memory8( "r", 0x1, 0x3 )
        .add_argument_read_memory8( "g", 0x2, 0x4 )
        .add_argument_read_memory8( "b", 0x4, 0x5 )
        .end();

    g_field_opcodes[0x71]
        .set_size( 0x2 )
        .set_short_name( "btlon" )
        .set_full_name( "BATTLE:ENCOUNTER" )
        .set_desc( "Enable or disable encounter." )
        .add_argument_u8( "disable", 0x1 )
        .set_can_break()
        .end();

    g_field_opcodes[0x72]
        .set_size( 0x3 )
        .set_short_name( "btlmd" )
        .set_full_name( "BATTLE:MODE" )
        .set_desc( "" )
        .add_argument_u8( "flags1", 0x1 )
        .add_argument_u8( "flags2", 0x2 )
        .end();

    g_field_opcodes[0x81]
        .set_size( 0x5 )
        .set_short_name( "set2" )
        .set_full_name( "SET:S16" )
        .set_desc( "Set variable." )
        .add_argument_write_memory16( "dst", 0x1, 0x2 )
        .add_argument_read_memory16( "src", 0x2, 0x3 )
        .end();

    g_field_opcodes[0xf0]
        .set_size( 0x2 )
        .set_short_name( "music" )
        .set_full_name( "AKAO:MUSIC_PLAY" )
        .set_desc( "Plays music from field file." )
        .add_argument_u8( "music", 0x1 )
        .end();

    g_field_opcodes[0xf2]
        .set_size( 0xe )
        .set_short_name( "akao" )
        .set_full_name( "AKAO:COMMAND" )
        .set_desc( "Execute AKAO command." )
        .add_argument_u8( "command", 0x4 )
        .add_argument_read_memory8( "arg0", 0x1, 0x5 )
        .add_argument_read_memory16( "arg1", 0x2, 0x6 )
        .add_argument_read_memory16( "arg2", 0x3, 0x8 )
        .add_argument_read_memory16( "arg3", 0x4, 0xa )
        .add_argument_read_memory16( "arg4", 0x6, 0xc )
        .end();

    g_field_opcodes[0xfc]
        .set_size( 0x2 )
        .set_short_name( "fmusc" )
        .set_full_name( "AKAO:SET_FIELD_MUSIC" )
        .set_desc( "Set field music." )
        .add_argument_u8( "fmusic", 0x1 )
        .end();
}
