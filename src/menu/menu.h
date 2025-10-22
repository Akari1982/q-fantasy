#pragma once

#include "psyq/libgpu.h"

#include <string>
#include <vector>
#include <memory>



struct MenuStr
{
    MenuStr( const std::string& text )
    {
        for( char c : text )
        {
            str.push_back(static_cast<u8>(c - 0x20));
        }
        str.push_back(0xff);
    }
    std::vector<u8> str;
};



void MenuLoadImage( std::vector<u8>::const_iterator ptr, s16 img_x, s16 img_y, s16 clut_x, s16 clut_y );
void MenuCreateDrawenvDispenv( DRAWENV* drawenv, DISPENV* dispenv );
void MenuSetDrawMode( int dfe, int dtd, int tpage, SRECT* tw );

void MenuSetOTag( OTag* otag );
void MenuSetPoly( std::vector<std::unique_ptr<OTag>>* poly );

void MenuDrawString( s16 x, s16 y, MenuStr& str, u8 color );
void MenuDrawCursor( s16 x, s16 y );



extern std::vector<std::unique_ptr<OTag>>* g_menu_poly;
extern OTag* g_menu_otag;
