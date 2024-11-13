#include "field.h"
#include "../game.h"



std::array< sFieldRain, 0x40 > field_rain;
std::array< sFieldRainPrim, 2 > field_rain_prim;

std::vector< u8> field_random =
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
    ////////////////////////////////
    // MISSING
    ////////////////////////////////

    field_main_loop();

    ////////////////////////////////
    // MISSING
    ////////////////////////////////
}



void
field_main_loop()
{
    field_rain_init( &field_rain_prim[ 0 ] );
    field_rain_init( &field_rain_prim[ 1 ] );

    while( true )
    {
        ClearOTagR( &field_rain_prim[ 0 ].poly[ 0 ], 0 );

        field_rain_update();

        // A1 = ot + 1749c; // rain packets
        // A2 = w[80071e40]; // matrix
        // A3 = ot + 17490; // draw_mode_packet
        field_rain_add_to_render( &field_rain_prim[ 0 ].poly[ 0 ] );

        DrawOTag( &field_rain_prim[ 0 ].poly[ 0 ] );

        GameRender();
        if( g_AppRunning == false ) return;
    }
}



void
field_rain_init( sFieldRainPrim* prim )
{
    for( int i = 0; i < 0x40; ++i )
    {
        field_rain[ i ].m10_wait = i % 8;
        field_rain[ i ].m12_seed = i * 4;
        field_rain[ i ].m16_render = false;

        SetLineF2( &prim->poly[ i ] );
        SetSemiTrans( &prim->poly[ i ], 1 );

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



void
field_rain_update()
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
                u8 rnd1 = field_random[ rnd_seed ];
                u8 rnd2 = field_random[ ( ( rnd_seed * 3 ) & 0xff)];

                field_rain[ i ].m8_p2.vx = /*w[80074ea4 + entity_id * 84 + c] >> c) + */ rnd1 * 0xc - 0x600;
                field_rain[ i ].m8_p2.vy = /*w[80074ea4 + entity_id * 84 + c] >> c) + */ rnd2 * 0xc - 0x600;
                field_rain[ i ].m0_p1 = field_rain[ i ].m8_p2;

                field_rain[ i ].m14 = /*(w[80074ea4 + entity_id * 84 + 14] >> c) - */ 0x300;
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



void
field_rain_add_to_render( sTag* ot )
{
    //S1 = A1;
    //matrix = A2;
    //packet = A3;

    //system_psyq_push_matrix();

    //A0 = matrix;
    //system_psyq_set_rot_matrix();

    //A0 = matrix;
    //system_psyq_set_trans_matrix();

    for( int i = 0; i < 0x40; ++i )
    {
        if( field_rain[ i ].m16_render == true )
        {
            //A0 = 800e42d8 + i * 18 + 0;
            //A1 = S1 + i * 10 + 8;
            //A2 = SP + 10;
            //A3 = SP + 14;
            //system_psyq_rot_trans_pers();

            //A0 = 800e42d8 + i * 18 + 8;
            //A1 = S1 + i * 10 + c;
            //A2 = SP + 10;
            //A3 = SP + 14;
            //system_psyq_rot_trans_pers();

            LINE_F2* p = &field_rain_prim[ 0 ].poly[ i ];
            p->x0y0.vx = 500 + field_rain[ i ].m0_p1.vx / 3;
            p->x0y0.vy = 1900 - field_rain[ i ].m0_p1.vz;
            p->x1y1.vx = 500 + field_rain[ i ].m8_p2.vx / 3;
            p->x1y1.vy = 1900 - field_rain[ i ].m8_p2.vz;

            if( i != 0 ) AddPrim( ot, p );
        }
    }

    //system_psyq_pop_matrix();

    //[packet] = w((w[packet] & ff000000) | (w[otag] & 00ffffff));
    //[otag] = w((w[otag] & ff000000) | (packet & 00ffffff));
}
