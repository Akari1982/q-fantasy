#include "libgpu.h"
#include "../game.h"



void sTag::execute()
{
    sColorAndCode colorAndCode = *(sColorAndCode*)(((u8*)this) + sizeof(sTag));

    u8 code = colorAndCode.code;
    if( size == 0 )
    {
    }
    else if( ( code & 0xe0 ) == 0x20 ) //poly
    {
        switch( code & 0x3c )
        {
            //case 0x20: ((POLY_F3*)this)->execute(); break;
            //case 0x24: ((POLY_FT3*)this)->execute(); break;
            //case 0x28: ((POLY_F4*)this)->execute(); break;
            //case 0x2C: ((POLY_FT4*)this)->execute(); break;
            //case 0x30: ((POLY_G3*)this)->execute(); break;
            //case 0x34: ((POLY_GT3*)this)->execute(); break;
            //case 0x38: ((POLY_G4*)this)->execute(); break;
            default: break;
        }
    }
    else if( ( code & 0xe0 ) == 0x40 ) // line
    {
        switch( code & 0x4c )
        {
            case 0x40: ((LINE_F2*)this)->execute(); break;
            //case 0x42: break;
            //case 0x48: ((LINE_F3*)this)->execute(); break;
            default: break;
        }
    }
    else if( ( code & 0xe0 ) == 0x60 ) // rect
    {
        switch( code & 0x7c )
        {
            //case 0x60: ((TILE*)this)->execute(); break;
            //case 0x64: ((SPRT*)this)->execute(); break;
            //case 0x74:
            //case 0x7c: ((SPRT_8*)this)->execute(); break;
            default: break;
        }
    }
    else
    {
        assert( code == 0xe1 );
        if( size == 1 )
        {
            //((DR_TPAGE*)this)->execute();
        }
        else
        {
            assert( size == 2 );
            //((DR_MODE*)this)->execute();
        }
    }
}



void
LINE_F2::execute()
{
    g_GameVram.begin();
    ofSetColor( 255,255,255, 144 );
    ofDrawLine( glm::vec3( x0y0.vx, x0y0.vy, 0 ), glm::vec3( x1y1.vx, x1y1.vy, 0 ) );
    g_GameVram.end();
}



sTag* ClearOTagR( sTag* ot, int n )
{
    sTag* current = ot;
    for( int i = 0; i < n - 1; ++i )
    {
        ++current;
        current->next = current - 1;
        current->size = 0;
    }

    TermPrim( ot );

    return ot;
}



void DrawOTag( sTag* ot )
{
    while( ot )
    {
        ot->execute();
        ot = ot->next;
    }
}



void SetLineF2(LINE_F2* p)
{
    p->size = 3;
    p->code = 0x40;
}



void AddPrim( sTag* ot, sTag* p )
{
    p->next = ot->next;
    ot->next = p;
}



void TermPrim( sTag* p )
{
    p->next = nullptr;
    p->size = 0;
}
