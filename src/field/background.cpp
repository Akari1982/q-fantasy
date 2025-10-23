#include "background.h"



void FieldBackgroundInitPoly( SPRT_16* p1, SPRT* p2, BgAnim* bg_anim, DR_MODE* dm )
{
/*
    V0 = w[0x8009d848];
    background = w[V0];

    count1 = 0;
    count2 = 0;
    g_bg2_dm_start = 0;
    g_bg3_dm_start = 0;

    block1 = background + 10;
    coords = background + w[background + 0];
    tpage = background + w[background + 4];
    coords2 = background + w[background + 8];
    coords3 = background + w[background + c];

    // background static 1st layer
    while( true )
    {
        if( h[block1] == 0x7fff )
        {
            block1 += 0x2;
            break;
        }
        else if( h[block1] == 0x7ffe )
        {
            system_psyq_set_draw_mode( dm, 0, 1, hu[tpage], 0 );
            dm += 1;
            tpage += 0x2;
            g_bg2_dm_start += 1;
        }
        else
        {
            for( int i = h[block1 + 0x4]; i != 0; --i )
            {
                SETLEN( p1, 0x3 );
                p1->r0 = 0x80;
                p1->g0 = 0x80;
                p1->b0 = 0x80;
                p1->code = 0x7d; // Textured Rectangle, 16x16, opaque, raw-texture
                p1->x0 = hu[coords + 0];
                p1->y0 = hu[coords + 2];
                p1->u0 = bu[coords + 4];
                p1->v0 = bu[coords + 5];
                p1->clut = hu[coords + 6];
                p1 += 1;
                coords += 0x8;

                bg_anim += 1;

                count1 += 1;
            }
        }

        block1 += 0x6;
    }

    g_bg2_dm_start = count1 - g_bg2_dm_start;

    // background 2nd layer with depth
    while( true )
    {
        if( h[block1] == 0x7fff )
        {
            block1 += 0x2;
            break;
        }

        for( int i = h[block1 + 0x4]; i != 0; --i )
        {
            u16 tpage = hu[coords2 + 8];
            system_psyq_set_draw_mode( dm, 0, 1, tpage, 0 );
            dm += 1;
            g_bg3_dm_start += 1;

            SETLEN( p1, 0x3 );
            [p1 + 4] = h(hu[coords2 + a]); // distance
            p1->b0 = 0x80;
            p1->code = ( hu[coords2 + c] & 0x80 ) ? 0x7f : 0x7d; // add transparency
            p1->x0 = hu[coords2 + 0];
            p1->y0 = hu[coords2 + 2];
            p1->u0 = bu[coords2 + 4];
            p1->v0 = bu[coords2 + 5];
            p1->clut = hu[coords2 + 6];
            p1 += 1;
            coords2 += 0xe;

            bg_anim->anim_id = bu[coords2 + c];
            bg_anim->frame_id = bu[coords2 + d];
            bg_anim += 1;

            count1 += 1;
        }

        block1 += 0x6;
    }

    g_bg3_anim_start = count1;

    // background dynamic 3rd layer
    while( true )
    {
        if( h[block1] == 0x7fff )
        {
            block1 += 0x2;
            break;
        }
        else if( h[block1] == 0x7ffe )
        {
            system_psyq_set_draw_mode( dm, 0, 1, hu[tpage], 0 );
            dm += 1;
            tpage += 2;
        }
        else
        {
            [block1 + 0x2] = h(count2);

            for( int i = h[block1 + 0x4]; i != 0; --i )
            {
                SETLEN( p2, 0x4 );
                p2->r0 = 0x80;
                p2->g0 = 0x80;
                p2->b0 = 0x80;
                p2->code = ( bu[coords3 + 8] & 0x80 ) ? 0x67 : 0x65;
                p2->x0 = hu[coords3 + 0];
                p2->y0 = hu[coords3 + 2];
                p2->u0 = bu[coords3 + 4];
                p2->v0 = bu[coords3 + 5];
                p2->clut = hu[coords3 + 6];
                p2->w = 0x20;
                p2->h = 0x20;
                p2 += 1;
                coords3 += 0xa;

                bg_anim->anim_id = bu[coords3 + 8];
                bg_anim->frame_id = bu[coords3 + 9];
                bg_anim += 1;

                count2 += 1;
            }
        }

        block1 += 0x6;
    }

    // background dynamic 4th layer
    while( true )
    {
        if( h[block1] == 0x7fff )
        {
            break;
        }
        else if( h[block1] == 0x7ffe )
        {
            system_psyq_set_draw_mode( dm, 0, 1, hu[tpage], 0 );
            dm += 1;
            tpage += 2;
        }
        else
        {
            [block1 + 0x2] = h(count2);

            for( int i = h[block1 + 0x4]; i != 0; --i )
            {
                SETLEN( p2, 0x4 );
                p2->r0 = 0x80;
                p2->g0 = 0x80;
                p2->b0 = 0x80;
                p2->code = ( bu[coords3 + 0x8] & 0x80 ) ? 0x67 : 0x65;
                p2->x0 = hu[coords3 + 0];
                p2->y0 = hu[coords3 + 2];
                p2->u0 = bu[coords3 + 4];
                p2->v0 = bu[coords3 + 5];
                p2->clut = hu[coords3 + 6];
                p2->w = 0x20;
                p2->h = 0x20;
                p2 += 1;
                coords3 += 0xa;

                bg_anim->anim_id = bu[coords3 + 8]; // animation
                bg_anim->frame_id = bu[coords3 + 9]; // index
                bg_anim += 1;

                count2 += 1;
            }
        }

        block1 += 0x6;
    }
*/
}
