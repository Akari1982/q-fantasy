#include "akao.h"
#include "system.h"
#include "ending/ending.h"
#include "field/field.h"
#include "psyq/filesystem.h"
#include "psyq/libgte.h"
#include "psyq/libspu.h"



DISPENV global_dispenv;
DRAWENV global_drawenv;



void system_main()
{
    system_init_base();

    system_init_akao_engine();

    system_init_dispenv_drawenv();

    ending_main();
    field_main();

    AkaoQuit();
    PsyqSpuQuit();
}



void system_init_base()
{
    PsyqSpuInit();
    PsyqInitGeom();
}



void system_init_akao_engine()
{
    std::vector<u8> instr_all;
    FileRead( "SOUND/INSTR.ALL", instr_all );

    std::vector<u8> instr_dat;
    FileRead( "SOUND/INSTR.DAT", instr_dat );

    std::vector<u8> effect_all;
    FileRead( "SOUND/EFFECT.ALL", effect_all );

    AkaoInit( &instr_all[0], &instr_dat[0] );
    AkaoLoadEffect( &effect_all[0] );
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