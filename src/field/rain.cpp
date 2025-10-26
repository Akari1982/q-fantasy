#include "rain.h"
#include "psyq/libgte.h"



u8 g_rain_enable = 0;
u8 g_rain_force = 0;

std::array< Rain, 0x40 > l_rain;



void FieldRainInit( FieldRenderData* data )
{
    for( int i = 0; i < l_rain.size(); ++i )
    {
        l_rain[ i ].wait = i % 0x8;
        l_rain[ i ].seed = i * 0x4;
        l_rain[ i ].render = false;

        LINE_F2* p = &data->rain[i];
        PsyqSetLineF2( p );
        PsyqSetSemiTrans( p, 0x1 );
        p->r0 = 0x10;
        p->g0 = 0x10;
        p->b0 = 0x10;
    }

    PsyqSetDrawMode( &data->rain_dm, 0, 0, PsyqGetTPage( 0, 0x1, 0, 0 ), 0 );
}



void FieldRainUpdate()
{
    static int wait = 0;
    ++wait;

    if( wait < 0x3 ) return;
    wait = 0;

    if( g_rain_enable & 0x80 )
    {
        if( g_rain_force != 0xff ) g_rain_force += 1;
    }
    else if( g_rain_force != 0 )
    {
        g_rain_force -= 1;
    }

//    pc_entity_id = h[0x800965e0]; // manual visible entity

    for( int i = 0; i < l_rain.size(); ++i )
    {
        if( l_rain[ i ].wait == 0 )
        {
            if( i < (g_rain_force / 0x4) )
            {
                l_rain[ i ].wait = 0x7;
                l_rain[ i ].render = true;

                l_rain[ i ].seed += 0x1;

                s16 rnd_seed = l_rain[ i ].seed;
                u8 rnd1 = g_field_random[ rnd_seed & 0xff ];
                u8 rnd2 = g_field_random[ ( ( rnd_seed * 0x3 ) & 0xff)];

                l_rain[ i ].p2.vx = /*w[80074ea4 + pc_entity_id * 84 + c] >> c) */ + rnd1 * 0xc - 0x600;
                l_rain[ i ].p2.vy = /*w[80074ea4 + pc_entity_id * 84 + c] >> c) */ + rnd2 * 0xc - 0x600;
                l_rain[ i ].p1 = l_rain[ i ].p2;

                l_rain[ i ].z = /*(w[80074ea4 + pc_entity_id * 84 + 14] >> c) */ - 0x300;
            }
            else
            {
                l_rain[ i ].wait = 0x1;
                l_rain[ i ].render = false;
            }
        }

        l_rain[ i ].p1.vz = l_rain[ i ].z + (l_rain[ i ].wait & 0x7) * 0x80 + 0x100;
        l_rain[ i ].p2.vz = l_rain[ i ].z + (l_rain[ i ].wait & 0x7) * 0x80;

        l_rain[ i ].wait -= 0x1;
    }
}



void FieldRainAddToRender( OTag* ot, LINE_F2* rain, MATRIX* m, DR_MODE* dm )
{
    PsyqPushMatrix();
    PsyqSetRotMatrix( m );
    PsyqSetTransMatrix( m );

    for( int i = 0; i < l_rain.size(); ++i )
    {
        if( l_rain[ i ].render == true )
        {
            LINE_F2* p = rain + i;

            u32 pt;
            u32 flag;
            DVECTOR sxy;

            PsyqRotTransPers( &l_rain[ i ].p1, &sxy, &pt, &flag );
            p->x0 = sxy.vx;
            p->y0 = sxy.vy;
            PsyqRotTransPers( &l_rain[ i ].p2, &sxy, &pt, &flag );
            p->x1 = sxy.vx;
            p->y1 = sxy.vy;

            PsyqAddPrim( ot, p );
        }
    }

    PsyqPopMatrix();

    PsyqAddPrim( ot, dm );
}
