#include "akao.h"
#include "system.h"
#include "../field/field.h"
#include "../psyq/libgte.h"
#include "../psyq/libspu.h"



DISPENV global_dispenv;
DRAWENV global_drawenv;



void system_main()
{
    system_init_base();

    system_init_akao_engine();

    system_init_dispenv_drawenv();

    field_main();
}



void system_init_base()
{
    PsyqSpuInit();
    PsyqInitGeom();
}



void system_init_akao_engine()
{
    FILE* instr_all = fopen( "data/SOUND/INSTR.ALL", "rb" );
    if( instr_all == NULL )
    {
        ofLog( OF_LOG_ERROR, "File SOUND/INSTR.ALL not found." );
    }
    FILE* instr_dat = fopen( "data/SOUND/INSTR.DAT", "rb" );
    if( instr_dat == NULL )
    {
        ofLog( OF_LOG_ERROR, "File SOUND/INSTR.DAT not found." );
    }
    FILE* effect_all = fopen( "data/SOUND/EFFECT.ALL", "rb" );
    if( effect_all == NULL )
    {
        ofLog( OF_LOG_ERROR, "File SOUND/EFFECT.ALL not found." );
    }

    system_akao_init( instr_all, instr_dat );
    system_akao_load_effect_file( effect_all );

    fclose( instr_all );
    fclose( instr_dat );
    fclose( effect_all );
}



void system_init_dispenv_drawenv()
{
    PsyqSetDefDispEnv( &global_dispenv, 0x0, 0x0, 0x140, 0xe0 );
    PsyqSetDefDrawEnv( &global_drawenv, 0x0, 0x8, 0x140, 0xe0 );
    global_drawenv.dtd = 1;
    global_drawenv.isbg = 0;

    PsyqPutDispEnv( &global_dispenv );
    PsyqPutDrawEnv( &global_drawenv );
}