#include "newgame.h"
#include "menu.h"
#include "kernel/file.h"
#include "psyq/libgpu.h"

#include <vector>
#include <memory>
#include <functional>

#define NEWGAME_FADEIN  0
#define NEWGAME_MENU    1
#define NEWGAME_FADEOUT 2
#define NEWGAME_FINISH -1

u32 l_newgame_state;
u32 l_newgame_rb;
OTag* l_newgame_otag_p;
OTag l_newgame_otag[0x2][0x4];

DRAWENV l_newgame_drawenv[0x2];
DISPENV l_newgame_dispenv[0x2];

std::vector<std::unique_ptr<OTag>> l_newgame_draw[0x2];

DVECTOR l_newgame_center = {0x96, 0x5d};



void MenuNewGameInit()
{
    std::vector<u8> file;
    FileRead( "MENU/SAVEMENU.MNU", file );
    std::vector<u8> part_file(file.begin() + 0x4edc, file.end());
    MenuLoadImage( part_file.begin(), 0x380, 0, 0, 0x1e0 );
}



bool MenuNewGameUpdate( u32 frame )
{
    static int y = 0;
    if( g_buttons_1_repeated == BUTTON_UP ) y -= 0x5;
    if( g_buttons_1_repeated == BUTTON_DOWN ) y += 0x5;
    if( g_buttons_1_repeated == BUTTON_CIRCLE ) l_newgame_state = NEWGAME_FINISH;

    MenuDrawCursor( l_newgame_center.vx - 0x12, l_newgame_center.vy + 0x6 + y );

    MenuStr str_new = MenuStr("NEW GAME");
    MenuStr str_con = MenuStr("Continue?");
    MenuDrawString( l_newgame_center.vx + 0x8, l_newgame_center.vy + 0x6, str_new, 0x7 );
    MenuDrawString( l_newgame_center.vx + 0x8, l_newgame_center.vy + 0x12, str_con, 0x0 );

    SRECT rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = 0x100;
    rect.h = 0x100;
    MenuSetDrawMode( 0, 0x1, 0x7f, &rect );

    auto poly = std::make_unique<POLY_FT4>();
    PsyqSetPolyFT4( poly.get() );
    poly->r0 = 0x60;
    poly->g0 = 0x60;
    poly->b0 = 0x60;
    poly->x0 = 0x6f;
    poly->y0 = 0x54;
    poly->u0 = 0;
    poly->v0 = 0;
    poly->clut = PsyqGetClut( 0, 0x1e0 );
    poly->x1 = 0x119;
    poly->y1 = 0x54;
    poly->u1 = 0x96;
    poly->v1 = 0;
    poly->tpage = PsyqGetTPage( 0x1, 0, 0x380, 0 );
    poly->x2 = 0x6f;
    poly->y2 = 0xea;
    poly->u2 = 0;
    poly->v2 = 0x96;
    poly->x3 = 0x119;
    poly->y3 = 0xea;
    poly->u3 = 0x96;
    poly->v3 = 0x96;
    PsyqAddPrim( g_menu_otag, poly.get() );
    g_menu_poly->emplace_back(std::move(poly));

    return false;
}



bool MenuNewGameMain()
{
    MenuCreateDrawenvDispenv( l_newgame_drawenv, l_newgame_dispenv );

    u32 frame = 0;
    bool result = false;

    l_newgame_state = NEWGAME_FADEIN;

    MenuNewGameInit();

    l_newgame_rb = 0;

    while( true )
    {
        MenuUpdateButtons();

        l_newgame_draw[l_newgame_rb].clear();
        MenuSetPoly( &l_newgame_draw[l_newgame_rb] );

        l_newgame_otag_p = l_newgame_otag[l_newgame_rb];
        PsyqClearOTag( l_newgame_otag_p , 0x1 );
        MenuSetOTag( l_newgame_otag_p );

//        system_menu_draw_add_window();

        result = MenuNewGameUpdate( frame );

        if( l_newgame_state == NEWGAME_FINISH ) break;

        PsyqVSync( 0 );

        PsyqPutDispEnv( &l_newgame_dispenv[l_newgame_rb] );
        PsyqPutDrawEnv( &l_newgame_drawenv[l_newgame_rb] );

        frame += 0x1;

        PsyqDrawOTag( l_newgame_otag_p );

        l_newgame_rb ^= 0x1;
    }

//    func1d4c38();

    PsyqVSync( 0 );

//    PsyqPutDispEnv( 0x801e3f00 );
//    PsyqPutDrawEnv( 0x801e3e90 );

//    for( int i = 0; i < 0x3; ++i )
//    {
//        if( bu[0x8009cbdc + i] != 0xff )
//        {
//            system_init_player_stat_from_equip( i );
//            system_init_player_stat_from_materia( i );
//        }
//    }

//    system_calculate_total_lure_gil_preemptive_value();

    return result;
}
