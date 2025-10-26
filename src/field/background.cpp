#include "background.h"
#include "field.h"



u16 l_bg2_dm_start;



void FieldBackgroundInitPoly( SPRT_16* p1, SPRT* p2, BgAnim* bg_anim, DR_MODE* dm )
{
    u32 count1 = 0;

    l_bg2_dm_start = 0;

    u32 bg_ofs = READ_LE_U32( &g_field_dat[ 0x8 ] ) - g_field_dat_base_addr;
    u32 block1 = bg_ofs + 10;
    u32 coords = bg_ofs + READ_LE_U32( &g_field_dat[bg_ofs + 0] );
    u32 tpage = bg_ofs + READ_LE_U32( &g_field_dat[bg_ofs + 0x4] );

    // background static 1st layer
    while( true )
    {
        if( READ_LE_U16( &g_field_dat[block1 + 0x0] ) == 0x7fff )
        {
            block1 += 0x2;
            break;
        }

        if( READ_LE_U16( &g_field_dat[block1 + 0x0] ) == 0x7ffe )
        {
            PsyqSetDrawMode( dm, 0, 0x1, READ_LE_U16( &g_field_dat[tpage] ), 0 );
            dm += 0x1;
            tpage += 0x2;
            l_bg2_dm_start += 0x1;
        }
        else
        {
            for( int i = READ_LE_U16( &g_field_dat[block1 + 0x4] ); i != 0; --i )
            {
                PsyqSetSprt16( p1 );
                PsyqSetShadeTex( p1, 0x1 );
                PsyqSetSemiTrans( p1, 0 );
                p1->r0 = 0x80;
                p1->g0 = 0x80;
                p1->b0 = 0x80;
                p1->x0 = READ_LE_U16( &g_field_dat[coords + 0x0] );
                p1->y0 = READ_LE_U16( &g_field_dat[coords + 0x2] );
                p1->u0 = READ_LE_U8( &g_field_dat[coords + 0x4] );
                p1->v0 = READ_LE_U8( &g_field_dat[coords + 0x5] );
                p1->clut = READ_LE_U16( &g_field_dat[coords + 0x6] );
                p1 += 0x1;
                coords += 0x8;
                bg_anim += 0x1;
                count1 += 0x1;
            }
        }
        block1 += 0x6;
    }

    l_bg2_dm_start = count1 - l_bg2_dm_start;
    u32 coords2 = bg_ofs + READ_LE_U32( &g_field_dat[bg_ofs + 0x8] );

    // background 2nd layer with depth
    while( true )
    {
        if( READ_LE_U16( &g_field_dat[block1] ) == 0x7fff )
        {
            block1 += 0x2;
            break;
        }

        for( int i = READ_LE_U16( &g_field_dat[block1 + 0x4] ); i != 0; --i )
        {
            PsyqSetDrawMode( dm, 0, 0x1, READ_LE_U16( &g_field_dat[coords2 + 0x8] ), 0 );
            dm += 0x1;

            PsyqSetSprt16( p1 );
            PsyqSetShadeTex( p1, 0x1 );
            PsyqSetSemiTrans( p1, (READ_LE_U16( &g_field_dat[coords2 + 0xc] ) & 0x80) ? 0x1 : 0 );

            p1->r0 = READ_LE_U8( &g_field_dat[coords2 + 0xa] ); // this is depth used in render
            p1->g0 = READ_LE_U16( &g_field_dat[coords2 + 0xb] ); // this is depth used in render
            p1->b0 = 0x80;
            p1->x0 = READ_LE_U16( &g_field_dat[coords2 + 0x0] );
            p1->y0 = READ_LE_U16( &g_field_dat[coords2 + 0x2] );
            p1->u0 = READ_LE_U8( &g_field_dat[coords2 + 0x4] );
            p1->v0 = READ_LE_U8( &g_field_dat[coords2 + 0x5] );
            p1->clut = READ_LE_U16( &g_field_dat[coords2 + 0x6] );
            p1 += 0x1;
            coords2 += 0xe;

            bg_anim->anim_id = READ_LE_U8( &g_field_dat[coords2 + 0xc] );
            bg_anim->frame_id = READ_LE_U8( &g_field_dat[coords2 + 0xd] );
            bg_anim += 0x1;

            count1 += 0x1;
        }
        block1 += 0x6;
    }
}



void FieldBackgroundAddToRender( FieldRenderData& render_data )
{
    u32 bg_ofs = READ_LE_U32( &g_field_dat[ 0x8 ] ) - g_field_dat_base_addr;

    u32 block1 = bg_ofs + 0x10;

    // background static 1st layer
    while( true )
    {
        if( READ_LE_U16( &g_field_dat[block1 + 0x0] ) == 0x7fff )
        {
            block1 += 0x2;
            break;
        }

        if( READ_LE_U16( &g_field_dat[block1 + 0x0] ) == 0x7ffe )
        {
            u16 dm_id = READ_LE_U16( &g_field_dat[block1 + 0x2] );
            PsyqAddPrim( &render_data.ot_scene[0x1000 - 0x1], &render_data.bg_dm[dm_id] );
        }
        else
        {
            u16 bg_id = READ_LE_U16( &g_field_dat[block1 + 0x2] );

            for( int i = READ_LE_U16( &g_field_dat[block1 + 0x4] ); i != 0; --i )
            {
                PsyqAddPrim( &render_data.ot_scene[0x1000 - 0x1], &render_data.bg_1[bg_id] );
                bg_id += 0x1;
            }
        }
        block1 += 0x6;
    }

    // background 2nd layer with depth
    while( true )
    {
        if( READ_LE_U16( &g_field_dat[block1 + 0x0] ) == 0x7fff )
        {
            block1 += 0x2;
            break;
        }

        u16 bg_id = READ_LE_U16( &g_field_dat[block1 + 0x2] );

        for( int i = READ_LE_U16( &g_field_dat[block1 + 0x4] ); i != 0; --i )
        {
            u8 anim_id = render_data.bg_anim[bg_id].anim_id & 0x3f;
            u8 frame_id = render_data.bg_anim[bg_id].frame_id;

            u16 depth = (render_data.bg_1[bg_id].g0 << 8) + (render_data.bg_1[bg_id].r0);
            PsyqAddPrim( &render_data.ot_scene[depth], &render_data.bg_1[bg_id] );

            u16 dm_id = bg_id - l_bg2_dm_start;
            PsyqAddPrim( &render_data.ot_scene[depth], &render_data.bg_dm[dm_id] );

            bg_id += 1;
        }
        block1 += 0x6;
    }
}
