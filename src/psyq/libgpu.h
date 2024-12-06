#pragma once

#include "typedef.h"



struct DISPENV
{
    RECT disp;
    RECT screen;
    u8 isinter;
    u8 isrgb24;
    u8 pad0;
    u8 pad1;
};

struct DRAWENV
{
    RECT clip;
    s16 ofs[ 2 ];
    RECT tw;
    u16 tpage;
    u8 dtd;
    u8 dfe;
    u8 isbg;
    u8 r0;
    u8 g0;
    u8 b0;
    DR_ENV dr_env;
};

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



DISPENV* SetDefDispEnv( DISPENV* env, s32 x, s32 y, s32 w, s32 h );
DRAWENV* SetDefDrawEnv( DRAWENV* env, s32 x, s32 y, s32 w, s32 h );
void SetDrawEnv( DR_ENV *dr_env, DRAWENV *env );
DISPENV* PutDispEnv( DISPENV* env );
DRAWENV* PutDrawEnv( DRAWENV* env );

sTag* ClearOTagR( sTag* ot, s32 n );
void DrawOTag( sTag* ot );

void SetLineF2( LINE_F2* p );

void AddPrim( sTag* ot, sTag* p );
void TermPrim( sTag* );

template< typename T >
void SetSemiTrans( T* p, s32 abe )
{
    if( abe == 0 ) p->code &= ~2;
    else p->code |= 2;
}
