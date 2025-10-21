#pragma once

#include "psyq/libgpu.h"

#include <vector>
#include <memory>



void MenuLoadImage( std::vector<u8>::const_iterator ptr, s16 img_x, s16 img_y, s16 clut_x, s16 clut_y );
void MenuCreateDrawenvDispenv( DRAWENV* drawenv, DISPENV* dispenv );
void MenuSetDrawMode( int dfe, int dtd, int tpage, SRECT* tw );

void MenuSetOTag( OTag* otag );
void MenuSetPoly( std::vector<std::unique_ptr<OTag>>* poly );

void MenuDrawCursor( s16 x, s16 y );



extern std::vector<std::unique_ptr<OTag>>* g_menu_poly;
extern OTag* g_menu_otag;
