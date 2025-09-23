#include "akao.h"
#include "system.h"
#include "ending/ending.h"
#include "field/field.h"
#include "psyq/filesystem.h"
#include "psyq/libgte.h"
#include "psyq/libspu.h"



DISPENV g_global_dispenv;
DRAWENV g_global_drawenv;



void SystemMain()
{
    SystemInitBase();

    SystemInitAkaoEngine();

    SystemInitDispEnvDrawEnv();

    EndingMain();
    FieldMain();
}



void SystemInitBase()
{
    PsyqSpuInit();
    atexit( PsyqSpuQuit );

    PsyqInitGeom();
}



void SystemInitAkaoEngine()
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



void SystemInitDispEnvDrawEnv()
{
    PsyqSetDefDispEnv( &g_global_dispenv, 0x0, 0x0, 0x140, 0xe0 );
    PsyqSetDefDrawEnv( &g_global_drawenv, 0x0, 0x8, 0x140, 0xe0 );
    g_global_drawenv.dtd = 1;
    g_global_drawenv.isbg = 0;

    PsyqPutDispEnv( &g_global_dispenv );
    PsyqPutDrawEnv( &g_global_drawenv );
}