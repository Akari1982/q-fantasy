#pragma once

#include "typedef.h"

#include <array>



struct sColorAndCode
{
    u8 r, g, b;
    union
    {
        struct
        {
            u8 textureBlending : 1;
            u8 semiTransparent : 1;
            u8 textured : 1;
            u8 quad : 1;
            u8 gouraud : 1;
            u8 command : 3;
        };
        u8 code;
    };
};

struct sTag
{
    sTag* next = nullptr;
    u8 size = 0;

    void execute();
};

struct LINE_F2 : public sTag
{
    union
    {
        struct
        {
            u8 r0, g0, b0;
            u8 code;
        };
        sColorAndCode m_colorAndCode;
    };
    DVECTOR x0y0, x1y1;

    void execute();
};

struct DR_ENV : public sTag
{
    u32 code[ 15 ];

    void execute();
};

struct DISPENV
{
    SRECT disp;
    SRECT screen;
    u8 isinter;
    u8 isrgb24;
    u8 pad0;
    u8 pad1;
};

struct DRAWENV
{
    SRECT clip;
    s16 ofs[ 2 ];
    SRECT tw;
    u16 tpage;
    u8 dtd;
    u8 dfe;
    u8 isbg;
    u8 r0;
    u8 g0;
    u8 b0;
    DR_ENV dr_env;
};



s32 PsyqVSync( s32 mode );

DISPENV* PsyqSetDefDispEnv( DISPENV* env, s32 x, s32 y, s32 w, s32 h );
DRAWENV* PsyqSetDefDrawEnv( DRAWENV* env, s32 x, s32 y, s32 w, s32 h );
void PsyqSetDrawEnv( DR_ENV *dr_env, DRAWENV *env );
DISPENV* PsyqPutDispEnv( DISPENV* env );
DRAWENV* PsyqPutDrawEnv( DRAWENV* env );

sTag* PsyqClearOTagR( sTag* ot, s32 n );
void PsyqDrawOTag( sTag* ot );

void PsyqSetLineF2( LINE_F2* p );

void PsyqAddPrim( sTag* ot, sTag* p );
void PsyqTermPrim( sTag* );

template< typename T >
void PsyqSetSemiTrans( T* p, s32 abe )
{
    if( abe == 0 ) p->code &= ~2;
    else p->code |= 2;
}



extern std::array<u8, 2048 * 512> g_vram;
