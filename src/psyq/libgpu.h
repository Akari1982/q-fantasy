#pragma once

#include "typedef.h"

#include <array>
#include <span>



#define GPU_OTAG        0x00
#define GPU_TERMINATE   0x01
#define GPU_DR_MODE     0x02
#define GPU_DR_ENV      0x03
#define GPU_POLY_FT4    0x2c
#define GPU_LINE_F2     0x40
#define GPU_SPRT        0x64
#define GPU_SPRT_16     0x7c



struct TIM_IMAGE
{
    u32 mode;
    SRECT crect;
    const void* caddr;
    SRECT prect;
    const void* paddr;
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
    s16 ofs[2];
    SRECT tw;
    u16 tpage;
    u8 dtd;
    u8 dfe;
    u8 isbg;
    u8 r0;
    u8 g0;
    u8 b0;
};

struct OTag
{
    OTag* next = nullptr;
    u8 type = GPU_OTAG;

    void execute();
};

struct LINE_F2 : public OTag
{
    u8 r0, g0, b0;
    u8 code;
    s16 x0, y0;
    s16 x1, y1;

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

struct SPRT : public OTag
{
    u8 r0, g0, b0;
    u8 code;
    s16 x0, y0;
    u8 u0, v0;
    u16 clut;
    s16 w, h;

    void execute();
};

struct SPRT_16 : public OTag
{
    u8 r0, g0, b0;
    u8 code;
    s16 x0, y0;
    u8 u0, v0;
    u16 clut;

    void execute();
};

struct DR_MODE : public OTag
{
    u8 dfe;
    u8 dtd;
    u16 tpage;
    SRECT tw;

    void execute();
};

struct DR_ENV : public OTag
{
    DRAWENV env;

    void execute();
};



int PsyqOpenTim( std::vector<u8>::const_iterator ptr );
TIM_IMAGE* PsyqReadTim( TIM_IMAGE* timimg );

void PsyqLoadImage( SRECT* rect, const u8* data );
void PsyqLoadImage( SRECT* rect, std::span<u8>::iterator data );
u16 PsyqLoadTPage( const u8* data, int tp, int abr, int x, int y, int w, int h );
void PsyqClearImage( SRECT* rect, u8 r, u8 g, u8 b );

s32 PsyqVSync( s32 mode );

DISPENV* PsyqSetDefDispEnv( DISPENV* env, s32 x, s32 y, s32 w, s32 h );
DRAWENV* PsyqSetDefDrawEnv( DRAWENV* env, s32 x, s32 y, s32 w, s32 h );
void PsyqSetDrawEnv( DR_ENV* dr_env, DRAWENV* env );
DISPENV* PsyqPutDispEnv( DISPENV* env );
DRAWENV* PsyqPutDrawEnv( DRAWENV* env );
void PsyqSetDrawMode( DR_MODE* p, int dfe, int dtd, int tpage, SRECT* tw );
void PsyqSetDispMask( int mask );

OTag* PsyqClearOTagR( OTag* ot, s32 n );
OTag* PsyqClearOTag( OTag* ot, s32 n );
void PsyqDrawOTag( OTag* ot );

void PsyqSetLineF2( LINE_F2* p );
void PsyqSetPolyFT4( POLY_FT4* p );
void PsyqSetSprt( SPRT* p );
void PsyqSetSprt16( SPRT_16* p );

void PsyqAddPrim( OTag* ot, OTag* p );
void PsyqTermPrim( OTag* );

u16 PsyqGetClut( s32 x, s32 y );
u16 PsyqGetTPage( int tp, int abr, int x, int y );

template< typename T >
void PsyqSetSemiTrans( T* p, s32 abe )
{
    if( abe == 0 ) p->code &= ~0x2;
    else p->code |= 0x2;
}

template< typename T >
void PsyqSetShadeTex( T* p, s32 tge )
{
    if( tge == 0 ) p->code &= ~0x1;
    else p->code |= 0x1;
}
