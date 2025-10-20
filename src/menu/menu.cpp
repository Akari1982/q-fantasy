#include "menu.h"
#include "psyq/tim.h"

std::vector<std::unique_ptr<OTag>>* g_menu_poly;
OTag* g_menu_otag;



void MenuLoadImage( std::vector<u8>::const_iterator ptr, s16 img_x, s16 img_y, s16 clut_x, s16 clut_y )
{
    PsyqGpuOpenTim( ptr );

    while( true )
    {
        TIM_IMAGE tim;
        if( PsyqGpuReadTim( &tim ) == 0 ) return;

        if( tim.caddr != 0 )
        {
            tim.crect.x = clut_x;
            tim.crect.y = clut_y;
            PsyqGpuLoadImage( &tim.crect, (u8*)tim.caddr );
        }

        if( tim.paddr != 0 )
        {
            tim.prect.x = img_x;
            tim.prect.y = img_y;
            PsyqGpuLoadImage( &tim.prect, (u8*)tim.paddr );
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
