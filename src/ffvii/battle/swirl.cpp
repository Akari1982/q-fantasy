#include "swirl.h"
#include "field/field.h"
#include "psyq/libgpu.h"
#include "psyq/libgte.h"



struct BattleSwirl
{
    OTag ot;
    POLY_FT4 poly[0xa][0x7];
};
BattleSwirl l_swirl[0x2];
OTag l_swirl_ot;
SVECTOR l_swirl_vec[0xb][0x8];
DVECTOR l_swirl_pos[0xb][0x8];
SVECTOR l_swirl_rot_vec;
VECTOR l_swirl_scale_vec;
DRAWENV l_swirl_drawenv;
DISPENV l_swirl_dispenv;
u32 l_swirl_rb;
u32 l_swirl_scale;
u32 l_swirl_step;
u32 l_swirl_is_render;
u32 l_swirl_col;



void SwirlUpdate()
{
    //PsyqMoveImage(0x80062d44, 0, 0x8);

    l_swirl_rb += 0x1;
    u32 rb = l_swirl_rb & 0x1;

    BattleSwirl& swirl = l_swirl[rb];

    l_swirl_ot = l_swirl[rb].ot;

    u32 step = l_swirl_step;
    l_swirl_step += 0x1;

    if (step >= 0x2f) // start fade out
    {
        if (l_swirl_col != 0) l_swirl_col -= 0x1;

        for(int y = 0; y < 0x7; ++y)
        {
            for(int x = 0; x < 0xa; ++x)
            {
                swirl.poly[x][y].r0 = l_swirl_col;
                swirl.poly[x][y].g0 = l_swirl_col;
                swirl.poly[x][y].b0 = l_swirl_col;
                swirl.poly[x][y].code = l_swirl[rb].poly[0][0].code;
            }
        }
    }
    else
    {
        for(int y = 0; y < 0x7; ++y)
        {
            for(int x = 0; x < 0xa; ++x)
            {
                swirl.poly[x][y].tpage = PsyqGetTPage(0x2, ((l_swirl_step % 0x5) == 0) ? 0x3 : 0, (x << 0x5) & 0xffc0, 0);
            }
        }
    }

    //PsyqRotMatrix(&l_swirl_rot_vec, 0x80063028);
    //PsyqScaleMatrix(0x80063028, &l_swirl_scale_vec);
    //PsyqSetRotMatrix(0x80063028);
    //PsyqSetTransMatrix(0x80063028);

    u32 scale = l_swirl_scale;
    l_swirl_scale += 0xa;

    l_swirl_rot_vec.vz -= 0x3;
    l_swirl_scale_vec.vx += scale / 0x10;
    l_swirl_scale_vec.vy += scale / 0x10;

    for(int y = 0; y < 0x8; ++y)
    {
        for(int x = 0; x < 0xb; ++x)
        {
            //PsyqRotTransPers(&l_swirl_vec[x][y], &l_swirl_pos[x][y], SP + 0x18, SP + 0x1c);
        }
    }

    PsyqClearOTag(&swirl.ot, 0x1);

    for(int y = 0; y < 0x7; ++y)
    {
        for(int x = 0; x < 0xa; ++x)
        {
            POLY_FT4* poly = &swirl.poly[x][y];

            poly->x0 = l_swirl_pos[x + 0x0][y + 0x0].vx;
            poly->y0 = l_swirl_pos[x + 0x0][y + 0x0].vy;
            poly->x1 = l_swirl_pos[x + 0x1][y + 0x0].vx;
            poly->y1 = l_swirl_pos[x + 0x1][y + 0x0].vy;
            poly->x2 = l_swirl_pos[x + 0x0][y + 0x1].vx;
            poly->y2 = l_swirl_pos[x + 0x0][y + 0x1].vy;
            poly->x3 = l_swirl_pos[x + 0x1][y + 0x1].vx;
            poly->y3 = l_swirl_pos[x + 0x1][y + 0x1].vy;

            PsyqAddPrim(&swirl.ot, poly);
        }
    }
}



bool SwirlRender()
{
    l_swirl_is_render += 0x1;

    if ((l_swirl_is_render & 0x1) == 0)
    {
        PsyqDrawOTag(&l_swirl_ot);
        SwirlUpdate();
    }

    if (l_swirl_step >= 0x4f) return false;

    return true;
}



void SwirlInit()
{
    PsyqVSync(0);

    //PsyqGetDispenv(&l_swirl_dispenv);
    PsyqSetDefDispEnv(&l_swirl_dispenv, 0, 0xe8, 0x140, 0xf0);
    l_swirl_dispenv.screen.y = (l_swirl_dispenv.screen.y < 0x11) ? 0 : 0x18;
    l_swirl_dispenv.isrgb24 = 0; // 16-bit mode

    PsyqSetDefDrawEnv(&l_swirl_drawenv, 0, 0xf0, 0x140, 0xe0);
    l_swirl_drawenv.tpage = 0;
    l_swirl_drawenv.dtd = 0; // dithering processing flag off
    l_swirl_drawenv.dfe = 0x1; // drawing to display area is permitted
    l_swirl_drawenv.isbg = 0; // does not clear drawing area when drawing environment is set

    PsyqPutDispEnv(&l_swirl_dispenv);
    PsyqPutDrawEnv(&l_swirl_drawenv);

    PsyqSetGeomOffset(0x9f, 0x77);
    PsyqSetGeomScreen(0x1e0);

    l_swirl_is_render = 0;

    // if current screen 0 then we copy it's content to screen 1
    if (g_field_rb == 0)
    {
        SRECT rect;
        rect.x = 0;
        rect.y = 0x8;
        rect.w = 0x140;
        rect.h = 0xe0;
        PsyqMoveImage(&rect, 0, 0xf0);
    }

    // add transparency flag because we will use it as texture and do blending
    SRECT rect1, rect2;
    u8* temp1;
    u8* temp2;
    for (int i = 0; i < 0x4; ++i)
    {
        rect2 = rect1;
        temp2 = temp1;

        rect1.x = 0;
        rect1.y = 0xf0 + i * 0x4a;
        rect1.w = 0x140;
        rect1.h = 0x4a;
        //temp1 = (i & 0x1) ? 0x801b0000 : 0x801b8000;

        PsyqDrawSync(0);

        if (i > 0)
        {
            for(int j = 0; j < 0x2e40; ++j)
            {
//                [temp2 + j * 0x4] = w(w[temp2 + j * 0x4] | 0x80008000); // add transparency flag
            }
            PsyqLoadImage(&rect2, temp2);
        }

        if (i < 0x3)
        {
            //PsyqStoreImage(&rect1, temp1);
        }
    }

    l_swirl_step = 0;
    l_swirl_col = 0x80;
    l_swirl_scale = 0x4;
    l_swirl_scale_vec.vx = 0x820;
    l_swirl_scale_vec.vy = 0x820;
    l_swirl_scale_vec.vz = 0x1000;
    l_swirl_rot_vec.vx = 0;
    l_swirl_rot_vec.vy = 0;
    l_swirl_rot_vec.vz = 0;

    // set up vertexes for swirl effect
    // they will be transformed by calculated matrix to screen space
    for(int y = 0; y < 0x8; ++y)
    {
        for(int x = 0; x < 0xb; ++x)
        {
            l_swirl_vec[x][y].vx = -0xa0 + x * 0x20;
            l_swirl_vec[x][y].vy = -0x70 + y * 0x20;
            l_swirl_vec[x][y].vz = 0;
        }
    }

    for(int y = 0; y < 0x7; ++y)
    {
        for(int x = 0; x < 0xa; ++x)
        {
            POLY_FT4* poly = &l_swirl[0].poly[x][y];

            PsyqSetPolyFT4(poly);
            PsyqSetSemiTrans(poly, 0x1);
            PsyqSetShadeTex(poly, 0);

            u8 u = (x * 0x20) & 0x3f;
            u8 v = 0x8 + y * 0x20;

            poly->r0 = 0x80;
            poly->g0 = 0x80;
            poly->b0 = 0x80;
            poly->u0 = u;
            poly->v0 = v;
            poly->u1 = u + 0x1f;
            poly->v1 = v;
            poly->u2 = u;
            poly->v2 = v + 0x1f;
            poly->u3 = u + 0x1f;
            poly->v3 = v + 0x1f;
        }
    }

    memcpy(&l_swirl[0x1], &l_swirl[0x0], 0xaf4);

    SwirlUpdate();

    //g_bg_render = BG_RENDER_BATTLE_SWIRL; // set render func to 3 (swirl render)
}
