#include "akao.h"
#include "../psyq/libspu.h"

AkaoInstrument g_akao_instrument[0x80];



void system_akao_init( FILE* instr_all, FILE* instr_dat )
{
/*
    [GP + 0x230] = w(0x80089580);
    [GP + 0x240] = w(0x80089580 + 0x1000);

    system_psyq_spu_init_malloc( 0x4, 0x8007e778 );
    system_psyq_spu_malloc_with_start_addr( 0x77000, 0x2000 );

*/

    system_akao_load_instr_files( instr_all, instr_dat );

    //PsyqSpuSetTransferStartAddr( 0x76fe0 );
    //PsyqSpuWrite( 0x8004a60c, 0x20 );

/*
    system_akao_spu_transfer_sync();

    system_akao_init_data();

    do
    {
        V0 = system_bios_open_event( RCntCNT2, EvSpINT, EvMdINTR, 0x80030234 ); // system_akao_main() execute function/stay busy
        [GP + 0xbc] = w(V0);
    } while( V0 == -1 )

    while( system_bios_enable_event( w[GP + 0xbc] ) == 0 ) {}

    while( system_psyq_set_r_cnt( RCntCNT2, 0x43d1, EvMdINTR ) == 0 ) {}

    while( system_psyq_start_r_cnt( RCntCNT2 ) == 0 ) {}
*/
}



void system_akao_load_effect_file( FILE* effect_all )
{
/*
    dst = w[GP + 0x230];
    for( int i = 0x3200; i != 0; --i )
    {
        [dst] = w(w[effect_all]);
        effect_all += 0x4;
        dst += 0x4;
    }

    FILE* check = fopen( "check.dat", "ab" );
    fwrite( g_akao_instrument, 1, 0x800 * 0x4, check);
    fclose( check );
*/
}



void system_akao_load_instr_files( FILE* instr_all, FILE* instr_dat )
{
/*
    system_psyq_spu_set_transfer_start_addr( w[instr_all + 0x0] );
    system_akao_spu_write( instr_all + 0x10, w[instr_all + 0x4] );
*/

    fseek( instr_dat, 0, SEEK_SET );
    fread( g_akao_instrument, 1, 0x800 * 0x4, instr_dat );

/*
    system_akao_spu_transfer_sync();
*/
}
