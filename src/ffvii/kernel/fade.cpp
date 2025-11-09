#include "fade.h"



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
