#include "fade.h"
#include "field.h"
#include "psyq/libgpu.h"
#include "kernel/game.h"
#include "system/logger.h"



struct FadePoly
{
    DR_MODE dr_mode;
    TILE tile[0x2];
};

FadePoly l_fade[0x2];

OTag g_fade_ot[0x2];



void FadeSetDrawMode(int abr)
{
    u16 tpage = PsyqGetTPage(0, abr, 0, 0);
    PsyqSetDrawMode(&l_fade[0x0].dr_mode, 0, 0, tpage, 0);
    PsyqSetDrawMode(&l_fade[0x1].dr_mode, 0, 0, tpage, 0);
}



void FadeInitPoly()
{
    for (int i = 0; i < 0x2; ++i)
    {
        for (int j = 0; j < 0x2; ++j)
        {
            TILE* poly = &l_fade[i].tile[j];
            PsyqSetTile(poly);
            PsyqSetSemiTrans(poly, 0x1);
            PsyqSetShadeTex(poly, 0x1);

            poly->r0 = 0;
            poly->g0 = 0;
            poly->b0 = 0;
            poly->w = 0xa0;
            poly->h = 0xe0;
        }
    }

    l_fade[0x0].tile[0x0].x0 = 0;
    l_fade[0x0].tile[0x0].y0 = 0;
    l_fade[0x0].tile[0x1].x0 = 0xa0;
    l_fade[0x0].tile[0x1].y0 = 0;

    l_fade[0x1].tile[0x0].x0 = 0;
    l_fade[0x1].tile[0x0].y0 = 0;
    l_fade[0x1].tile[0x1].x0 = 0xa0;
    l_fade[0x1].tile[0x1].y0 = 0;
}



void FadeCopyScreen()
{
    PsyqMoveImage(&g_main_dispenv[g_field_rb].disp, g_main_dispenv[(g_field_rb + 0x1) & 0x1].disp.x, g_main_dispenv[(g_field_rb + 0x1) & 0x1].disp.y);
}



void FadeSetPolyMonochrome(u8 gray)
{
    PsyqClearOTagR(&g_fade_ot[g_field_rb], 0x1);

    l_fade[g_field_rb].tile[0x0].r0 = gray;
    l_fade[g_field_rb].tile[0x0].g0 = gray;
    l_fade[g_field_rb].tile[0x0].b0 = gray;
    l_fade[g_field_rb].tile[0x1].r0 = gray;
    l_fade[g_field_rb].tile[0x1].g0 = gray;
    l_fade[g_field_rb].tile[0x1].b0 = gray;

    PsyqAddPrim(&g_fade_ot[g_field_rb], &l_fade[g_field_rb].tile[0x0]);
    PsyqAddPrim(&g_fade_ot[g_field_rb], &l_fade[g_field_rb].tile[0x1]);
    PsyqAddPrim(&g_fade_ot[g_field_rb], &l_fade[g_field_rb].dr_mode);
}



void FadeSetPolyRGBGradual(u16 r, u16 g, u16 b)
{
    PsyqClearOTagR(&g_fade_ot[g_field_rb], 0x1);

    l_fade[g_field_rb].tile[0x0].r0 = (g_field_control.fade_steps * r) >> 0x8;
    l_fade[g_field_rb].tile[0x0].g0 = (g_field_control.fade_steps * g) >> 0x8;
    l_fade[g_field_rb].tile[0x0].b0 = (g_field_control.fade_steps * b) >> 0x8;
    l_fade[g_field_rb].tile[0x1].r0 = (g_field_control.fade_steps * r) >> 0x8;
    l_fade[g_field_rb].tile[0x1].g0 = (g_field_control.fade_steps * g) >> 0x8;
    l_fade[g_field_rb].tile[0x1].b0 = (g_field_control.fade_steps * b) >> 0x8;

    PsyqAddPrim(&g_fade_ot[g_field_rb], &l_fade[g_field_rb].tile[0x0]);
    PsyqAddPrim(&g_fade_ot[g_field_rb], &l_fade[g_field_rb].tile[0x1]);
    PsyqAddPrim(&g_fade_ot[g_field_rb], &l_fade[g_field_rb].dr_mode);
}



void FadeSetPolyRGBDirect(u8 r, u8 g, u8 b)
{
    PsyqClearOTagR(&g_fade_ot[g_field_rb], 0x1);

    l_fade[g_field_rb].tile[0x0].r0 = r;
    l_fade[g_field_rb].tile[0x0].g0 = g;
    l_fade[g_field_rb].tile[0x0].b0 = b;
    l_fade[g_field_rb].tile[0x1].r0 = r;
    l_fade[g_field_rb].tile[0x1].g0 = g;
    l_fade[g_field_rb].tile[0x1].b0 = b;

    PsyqAddPrim(&g_fade_ot[g_field_rb], &l_fade[g_field_rb].tile[0x0]);
    PsyqAddPrim(&g_fade_ot[g_field_rb], &l_fade[g_field_rb].tile[0x1]);
    PsyqAddPrim(&g_fade_ot[g_field_rb], &l_fade[g_field_rb].dr_mode);
}



void FadeStepsDiscrease()
{
    g_field_control.disable_render = 0;

    g_field_control.fade_steps -= g_field_control.fade_step;

    if ((g_field_control.fade_steps <= 0)/* || (g_movie_play == 0x1)*/)
    {
        g_field_control.fade_type = FADE_TYPE_NONE;
        g_field_control.fade_steps = 0;
    }
}



void FadeStepsIncrease()
{
    g_field_control.disable_render = 0;

    g_field_control.fade_steps += g_field_control.fade_step;

    if ((g_field_control.fade_steps >= 0x100))
    {
        g_field_control.fade_steps = 0xff;
    }
}



u8 FadeColorInterpolate(s16 col_from, s16 col_to, s16 steps_cur, s16 steps_dst)
{
    return col_from + ((col_to - col_from) * steps_cur) / steps_dst;
}



void FadeSetPolyRGBInterpolate()
{
    g_field_control.disable_render = 0;

    g_field_control.fade_steps += 0x1;

    if (g_field_control.fade_steps >= g_field_control.fade_step)
    {
        g_field_control.nfade_r_from = g_field_control.nfade_r_to;
        g_field_control.nfade_g_from = g_field_control.nfade_g_to;
        g_field_control.nfade_b_from = g_field_control.nfade_b_to;
        g_field_control.fade_steps = g_field_control.fade_step;
    }

    g_field_control.fade_r = FadeColorInterpolate(g_field_control.nfade_r_from, g_field_control.nfade_r_to, g_field_control.fade_steps, g_field_control.fade_step);
    g_field_control.fade_g = FadeColorInterpolate(g_field_control.nfade_g_from, g_field_control.nfade_g_to, g_field_control.fade_steps, g_field_control.fade_step);
    g_field_control.fade_b = FadeColorInterpolate(g_field_control.nfade_b_from, g_field_control.nfade_b_to, g_field_control.fade_steps, g_field_control.fade_step);

    FadeSetPolyRGBDirect(g_field_control.fade_r, g_field_control.fade_g, g_field_control.fade_b);
}



void FadeUpdate()
{
    switch (g_field_control.fade_type)
    {
        case FADE_TYPE_NONE:
        {
            g_field_control.disable_render = 0;
            g_field_control.nfade_r_from = 0;
            g_field_control.nfade_g_from = 0;
            g_field_control.nfade_b_from = 0;
        }
        break;

        case FADE_TYPE_DIS_GRAD_SUB:
        {
            FadeSetDrawMode(0x2);
            FadeStepsDiscrease();
            FadeSetPolyRGBGradual(0x100 - g_field_control.fade_r, 0x100 - g_field_control.fade_g, 0x100 - g_field_control.fade_b);
        }
        break;

        case FADE_TYPE_INC_GRAD_SUB:
        {
            FadeSetDrawMode(0x2);
            FadeStepsIncrease();
            FadeSetPolyRGBGradual(0x100 - g_field_control.fade_r, 0x100 - g_field_control.fade_g, 0x100 - g_field_control.fade_b);
        }
        break;

        case FADE_TYPE_BG_SLOW_SUB:
        {
            g_field_control.fade_steps += 0x1;

            if (g_field_control.fade_steps == 0x22)
            {
                g_field_control.fade_type = FADE_TYPE_NONE;
            }
            else
            {
                FadeSetDrawMode(0x2);
                FadeSetPolyMonochrome(0x10);
            }
        }
        break;

        case FADE_TYPE_DISABLE_RENDER:
        {
            g_field_control.disable_render = 0x1;
        }
        break;

        case FADE_TYPE_DIS_GRAD_ADD:
        {
            FadeSetDrawMode(0x1);
            FadeStepsDiscrease();
            FadeSetPolyRGBGradual(g_field_control.fade_r, g_field_control.fade_g, g_field_control.fade_b);
        }
        break;

        case FADE_TYPE_INC_GRAD_ADD:
        {
            g_field_control.disable_render = 0;
            FadeSetDrawMode(0x1);
            FadeStepsIncrease();
            FadeSetPolyRGBGradual(g_field_control.fade_r, g_field_control.fade_g, g_field_control.fade_b);
        }
        break;

        case FADE_TYPE_DIS_DIR_SUB:
        {
            FadeSetDrawMode(0x2);
            FadeStepsDiscrease();
            FadeSetPolyRGBDirect(0x100 - g_field_control.fade_r, 0x100 - g_field_control.fade_g, 0x100 - g_field_control.fade_b);
        }
        break;

        case FADE_TYPE_INC_DIR_SUB:
        {
            FadeSetDrawMode(0x2);
            FadeStepsIncrease();
            FadeSetPolyRGBDirect(0x100 - g_field_control.fade_r, 0x100 - g_field_control.fade_g, 0x100 - g_field_control.fade_b);
        }
        break;

        case FADE_TYPE_DIS_DIR_ADD:
        {
            FadeSetDrawMode(0x1);
            FadeStepsDiscrease();
            FadeSetPolyRGBDirect(g_field_control.fade_r, g_field_control.fade_g, g_field_control.fade_b); // immediate
        }
        break;

        case FADE_TYPE_INC_DIR_ADD:
        {
            g_field_control.disable_render = 0;
            FadeSetDrawMode(0x1);
            FadeStepsIncrease();
            FadeSetPolyRGBDirect(g_field_control.fade_r, g_field_control.fade_g, g_field_control.fade_b); // immediate
        }
        break;

        case FADE_TYPE_ADD_INTERPOLATE:
        {
            FadeSetDrawMode(0x1);
            FadeSetPolyRGBInterpolate();
        }
        break;

        case FADE_TYPE_SUB_INTERPOLATE:
        {
            FadeSetDrawMode(0x2);
            FadeSetPolyRGBInterpolate();
        }
        break;

        case FADE_TYPE_BG_FAST_SUB:
        {
            g_field_control.fade_steps += 0x1;

            if (g_field_control.fade_steps == 0x12)
            {
                g_field_control.fade_type = FADE_TYPE_NONE;
            }
            else
            {
                FadeSetDrawMode(0x2);
                FadeSetPolyMonochrome(0x20);
            }
        }
        break;
    }
}
