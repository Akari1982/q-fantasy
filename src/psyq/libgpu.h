#pragma once

#include "typedef.h"



struct sColorAndCode
{
    u8 r, g, b;
    union {
        struct {
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
    union {
        struct {
            u8 r0, g0, b0;
            u8 code;
        };
        sColorAndCode m_colorAndCode;
    };
    DVECTOR x0y0, x1y1;

    void execute();
};

sTag* ClearOTagR( sTag* ot, int n );
void DrawOTag( sTag* ot );

void SetLineF2( LINE_F2* p );

void AddPrim( sTag* ot, sTag* p );
void TermPrim( sTag* );

template< typename T >
void SetSemiTrans( T* p, int abe )
{
    if( abe == 0 ) p->code &= ~2;
    else p->code |= 2;
}
