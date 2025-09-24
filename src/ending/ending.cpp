#include "ending.h"
#include "system/game.h"
#include "kernel/system.h"
#include "psyq/filesystem.h"
#include "psyq/libgte.h"

#include <vector>



void EndingMain()
{
    // Init area 0x280 x 0x1e0 for rendering.
    // Distance 0x200 and background color is black
//    ending_render_init( 0x280, 0x1e0, 0x200, 0, 0, 0 );

//    do
//    {
//        system_psyq_cd_search_file( SP + 0x20, "\STARTUP\SCEAP.LZS;1" );
//
//        if( V0 <= 0 )
//        {
//            if( V0 >= -1 )
//            {
//                system_bios_printf( "scea file read error" );
//                return;
//            }
//        }
//
//        system_psyq_cd_control_b( 0x2, SP + 0x20, 0 ); // execute command Setloc
//
//        system_psyq_cd_read( (w[SP + 0x24] + 0x7ff) >> 0xb, 0x80180000, 0x80 );
//
//        do
//        {
//            V0 = system_psyq_cd_read_sync( 0x1, 0 );
//        } while( V0 > 0 );
//    } while( V0 != 0 )
//
//    system_lzs_decompress( 0x80180000, 0x80100000 );
//
//    system_psyq_set_disp_mask( 1 );
//
//    for( int i = 0xfe; i <= 0; i -= 2 )
//    {
//        rb = rb < 0x1;
//
//        ending_render_vsync( 0 );
//
//        s16 even_odd = system_psyq_get_ode() ^ 0x1;
//
//        u32* image = 0x80100000 + even_odd * 0x500;
//
//        RECT rect;
//        rect.x = 0;
//        rect.y = even_odd;
//        rect.w = 0x280;
//        rect.h = 1;
//
//        while( rect.y < 0x1e0 )
//        {
//            system_psyq_load_image( &rect, image );
//            system_psyq_draw_sync( 0 );
//            rect.y += 0x2;
//            image += 0xa00;
//        }
//
//        u32* ot = 0x800a64e4 + rb * 0x4;
//        system_psyq_clear_otag_r( ot, 0x1 );
//
//        TILE* tile = 0x800a6504 * rb * 10;
//        system_psyq_set_tile( tile );
//        system_psyq_set_semi_trans( tile , 0x1 );
//        tile->r0 = i;
//        tile->g0 = i;
//        tile->b0 = i;
//        tile->x0 = 0x1e;
//        tile->y0 = 0xc8;
//        tile->w = 0x244;
//        tile->h = 0x4a;
//        system_psyq_add_prim( ot, tile );
//
//        DR_MODE* dm = 0x800a64ec + rb * 0xc;
//        system_psyq_set_draw_mode( dm, 0x1, 0x1, system_psyq_get_tpage( 0x2, 0x2, 0, 0 ), 0 );
//        system_psyq_add_prim( ot, dm );
//
//        system_psyq_draw_otag( ot );
//    }
//
//    for( int i = 0; i < 0x12c; ++i )
//    {
//        rb = rb < 0x1;
//
//        ending_render_vsync( 0 );
//
//        s16 even_odd = system_psyq_get_ode() ^ 0x1;
//
//        u32* image = 0x80100000 + even_odd * 0x500;
//
//        RECT rect;
//        rect.x = 0;
//        rect.y = even_odd;
//        rect.w = 0x280;
//        rect.h = 1;
//
//        while( rect.y < 0x1e0 )
//        {
//            system_psyq_load_image( &rect, image );
//            system_psyq_draw_sync( 0 );
//            image += 0xa00;
//            rect.y += 0x2;
//        }
//    }
//
//    for( int i = 0; i < fe; i += 2 )
//    {
//        rb = rb < 0x1;
//
//        ending_render_vsync( 0 );
//
//        s16 even_odd = system_psyq_get_ode() ^ 0x1;
//
//        u32* image = 0x80100000 + even_odd * 0x500;
//
//        RECT rect;
//        rect.x = 0;
//        rect.y = even_odd;
//        rect.w = 0x280;
//        rect.h = 1;
//
//        while( rect.y < 0x1e0 )
//        {
//            system_psyq_load_image( &rect, image );
//            system_psyq_draw_sync( 0 );
//            image += 0xa00;
//            rect.y += 0x2;
//        }
//
//        u32* ot = 0x800a64e4 + rb * 0x4;
//        system_psyq_clear_otag_r( ot, 0x1 );
//
//        TILE* tile = 0x800a6504 + rb * 0x10;
//        system_psyq_set_tile( tile );
//        system_psyq_set_semi_trans( tile, 0x1 );
//        tile->r0 = i;
//        tile->g0 = i;
//        tile->b0 = i;
//        tile->x0 = 0x1e;
//        tile->y0 = 0xc8;
//        tile->w = 0x244;
//        tile->h = 0x4a;
//        system_psyq_add_prim( ot, tile );
//
//        DR_MODE* dm = 0x800a64ec + rb * 0xc;
//        system_psyq_set_draw_mode( dm, 0x1, 0x1, system_psyq_get_tpage( 0x2, 0x2, 0, 0 ), 0 );
//        system_psyq_add_prim( ot, dm );
//
//        system_psyq_draw_otag( ot );
//    }
//
//    system_psyq_set_disp_mask( 0 );
}
