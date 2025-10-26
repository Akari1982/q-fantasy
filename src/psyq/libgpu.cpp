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
    int vram_offset = rect->y * VRAM_W + rect->x * 2;

    for( int y = 0; y < rect->h; ++y )
    {
        auto vramIterator = g_vram.begin() + vram_offset;
        for( int x = 0; x < (rect->w * 2); ++x )
        {
            *(vramIterator++) = *(data++);
            ++vram_offset;
        }
        vram_offset += VRAM_W - rect->w * 2;
    }

    GPUUpdateTextureFromVram();
}



void PsyqLoadImage( SRECT* rect, std::span<u8>::iterator data )
{
    for( int y = 0; y < rect->h; ++y )
    {
        auto vram = g_vram.begin() + (rect->y + y) * VRAM_W + rect->x * 2;

        for( int x = 0; x < (rect->w * 2); ++x )
        {
            *vram = READ_LE_U8( data );
            vram += 1;
            data += 1;
        }
    }

    GPUUpdateTextureFromVram();
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



void PsyqClearImage( SRECT* rect, u8 r, u8 g, u8 b )
{
    for( int y = 0; y < rect->h; ++y )
    {
        auto vram = g_vram.begin() + (rect->y + y) * VRAM_W + rect->x * 2;

        for( int x = 0; x < rect->w * 2; ++x )
        {
            *vram = 0;
            *vram |= ((g >> 0x3) >> 0x3) & 0x03;
            *vram |= ((b >> 0x3) << 0x2) & 0x7c;
            vram += 1;
            *vram = 0;
            *vram |= (r >> 0x3) & 0x1f;
            *vram |= ((g >> 0x3) << 0x5) & 0xe0;
        }
    }

    GPUUpdateTextureFromVram();
}



s32 PsyqVSync( s32 mode )
{
    GPUUpdateScreenFromVram();

    GameRender();

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
    env->ofs[0] = x;
    env->ofs[1] = y;
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
    dr_env->type = GPU_DR_ENV;
    dr_env->env = *env;
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
    DR_ENV dr_env;
    PsyqSetDrawEnv( &dr_env, env );
    dr_env.execute();

    return env;
}



void PsyqSetDrawMode( DR_MODE* p, int dfe, int dtd, int tpage, SRECT* tw )
{
    p->type = GPU_DR_MODE;
    p->dfe = dfe;
    p->dtd = dtd;
    p->tpage = tpage;
    if( tw != nullptr ) p->tw = *tw;
}



OTag* PsyqClearOTagR( OTag* ot, s32 n )
{
    OTag* current = ot;
    for( int i = 0; i < n - 1; ++i )
    {
        ++current;
        current->next = current - 1;
        current->type = GPU_OTAG;
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
        (current - 1)->type = GPU_OTAG;
    }

    PsyqTermPrim( ot );

    return ot;
}



void PsyqDrawOTag( OTag* ot )
{
    GPUUpdateRenderFromVram();

    while( ot )
    {
        ot->execute();
        ot = ot->next;
    }

    GPUUpdateVramFromRender();
}



void PsyqSetLineF2( LINE_F2* p )
{
    p->type = GPU_LINE_F2;
    p->code = 0x40;
}



void PsyqSetPolyFT4( POLY_FT4* p )
{
    p->type = GPU_POLY_FT4;
    p->code = 0x2c;
}




void PsyqSetSprt( SPRT* p )
{
    p->type = GPU_SPRT;
    p->code = 0x64;
}



void PsyqSetSprt16( SPRT_16* p )
{
    p->type = GPU_SPRT_16;
    p->code = 0x7c;
}


void PsyqAddPrim( OTag* ot, OTag* p )
{
    p->next = ot->next;
    ot->next = p;
}



void PsyqTermPrim( OTag* p )
{
    p->next = nullptr;
    p->type = GPU_TERMINATE;
}



u16 PsyqGetClut( s32 x, s32 y )
{
    return (y << 0x6) | ((x >> 0x4) & 0x3f);
}



u16 PsyqGetTPage( int tp, int abr, int x, int y )
{
    return ((y & 0x200) << 0x2) | ((tp & 0x3) << 0x7) | ((abr & 0x3) << 0x5) | ((y & 0x100) >> 0x4) | ((x & 0x3ff) >> 0x6);
}
