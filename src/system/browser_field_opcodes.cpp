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
        .add_argument_memory2( "a", 0x1, 0x2 )
        .add_argument_memory2( "b", 0x2, 0x4 )
        .add_argument_u8( "cond", 0x6 )
        .add_argument_jump8( "jump", 0x7 )
        .end();

    g_field_opcodes[0x64]
        .set_size( 0x6 )
        .set_short_name( "scr2d" )
        .set_full_name( "BACKGROUND:SET_POSITION" )
        .set_desc( "Scroll to given coordinates. Instant." )
        .add_argument_memory2( "x", 0x1, 0x2 )
        .add_argument_memory2( "y", 0x2, 0x4 )
        .end();

    g_field_opcodes[0x6b]
        .set_size( 0x9 )
        .set_short_name( "fade" )
        .set_full_name( "FADE:INIT" )
        .set_desc( "Set fade." )
        .add_argument_u8( "steps", 0x6 )
        .add_argument_u8( "type", 0x7 )
        .add_argument_u8( "start_val", 0x8 )
        .add_argument_memory1( "r", 0x1, 0x3 )
        .add_argument_memory1( "g", 0x2, 0x4 )
        .add_argument_memory1( "b", 0x4, 0x5 )
        .end();
}
