#include "libgpu.h"
#include "psxgpu.h"



std::vector<u8>::const_iterator l_current_tim;



int PsyqOpenTim( std::vector<u8>::const_iterator ptr )
{
    l_current_tim = ptr;
    return 0;
}



int GetTimData( std::vector<u8>::const_iterator input, TIM_IMAGE* timimg )
{
    if( READ_LE_U32( input ) == 0x10 )
    {
        std::vector<u8>::const_iterator timaddr = input + 0x8;
        timimg->mode = READ_LE_U32( input + 0x4 );

        int clut_len = 0;

        if( timimg->mode & 0x8 )
        {
            timimg->crect.x = READ_LE_S16( input + 0xc + 0x0 );
            timimg->crect.y = READ_LE_S16( input + 0xc + 0x2 );
            timimg->crect.w = READ_LE_S16( input + 0xc + 0x4 );
            timimg->crect.h = READ_LE_S16( input + 0xc + 0x6 );
            timimg->caddr = &(*(input + 0x14));
            clut_len = READ_LE_U32( timaddr ) / 4;
            timaddr += READ_LE_U32( timaddr ) & ~0x3;
        }
        else
        {
            timimg->caddr = nullptr;
        }

        timimg->prect.x = READ_LE_S16( timaddr + 0x4 + 0x0 );
        timimg->prect.y = READ_LE_S16( timaddr + 0x4 + 0x2 );
        timimg->prect.w = READ_LE_S16( timaddr + 0x4 + 0x4 );
        timimg->prect.h = READ_LE_S16( timaddr + 0x4 + 0x6 );
        timimg->paddr = &(*(timaddr + 0xc));
        return clut_len + (READ_LE_U32( timaddr ) / 4) + 0x2;
    }
    else
    {
        return -1;
    }
}



TIM_IMAGE* PsyqReadTim( TIM_IMAGE* timimg )
{
    int tim_offset = GetTimData( l_current_tim, timimg );

    if( tim_offset != -1 )
    {
        l_current_tim += tim_offset * 4;
        return timimg;
    }

    return nullptr;
}



void PsyqLoadImage( SRECT* rect, const u8* data )
{
    int vram_offset = rect->y * 2048 + rect->x * 2;

    for( int y = 0; y < rect->h; ++y )
    {
        auto vramIterator = g_vram.begin() + vram_offset;
        for( int x = 0; x < (rect->w * 2); ++x )
        {
            *(vramIterator++) = *(data++);
            ++vram_offset;
        }
        vram_offset += 2048 - rect->w * 2;
    }
}



void PsyqLoadImage( SRECT* rect, std::span<u8>::iterator data )
{
    for( int y = 0; y < rect->h; ++y )
    {
        auto vram = g_vram.begin() + (rect->y + y) * 2048 + rect->x * 2;

        for( int x = 0; x < (rect->w * 2); ++x )
        {
            *vram = READ_LE_U8( data );
            vram += 1;
            data += 1;
        }
    }
}



u16 PsyqLoadTPage( const u8* data, int tp, int abr, int x, int y, int w, int h )
{
    SRECT rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    if( tp == 0 ) rect.w = w / 0x4;
    else if( tp == 0x1 ) rect.w = w / 0x2;

    PsyqLoadImage( &rect, data );

    return PsyqGetTPage( tp, abr, x, y );
}



s32 PsyqVSync( s32 mode )
{
    GameRender();

    GPUUpdateVram();

    return 1;
}



DISPENV* PsyqSetDefDispEnv( DISPENV* env, s32 x, s32 y, s32 w, s32 h )
{
    env->disp.x = x;
    env->disp.y = y;
    env->disp.w = w;
    env->disp.h = h;
    env->screen.x = 0;
    env->screen.y = 0;
    env->screen.w = 0;
    env->screen.h = 0;
    env->isrgb24 = 0;
    env->isinter = 0;
    env->pad1 = 0;
    env->pad0 = 0;
    return env;
}



DRAWENV* PsyqSetDefDrawEnv( DRAWENV* env, s32 x, s32 y, s32 w, s32 h )
{
    env->clip.x = x;
    env->clip.y = y;
    env->clip.w = w;
    env->clip.h = h;
    env->ofs[ 0 ] = x;
    env->ofs[ 1 ] = y;
    env->tw.x = 0;
    env->tw.y = 0;
    env->tw.w = 0;
    env->tw.h = 0;
    env->tpage = 0xa;
    env->dtd = 1;
    env->dfe = 0;
    env->isbg = 0;
    env->r0 = 0;
    env->g0 = 0;
    env->b0 = 0;
    return env;
}



void PsyqSetDrawEnv( DR_ENV* dr_env, DRAWENV* env )
{
/*
    A0 = h[env + 0]; // x top clip
    A1 = h[env + 2]; // y top clip
    system_gpu_set_drawing_area_top_left(); // create packet for clip
    [dr_env + 4] = w(V0);

    A0 = h[env + 0] + h[env + 4] - 1;
    A1 = h[env + 2] + h[env + 6] - 1;
    system_gpu_set_drawing_area_bottom_right(); // create packet for сlip
    [dr_env + 8] = w(V0);

    A0 = h[env + 8]; // offset x
    A1 = h[env + a]; // offset y
    system_gpu_set_drawing_offset(); // create packet for offset
    [dr_env + c] = w(V0);

    A0 = bu[env + 17]; // 0: drawing to display area is blocked, 1: drawing to display area is permitted
    A1 = bu[env + 16]; // dithering processing flag. 0: off; 1: on
    A2 = hu[env + 14]; // initial values of texture page
    system_gpu_get_draw_mode_setting_command(); // create packet
    [dr_env + 10] = w(V0);

    A0 = env + c; // texture window rect
    system_gpu_get_texture_window_setting_command(); // create packet
    [dr_env + 14] = w(V0);

    [dr_env + 18] = w(e6000000);

    [dr_env + 3] = b(6); // number of 0x4 packets to gpu

    if( bu[env + 18] != 0 )
    {
        rect_x = hu[env + 0];
        rect_y = hu[env + 2];
        rect_w = hu[env + 4];
        rect_h = hu[env + 6];

        m_width = h[80062с04] - 1;
        m_height = h[80062c06] - 1;

        rect_w = ( rect_w >= 0 ) ? ( ( m_width < rect_w ) ? m_width : rect_w ) : 0;
        rect_h = ( rect_h >= 0 ) ? ( ( m_height < rect_h ) ? m_height : rect_h ) : 0;

        if( ( rect_x & 3f ) || ( rect_w & 3f ) )
        {
            rect_x = rect_x - hu[env + 8];
            rect_y = rect_y - hu[env + a];

            [dr_env + 7 * 4] = w(60000000 | (bu[env + 1b] << 10) | (bu[env + 1a] << 8) | bu[env + 19]);
            [dr_env + 8 * 4] = w((rect_y << 10) | rect_x);
            [dr_env + 9 * 4] = w((rect_h << 10) | rect_w);
        }
        else
        {
            [dr_env + 7 * 4] = w(02000000 | (bu[env + 1b] << 10) | (bu[env + 1a] << 08); | bu[env + 19]);
            [dr_env + 8 * 4] = w((rect_y << 10) | rect_x);
            [dr_env + 9 * 4] = w((rect_h << 10) | rect_w);
        }
        [dr_env + 3] = b(9);
    }
*/
}



DISPENV* PsyqPutDispEnv( DISPENV* env )
{
    if( (g_screen.getWidth() != env->disp.w) || (g_screen.getHeight() != env->disp.h) )
    {
        g_screen.allocate( env->disp.w, env->disp.h, GL_RGBA8, false );
    }

    g_rendering_disp_x = env->disp.x;
    g_rendering_disp_y = env->disp.y;

    return env;
}



DRAWENV* PsyqPutDrawEnv( DRAWENV* env )
{
    if( (g_render.getWidth() != env->clip.w) || (g_render.getHeight() != env->clip.h) )
    {
        g_render.allocate( env->clip.w, env->clip.h, GL_RGBA );
    }

    g_rendering_draw_x = env->clip.x;
    g_rendering_draw_y = env->clip.y;

    g_render.begin();
    ofClear( env->r0, env->g0, env->b0, 255 );
    g_render.end();

    return env;
}



u32 get_mode( int dfe, int dtd, int tpage )
{
    u32 add_dtd = 0;
    u32 add_dfe = 0;
    if( dtd != 0 ) add_dtd = 0x200; // Dither 24bit to 15bit Dither Enabled
    if( dfe != 0 ) add_dfe = 0x400; // Drawing to display area Allowed
    return 0xe1000000 | add_dtd | add_dfe | (tpage & 0x9ff);
}



u32 get_tw( SRECT* tw )
{
    if( tw == nullptr ) return 0;
    u32 off_x = tw->x >> 0x3;
    u32 off_y = tw->y >> 0x3;
    u32 mask_x = ((0 - tw->w) & 0xff) >> 0x3;
    u32 mask_y = ((0 - tw->h) & 0xff) >> 0x3;
    return 0xe2000000 | (off_y << 0xf) | (off_x << 0xa) | (mask_y << 0x5) | mask_x;
}



void PsyqSetDrawMode( DR_MODE* p, int dfe, int dtd, int tpage, SRECT* tw )
{
    p->size = 0x2;
    p->code[0] = get_mode( dfe, dtd, tpage );
    p->code[1] = get_tw( tw );
}



OTag* PsyqClearOTagR( OTag* ot, s32 n )
{
    OTag* current = ot;
    for( int i = 0; i < n - 1; ++i )
    {
        ++current;
        current->next = current - 1;
        current->size = 0;
    }

    PsyqTermPrim( ot );

    return ot;
}



OTag* PsyqClearOTag( OTag* ot, s32 n )
{
    OTag* current = ot;
    for( int i = n - 1; i != 0; --i )
    {
        ++current;
        (current - 1)->next = current;
        (current - 1)->size = 0;
    }

    PsyqTermPrim( ot );

    return ot;
}



void PsyqDrawOTag( OTag* ot )
{
    while( ot )
    {
        ot->execute();
        ot = ot->next;
    }
}



void PsyqSetLineF2( LINE_F2* p )
{
    p->size = 0x3;
    p->code = 0x40;
}



void PsyqSetPolyFT4( POLY_FT4* p )
{
    p->size = 0x9;
    p->code = 0x2c;
}




void PsyqSetSprt( SPRT* p )
{
    p->size = 0x4;
    p->code = 0x64;
}



void PsyqAddPrim( OTag* ot, OTag* p )
{
    p->next = ot->next;
    ot->next = p;
}



void PsyqTermPrim( OTag* p )
{
    p->next = nullptr;
    p->size = 0;
}



u16 PsyqGetClut( s32 x, s32 y )
{
    return (y << 0x6) | ((x >> 0x4) & 0x3f);
}



u16 PsyqGetTPage( int tp, int abr, int x, int y )
{
    return ((y & 0x200) << 0x2) | ((tp & 0x3) << 0x7) | ((abr & 0x3) << 0x5) | ((y & 0x100) >> 0x4) | ((x & 0x3ff) >> 0x6);
}
