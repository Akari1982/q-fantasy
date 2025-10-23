#include "menu.h"
#include "psyq/libgpu.h"



u16 g_buttons_1_state;
u16 g_buttons_1_prev;
u16 g_buttons_1_pressed;
u16 g_buttons_1_repeated;
bool l_buttons_1_repeat_start;
u16 l_buttons_1_repeat_cur;

std::vector<u8> g_font_paddings;

u8 l_max_string_size = 0x40;
u8 l_str_global_mode = 1;

std::vector<std::unique_ptr<OTag>>* g_menu_poly;
OTag* g_menu_otag;



void MenuUpdateButtons()
{
    u32 buttons = ButtonsGetCurrent();

    g_buttons_1_state = buttons;
    g_buttons_1_pressed = (g_buttons_1_state ^ g_buttons_1_prev) & g_buttons_1_state;
    g_buttons_1_repeated = 0;

    // repeat logic
    if( g_buttons_1_state == g_buttons_1_prev )
    {
        if( l_buttons_1_repeat_start == false )
        {
            if( l_buttons_1_repeat_cur != 0xf )
            {
                l_buttons_1_repeat_cur += 0x1;
            }
            else
            {
                l_buttons_1_repeat_start = true;
                l_buttons_1_repeat_cur = 0;
            }
        }
        else
        {
            if( l_buttons_1_repeat_cur != 3 )
            {
                l_buttons_1_repeat_cur += 0x1;
            }
            else
            {
                l_buttons_1_repeat_cur = 0;
                g_buttons_1_repeated = g_buttons_1_state;
            }
        }
    }
    else
    {
        l_buttons_1_repeat_start = false;
        l_buttons_1_repeat_cur = 0;
    }

    g_buttons_1_prev = g_buttons_1_state;
    g_buttons_1_repeated |= g_buttons_1_pressed;
}



void MenuLoadImage( std::vector<u8>::const_iterator ptr, s16 img_x, s16 img_y, s16 clut_x, s16 clut_y )
{
    PsyqOpenTim( ptr );

    while( true )
    {
        TIM_IMAGE tim;
        if( PsyqReadTim( &tim ) == 0 ) return;

        if( tim.caddr != 0 )
        {
            tim.crect.x = clut_x;
            tim.crect.y = clut_y;
            PsyqLoadImage( &tim.crect, (u8*)tim.caddr );
        }

        if( tim.paddr != 0 )
        {
            tim.prect.x = img_x;
            tim.prect.y = img_y;
            PsyqLoadImage( &tim.prect, (u8*)tim.paddr );
        }
    }
}



void MenuCreateDrawenvDispenv( DRAWENV* drawenv, DISPENV* dispenv )
{
    PsyqVSync( 0 );

    PsyqSetDefDrawEnv( drawenv, 0, 0, 0x180, 0x1d8 );
    drawenv->dfe = 0x1;
    drawenv->isbg = 0x1;
    PsyqPutDrawEnv( drawenv );

    PsyqVSync( 0 );

    PsyqSetDefDrawEnv( drawenv + 0x0, 0,  0x8, 0x180, 0xe0 );
    PsyqSetDefDrawEnv( drawenv + 0x1, 0, 0xf0, 0x180, 0xe0 );

    PsyqSetDefDispEnv( dispenv + 0x0, 0, 0xe8, 0x16c, 0xf0 );
    PsyqSetDefDispEnv( dispenv + 0x1, 0,    0, 0x16c, 0xf0 );

    (drawenv + 0x0)->dtd = 0x1;
    (drawenv + 0x0)->dfe = 0x1;
    (drawenv + 0x0)->isbg = 0x1;
    (drawenv + 0x0)->r0 = 0;
    (drawenv + 0x0)->g0 = 0;
    (drawenv + 0x0)->b0 = 0;
    (drawenv + 0x0)->tpage = 0x3f;

    (drawenv + 0x1)->dtd = 0x1;
    (drawenv + 0x1)->dfe = 0x1;
    (drawenv + 0x1)->isbg = 0x1;
    (drawenv + 0x1)->r0 = 0;
    (drawenv + 0x1)->g0 = 0;
    (drawenv + 0x1)->b0 = 0;
    (drawenv + 0x1)->tpage = 0x3f;

    PsyqVSync( 0 );

    PsyqPutDispEnv( dispenv );
    PsyqPutDrawEnv( drawenv );

//    system_psyq_set_disp_mask( 0x1 );
}



void MenuSetDrawMode( int dfe, int dtd, int tpage, SRECT* tw )
{
    auto poly = std::make_unique<DR_MODE>();
    PsyqSetDrawMode( poly.get(), dfe, dtd, tpage, tw );
    PsyqAddPrim( g_menu_otag, poly.get() );
    g_menu_poly->emplace_back(std::move(poly));
}



void MenuSetOTag( OTag* otag )
{
    g_menu_otag = otag;
}


void MenuSetPoly( std::vector<std::unique_ptr<OTag>>* poly )
{
    g_menu_poly = poly;
}



void MenuDrawCursor( s16 x, s16 y )
{
    auto poly = std::make_unique<SPRT>();
    PsyqSetSprt( poly.get() );
    PsyqSetSemiTrans( poly.get(), 0x1 );
    PsyqSetShadeTex( poly.get(), 0x1 );
    poly->x0 = x;
    poly->y0 = y;
    poly->u0 = 0xe0;
    poly->v0 = 0x8;
    poly->clut = PsyqGetClut( 0x100, 0x1e1 );
    poly->w = 0x18;
    poly->h = 0x10;
    PsyqAddPrim( g_menu_otag, poly.get() );
    g_menu_poly->emplace_back(std::move(poly));

    SRECT rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = 0xff;
    rect.h = 0xff;
    MenuSetDrawMode( 0, 0x1, PsyqGetTPage( 0, 0x2, 0x3c0, 0x100 ), &rect );
}



s16 MenuDrawSingleLetter( s16 x, s16 y, u8 color, u16 letter )
{
    u16 clut_x = 0;
    s16 tpage_x = 0;
    u16 set_start = 0;
    u16 tex_y = 0;

    switch( (letter >> 0x8) )
    {
        case 0xf8:
        {
            return x;
        }

        case 0xfa:
        {
            tex_y = 0x84;
            set_start = 0xe7;
        }
        break;

        case 0xfb:
        {
            clut_x = 0x10;
            tex_y = 0x0;
            set_start = 0x1b9;
        }
        break;

        case 0xfc:
        {
            clut_x = 0x10;
            tex_y = 0x84;
            set_start = 0x2a0;
        }
        break;

        case 0xfd:
        {
            tex_y = 0x84;
            set_start = 0x372;
            tpage_x = -0x40;
        }
        break;

        case 0xfe:
        {
            clut_x = 0x10;
            tex_y = 0x84;
            set_start = 0x444;
            tpage_x = -0x40;
        }
        break;

        default:
        {
            set_start = 0;
            tex_y = 0;
        }
    }

    u8 glyph = letter & 0xff;
    u8 letter_pad = g_font_paddings[glyph + set_start] >> 0x5;
    u8 letter_w = g_font_paddings[glyph + set_start] & 0x1f;

    x += letter_pad;
    u16 tex_x = (glyph % 0x15) * 0xc;
    tex_y += (glyph / 0x15) * 0xc;

    auto poly = std::make_unique<SPRT>();
    PsyqSetSprt( poly.get() );
    PsyqSetShadeTex( poly.get(), 0x1 );
    poly->x0 = x;
    poly->y0 = y;
    poly->u0 = tex_x;
    poly->v0 = tex_y;
    poly->clut = PsyqGetClut( 0x100 | clut_x, 0x1f0 + color );
    poly->w = 0xc;
    poly->h = 0xc;
    PsyqAddPrim( g_menu_otag, poly.get() );
    g_menu_poly->emplace_back(std::move(poly));

    x += letter_w;

    if( l_str_global_mode == 0 )
    {
        SRECT tw;
        tw.x = 0;
        tw.y = 0;
        tw.w = 0xff;
        tw.h = 0xff;
        auto poly = std::make_unique<DR_MODE>();
        PsyqSetDrawMode( poly.get(), 0, 0x1, (((0x380 + tpage_x) & 0x3ff) >> 0x6) | 0x30, &tw );
        PsyqAddPrim( g_menu_otag, poly.get() );
        g_menu_poly->emplace_back(std::move(poly));
    }

    return x;
}



void MenuDrawString( s16 x, s16 y, MenuStr& str, u8 color )
{
    u8* temp = str.str.data();
    if( str.str.size() == 0 ) return;

    s16 cur_x = x;

    for( int i = 0; i < l_max_string_size; ++i )
    {
        u16 letter = *temp;
        temp += 0x1;

        if( letter == 0xff ) break;

        if( (letter == 0xf8) || (letter == 0xfa) || (letter == 0xfb) || (letter == 0xfc) || (letter == 0xfd) || (letter == 0xfe) )
        {
            letter = (letter << 0x8) | *temp;
            temp += 0x1;
        }

        cur_x = MenuDrawSingleLetter( cur_x, y, color, letter );
    }

    if( l_str_global_mode != 0 )
    {
        SRECT tw;
        tw.x = 0;
        tw.y = 0;
        tw.w = 0xff;
        tw.h = 0xff;
        auto poly = std::make_unique<DR_MODE>();
        PsyqSetDrawMode( poly.get(), 0, 0x1, 0x3e, &tw );
        PsyqAddPrim( g_menu_otag, poly.get() );
        g_menu_poly->emplace_back(std::move(poly));
    }
}
