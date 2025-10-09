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

struct OTag
{
    OTag* next = nullptr;
    u8 size = 0;

    void execute();
};

struct LINE_F2 : public OTag
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

struct POLY_FT4 : public OTag
{
    u8 r0, g0, b0;
    u8 code;
    s16 x0, y0;
    u8 u0, v0;
    u16 clut;
    s16 x1, y1;
    u8 u1, v1;
    u16 tpage;
    s16 x2, y2;
    u8 u2, v2;
    u16 pad1;
    s16 x3, y3;
    u8 u3, v3;
    u16 pad2;

    void execute();
};

struct DR_ENV : public OTag
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

OTag* PsyqClearOTagR( OTag* ot, s32 n );
OTag* PsyqClearOTag( OTag* ot, s32 n );
void PsyqDrawOTag( OTag* ot );

void PsyqSetLineF2( LINE_F2* p );
void PsyqSetPolyFT4( POLY_FT4* p );

void PsyqAddPrim( OTag* ot, OTag* p );
void PsyqTermPrim( OTag* );

u16 PsyqGetClut( s32 x, s32 y );
u16 PsyqGetTPage( int tp, int abr, int x, int y );

template< typename T >
void PsyqSetSemiTrans( T* p, s32 abe )
{
    if( abe == 0 ) p->code &= ~2;
    else p->code |= 2;
}



extern std::array<u8, 2048 * 512> g_vram;
