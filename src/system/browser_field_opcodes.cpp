#include "browser_field_opcodes.h"

#include <array>



std::array<OpcodeInfo, 256> g_field_opcodes;



void FieldBrowserInitOpcode()
{
    g_field_opcodes[0x16]
        .set_size( 0x8 )
        .set_short_name( "if2" )
        .set_full_name( "IF" )
        .set_desc( "Checks condition and jump if it fails." )
        .end();

    g_field_opcodes[0x64]
        .set_size( 0x6 )
        .set_short_name( "scr2d" )
        .set_full_name( "BACKGROUND:SET_POSITION" )
        .set_desc( "Scroll to given coordinates. Instant." )
        .add_argument_memory2( "X", 0x1, 0x2 )
        .add_argument_memory2( "Y", 0x2, 0x4 )
        .end();
}
