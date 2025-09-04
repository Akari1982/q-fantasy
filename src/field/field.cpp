#include "field.h"
#include "system/game.h"
#include "kernel/system.h"
#include "kernel/akao.h"
#include "psyq/filesystem.h"
#include "psyq/libgte.h"

#include <vector>


u32 field_dat_base_addr = 0x80114fe4;
std::vector<u8> field_dat;

struct sBaseDrawOffset
{
    u32 x;
    u32 y;
};
sBaseDrawOffset base_draw_offset;

DRAWENV field_draw_env;

struct sWalkMesh
{
    SVECTOR p1;
    SVECTOR p2;
    SVECTOR p3;
};
std::vector< sWalkMesh > field_walkmesh;

struct sWalkMeshAccess
{
    u16 p1;
    u16 p2;
    u16 p3;
};
std::vector< sWalkMeshAccess > field_walkmesh_access;


std::array< sFieldRain, 0x40 > field_rain;
std::array< sFieldRainPrim, 2 > field_rain_prim;

std::vector<u8> field_random =
{
    0xb1, 0xca, 0xee, 0x6c, 0x5a, 0x71, 0x2e, 0x55, 0xd6, 0x00, 0xcc, 0x99, 0x90, 0x6b, 0x7d, 0xeb,
    0x4f, 0xa0, 0x07, 0xac, 0xdf, 0x8a, 0x56, 0x9e, 0xf1, 0x9a, 0x63, 0x75, 0x11, 0x91, 0xa3, 0xb8,
    0x94, 0x73, 0xf7, 0x54, 0xd9, 0x6e, 0x72, 0xc0, 0xf4, 0x80, 0xde, 0xb9, 0xbb, 0x8d, 0x66, 0x26,
    0xd0, 0x36, 0xe1, 0xe9, 0x70, 0xdc, 0xcd, 0x2f, 0x4a, 0x67, 0x5d, 0xd2, 0x60, 0xb5, 0x9d, 0x7f,
    0x45, 0x37, 0x50, 0x44, 0x78, 0x04, 0x19, 0x2c, 0xef, 0xfd, 0x64, 0x81, 0x03, 0xda, 0x95, 0x4c,
    0x7a, 0x0b, 0xad, 0x1f, 0xba, 0xdd, 0x3e, 0xf9, 0xd7, 0x1a, 0x29, 0xf8, 0x18, 0xb3, 0x20, 0xf6,
    0xd1, 0x5e, 0x34, 0x92, 0x7b, 0x24, 0x43, 0x88, 0x97, 0xd4, 0x0f, 0x35, 0xaa, 0x83, 0x68, 0x27,
    0xa8, 0xd5, 0xbe, 0xfa, 0x14, 0x31, 0xaf, 0x10, 0x0d, 0xd8, 0x6a, 0xce, 0x23, 0x61, 0xf3, 0x3d,
    0xa4, 0x08, 0x33, 0xe3, 0xa9, 0x38, 0xe6, 0x93, 0x1d, 0x1c, 0xf0, 0x0e, 0x87, 0x59, 0x65, 0x82,
    0xbc, 0xff, 0xfe, 0x7e, 0x8f, 0xc1, 0x1e, 0xf5, 0xcb, 0x49, 0x02, 0x32, 0x09, 0xc4, 0x8e, 0xc6,
    0x2b, 0x40, 0xa7, 0x17, 0x76, 0x3b, 0x16, 0x2a, 0xc8, 0xfb, 0xb2, 0x58, 0xa5, 0x15, 0xae, 0x25,
    0xcf, 0x46, 0xc7, 0x48, 0xb4, 0x0a, 0x3f, 0xc9, 0x06, 0x85, 0x51, 0x89, 0x62, 0x4d, 0x12, 0x8c,
    0xea, 0xa2, 0x98, 0x4b, 0x79, 0x6f, 0x5c, 0x47, 0x30, 0x1b, 0xe7, 0xc5, 0x22, 0x9c, 0xe8, 0x96,
    0x3a, 0xe4, 0x7c, 0xe0, 0x69, 0xa1, 0xb7, 0x05, 0x39, 0x74, 0x01, 0x9f, 0xbd, 0xc3, 0x84, 0xfc,
    0x77, 0x86, 0x13, 0x4e, 0xbf, 0xf2, 0x53, 0x5b, 0xed, 0x21, 0x8b, 0x6d, 0xc2, 0x41, 0xb6, 0xdb,
    0x3c, 0xd3, 0x28, 0xec, 0x2d, 0xe2, 0x9b, 0xa6, 0x42, 0x52, 0x57, 0x5f, 0xe5, 0xab, 0xb0, 0x0c,
};



void
field_main()
{
    PsyqSetDefDrawEnv( &field_draw_env, 0x0, 0x8, 0x140, 0xe0 );
    field_draw_env.dtd = 1;
    field_draw_env.isbg = 0;

    field_load_mim_dat_files();

    field_main_loop();
}



void
field_main_loop()
{
    base_draw_offset.x = 0xa0;
    base_draw_offset.y = 0x78;

    //V0 = w[8009a044];
    //A0 = w[V0] + 4;
    //[800e4274] = w(A0); // offset to walkmesh block
    //V0 = w[V0];
    //[80114458] = w(A0 + hu[V0] * 18); // walkmesh triangle access block

    if( 0 )
    {
        u32 events_addr = READ_LE_U32( &field_dat[0x0] ) - field_dat_base_addr;
        u8 number_of_entity = READ_LE_U8( &field_dat[events_addr + 0x2] );
        u32 music_offset = events_addr + READ_LE_U32( &field_dat[events_addr + 0x20 + number_of_entity * 0x8 + 0 * 0x4] );
        u16 music_size = READ_LE_U16( &field_dat[music_offset + 0x6] );

        AkaoCopyMusic( &field_dat[music_offset + 0x10], music_size );
        AkaoMusicChannelsInit();
    }
    else
    {
        std::vector<u8> music;
        //FileRead( "Aerith's_Theme.snd", music );
        //FileRead( "Lurking_In_The_Darkness.snd", music );
        //FileRead( "Fighting.snd", music );

        //std::vector<u8> instr2_all;
        //FileRead( "data/SOUND/INSTR2.ALL", instr2_all );
        //std::vector<u8> instr2_dat;
        //FileRead( "data/SOUND/INSTR2.DAT", instr2_dat );
        //AkaoLoadInstr2( &instr2_all[0], &instr2_dat[0] );

        FileRead( "data/ENEMY6/OVER2.SND", music );

        u16 music_size = READ_LE_U16( &music[0x6] );

        //AkaoSetReverbMode( READ_LE_S16( &music[0x8] ) );
        //AkaoCopyMusic( &music[0x10], music_size );
        //AkaoMusicChannelsInit();
    }







    u32 walkmesh_addr = READ_LE_U32( &field_dat[ 0x4 ] ) - field_dat_base_addr;
    u32 id_n = READ_LE_U32( &field_dat[ walkmesh_addr ] );
    walkmesh_addr += 0x4;
    u32 walkmesh_access_addr = walkmesh_addr + id_n * 0x18;
    printf( "Number of trianges in walkmesh: 0x%02x\n", id_n );
    field_walkmesh.resize( id_n );
    field_walkmesh_access.resize( id_n );
    for( int i = 0; i < id_n; ++i )
    {
        field_walkmesh[ i ].p1.vx = READ_LE_S16( &field_dat[ walkmesh_addr + i * 0x18 + 0x0 ]  );
        field_walkmesh[ i ].p1.vy = READ_LE_S16( &field_dat[ walkmesh_addr + i * 0x18 + 0x2 ]  );
        field_walkmesh[ i ].p1.vz = READ_LE_S16( &field_dat[ walkmesh_addr + i * 0x18 + 0x4 ]  );
        field_walkmesh[ i ].p2.vx = READ_LE_S16( &field_dat[ walkmesh_addr + i * 0x18 + 0x8 ]  );
        field_walkmesh[ i ].p2.vy = READ_LE_S16( &field_dat[ walkmesh_addr + i * 0x18 + 0xa ]  );
        field_walkmesh[ i ].p2.vz = READ_LE_S16( &field_dat[ walkmesh_addr + i * 0x18 + 0xc ]  );
        field_walkmesh[ i ].p3.vx = READ_LE_S16( &field_dat[ walkmesh_addr + i * 0x18 + 0x10 ] );
        field_walkmesh[ i ].p3.vy = READ_LE_S16( &field_dat[ walkmesh_addr + i * 0x18 + 0x12 ] );
        field_walkmesh[ i ].p3.vz = READ_LE_S16( &field_dat[ walkmesh_addr + i * 0x18 + 0x14 ] );
        field_walkmesh_access[ i ].p1 = READ_LE_U16( &field_dat[ walkmesh_access_addr + i * 0x6 + 0x0 ] );
        field_walkmesh_access[ i ].p2 = READ_LE_U16( &field_dat[ walkmesh_access_addr + i * 0x6 + 0x2 ] );
        field_walkmesh_access[ i ].p3 = READ_LE_U16( &field_dat[ walkmesh_access_addr + i * 0x6 + 0x4 ] );
    }

    field_rain_init( &field_rain_prim[ 0 ] );
    field_rain_init( &field_rain_prim[ 1 ] );

    while( true )
    {
        PsyqClearOTagR( &field_rain_prim[ 0 ].poly[ 0 ], 0 );

        field_rain_update();

        MATRIX m;

// md1_stin
        m.m[ 0 ][ 0 ] = 0x0fe2;
        m.m[ 0 ][ 1 ] = 0xfe11;
        m.m[ 0 ][ 2 ] = 0x0000;
        m.m[ 1 ][ 0 ] = 0xffcb;
        m.m[ 1 ][ 1 ] = 0xfe52;
        m.m[ 1 ][ 2 ] = 0xf016;
        m.m[ 2 ][ 0 ] = 0x01eb;
        m.m[ 2 ][ 1 ] = 0x0fcb;
        m.m[ 2 ][ 2 ] = 0xfe4f;
        m.t[ 0 ] = 0xfffffe93;
        m.t[ 1 ] = 0x00000ccf;
        m.t[ 2 ] = 0xffff9669;
        PsyqSetGeomScreen( 0x182 );

// md1_1
/*
        m.m[ 0 ][ 0 ] = 0x0c9e;
        m.m[ 0 ][ 1 ] = 0x09d6;
        m.m[ 0 ][ 2 ] = 0xffff;
        m.m[ 1 ][ 0 ] = 0x069d;
        m.m[ 1 ][ 1 ] = 0xf783;
        m.m[ 1 ][ 2 ] = 0xf428;
        m.m[ 2 ][ 0 ] = 0xf8b8;
        m.m[ 2 ][ 1 ] = 0x0956;
        m.m[ 2 ][ 2 ] = 0xf53d;
        m.t[ 0 ] = 0xfffffc1a;
        m.t[ 1 ] = 0x000003a2;
        m.t[ 2 ] = 0x00001415;
        SetGeomScreen( 0x2bf );
*/

        // A1 = ot + 1749c; // rain packets
        // A3 = ot + 17490; // draw_mode_packet
        field_rain_add_to_render( &field_rain_prim[ 0 ].poly[ 0 ], &m );

        PsyqPushMatrix();
        PsyqSetRotMatrix( &m );
        PsyqSetTransMatrix( &m );
        std::array< LINE_F2, 0x1000 > walkmesh_prim;
        for( int i = 0; i < id_n; ++i )
        {
            u32 pt;
            u32 flag;
            DVECTOR sxy1, sxy2, sxy3;
            PsyqRotTransPers( &field_walkmesh[ i ].p1, &sxy1, &pt, &flag );
            PsyqRotTransPers( &field_walkmesh[ i ].p2, &sxy2, &pt, &flag );
            PsyqRotTransPers( &field_walkmesh[ i ].p3, &sxy3, &pt, &flag );

            PsyqSetLineF2( &walkmesh_prim[ i * 3 + 0 ] );
            PsyqSetSemiTrans( &walkmesh_prim[ i * 3 + 0 ], 0 );
            walkmesh_prim[ i * 3 + 0 ].r0 = 0x7f;
            walkmesh_prim[ i * 3 + 0 ].g0 = 0x0;
            walkmesh_prim[ i * 3 + 0 ].b0 = 0x0;
            walkmesh_prim[ i * 3 + 0 ].x0y0 = sxy1;
            walkmesh_prim[ i * 3 + 0 ].x1y1 = sxy2;
            PsyqAddPrim( &field_rain_prim[ 0 ].poly[ 0 ], &walkmesh_prim[ i * 3 + 0 ] );

            PsyqSetLineF2( &walkmesh_prim[ i * 3 + 1 ] );
            PsyqSetSemiTrans( &walkmesh_prim[ i * 3 + 1 ], 0 );
            walkmesh_prim[ i * 3 + 1 ].r0 = 0x7f;
            walkmesh_prim[ i * 3 + 1 ].g0 = 0x0;
            walkmesh_prim[ i * 3 + 1 ].b0 = 0x0;
            walkmesh_prim[ i * 3 + 1 ].x0y0 = sxy1;
            walkmesh_prim[ i * 3 + 1 ].x1y1 = sxy3;
            PsyqAddPrim( &field_rain_prim[ 0 ].poly[ 0 ], &walkmesh_prim[ i * 3 + 1 ] );

            PsyqSetLineF2( &walkmesh_prim[ i * 3 + 2 ] );
            PsyqSetSemiTrans( &walkmesh_prim[ i * 3 + 2 ], 0 );
            walkmesh_prim[ i * 3 + 2 ].r0 = 0x7f;
            walkmesh_prim[ i * 3 + 2 ].g0 = 0x0;
            walkmesh_prim[ i * 3 + 2 ].b0 = 0x0;
            walkmesh_prim[ i * 3 + 2 ].x0y0 = sxy2;
            walkmesh_prim[ i * 3 + 2 ].x1y1 = sxy3;
            PsyqAddPrim( &field_rain_prim[ 0 ].poly[ 0 ], &walkmesh_prim[ i * 3 + 2 ] );
        }
        PsyqPopMatrix();

        PsyqPutDispEnv( &global_dispenv );
        PsyqPutDrawEnv( &field_draw_env );

        PsyqDrawOTag( &field_rain_prim[ 0 ].poly[ 0 ] );

        GameRender();
        if( g_AppRunning == false ) return;
    }
}



void field_update_drawenv()
{
}



void field_load_mim_dat_files()
{
    FileLZS( "data/FIELD/MD1STIN.DAT", field_dat );
}



void field_rain_init( sFieldRainPrim* prim )
{
    for( int i = 0; i < 0x40; ++i )
    {
        field_rain[ i ].m10_wait = i % 8;
        field_rain[ i ].m12_seed = i * 4;
        field_rain[ i ].m16_render = false;

        PsyqSetLineF2( &prim->poly[ i ] );
        PsyqSetSemiTrans( &prim->poly[ i ], 1 );

        LINE_F2* p = &prim->poly[ i ];
        p->r0 = 0x10;
        p->g0 = 0x10;
        p->b0 = 0x10;
    }

/*
    A0 = 0; // tp
    A1 = 1; // abr
    A2 = 0; // vram_x
    A3 = 0; // vram_y
    system_create_texture_page_settings_for_packet();

    A0 = prim + 17490;
    A1 = 0;
    A2 = 0;
    A3 = V0 & ffff;
    A4 = 0;
    system_gpu_create_texture_setting_packet();
*/
    // SetDrawMode(&gDialogDrModes[0][i], 0, 0, GetTPage(0, 0, 0x380, 0x100), &gDialogRects[i]);
}



void field_rain_update()
{
    static int wait = 0;
    ++wait;

    if( wait < 3 ) return;
    wait = 0;


    //if( bu[8009c6e4 + fa4 + 83] & 80 )
    //{
        //if( bu[800e48d8] != ff )
        //{
            //[800e48d8] = b(bu[800e48d8] + 1);
        //}
    //}
    //else if( bu[800e48d8] != 0 )
    //{
        //[800e48d8] = b(bu[800e48d8] - 1);
    //}

    //entity_id = h[800965e0]; // manual visible entity
    // md1_stin
    u32 x = 0xe31dbc;
    u32 y = 0x6b46368;
    u32 z = 0x136000;

    // md1_1
    //u32 x = 0x419000;
    //u32 y = 0x190000;
    //u32 z = 0x510000;

    for( int i = 0; i < 0x40; ++i )
    {
        if( field_rain[ i ].m10_wait == 0 )
        {
            if( i < 0x40 /*( bu[800e48d8] / 4 )*/ )
            {
                field_rain[ i ].m10_wait = 7;
                field_rain[ i ].m16_render = true;

                field_rain[ i ].m12_seed += 1;

                s16 rnd_seed = field_rain[ i ].m12_seed;
                u8 rnd1 = field_random[ rnd_seed & 0xff ];
                u8 rnd2 = field_random[ ( ( rnd_seed * 3 ) & 0xff)];

                field_rain[ i ].m8_p2.vx = (x >> 0xc) /*w[80074ea4 + entity_id * 84 + c] >> c) */ + rnd1 * 0xc - 0x600;
                field_rain[ i ].m8_p2.vy = (y >> 0xc) /*w[80074ea4 + entity_id * 84 + c] >> c) */ + rnd2 * 0xc - 0x600;
                field_rain[ i ].m0_p1 = field_rain[ i ].m8_p2;

                field_rain[ i ].m14 = (z >> 0xc) /*(w[80074ea4 + entity_id * 84 + 14] >> c) */ - 0x300;
            }
            else
            {
                field_rain[ i ].m10_wait = 1;
                field_rain[ i ].m16_render = false;
            }
        }

        field_rain[ i ].m0_p1.vz = field_rain[ i ].m14 + (field_rain[ i ].m10_wait & 7) * 0x80 + 0x100;
        field_rain[ i ].m8_p2.vz = field_rain[ i ].m14 + (field_rain[ i ].m10_wait & 7) * 0x80;

        field_rain[ i ].m10_wait -= 1;
    }
}



void field_rain_add_to_render( sTag* ot, MATRIX* m )
{
    //S1 = A1;
    //packet = A3;

    PsyqPushMatrix();

    PsyqSetRotMatrix( m );
    PsyqSetTransMatrix( m );

    for( int i = 0; i < 0x40; ++i )
    {
        if( field_rain[ i ].m16_render == true )
        {
            u32 pt;
            u32 flag;
            DVECTOR sxy;

            //A1 = S1 + i * 10 + 8;
            PsyqRotTransPers( &field_rain[ i ].m0_p1, &sxy, &pt, &flag );

            LINE_F2* p = &field_rain_prim[ 0 ].poly[ i ];
            //p->x0y0.vx = 500 + field_rain[ i ].m0_p1.vx / 3;
            //p->x0y0.vy = 1900 - field_rain[ i ].m0_p1.vz;
            p->x0y0 = sxy;

            //A1 = S1 + i * 10 + c;
            PsyqRotTransPers( &field_rain[ i ].m8_p2, &sxy, &pt, &flag );

            p->x1y1 = sxy;

            if( i != 0 ) PsyqAddPrim( ot, p );
        }
    }

    PsyqPopMatrix();

    //[packet] = w((w[packet] & ff000000) | (w[otag] & 00ffffff));
    //[otag] = w((w[otag] & ff000000) | (packet & 00ffffff));
}
