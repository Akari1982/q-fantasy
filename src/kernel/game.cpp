#include "akao.h"
#include "file.h"
#include "game.h"
#include "ending/ending.h"
#include "menu/menu.h"
#include "menu/newgame.h"
#include "field/field.h"
#include "psyq/libgte.h"
#include "psyq/libspu.h"



void GameInitBase();
void GameInitKernel();
void GameInitAkaoEngine();



void GameMain()
{
    GameInitBase();
    GameInitAkaoEngine();
    FieldInitEnv();

    EndingMainLogo();

    //GameInitKernel();

    //MenuNewGameMain();

    FieldMain();
}



void GameInitBase()
{
    PsyqSpuInit();
    atexit( PsyqSpuQuit );

    PsyqInitGeom();
}



void GameInitAkaoEngine()
{
    std::vector<u8> instr_all;
    FileRead( "SOUND/INSTR.ALL", instr_all );

    std::vector<u8> instr_dat;
    FileRead( "SOUND/INSTR.DAT", instr_dat );

    std::vector<u8> effect_all;
    FileRead( "SOUND/EFFECT.ALL", effect_all );

    AkaoInit( &instr_all[0], &instr_dat[0] );
    atexit( AkaoQuit );

    AkaoLoadEffect( &effect_all[0] );
}



void GameInitKernel()
{
    std::vector<u8> window_bin;
    FileRead( "INIT/WINDOW.BIN", window_bin );

    GZIPSetDataBlock( window_bin );

    while( true )
    {
        u16 type = GZIPGetType();

        if( type == 0xffff ) break;

        if( type == 0 ) // image
        {
            std::vector<u8> temp;
            GZIPPackDecompressNextBlock( temp );

            SRECT rect;
            u32 offset = 0;
            if( READ_LE_U32( &temp[0x4] ) & 0x8 )
            {
                offset = (READ_LE_U32( &temp[0x8] ) >> 0x2) << 0x2;

                rect.x = READ_LE_S16( &temp[0xc] );
                rect.y = READ_LE_S16( &temp[0xe] );
                rect.w = READ_LE_S16( &temp[0x10] );
                rect.h = READ_LE_S16( &temp[0x12] );
                PsyqLoadImage( &rect, &temp[0x14] );
            }

            rect.x = READ_LE_S16( &temp[offset + 0xc] );
            rect.y = READ_LE_S16( &temp[offset + 0xe] );
            rect.w = READ_LE_S16( &temp[offset + 0x10] );
            rect.h = READ_LE_S16( &temp[offset + 0x12] );
            PsyqLoadImage( &rect, &temp[offset + 0x14] );
        }
        else if( type == 0x1 )
        {
            GZIPPackDecompressNextBlock( g_font_paddings );
        }
    }
}
