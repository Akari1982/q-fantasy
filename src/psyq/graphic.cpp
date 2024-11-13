#include "graphic.h"



sTag*
ClearOTagR( sTag* ot, int n )
{
    sTag* pCurrent = ot;
    for( int i = 0; i < n - 1; ++i )
    {
        ++pCurrent;
        pCurrent->m0_pNext = pCurrent - 1;
        pCurrent->m3_size = 0;
    }

    TermPrim( ot );

    return ot;
}



void
DrawOTag( sTag* ot )
{
    while( ot )
    {
        ot->execute();
        ot = ot->m0_pNext;
    }
}



void
AddPrim( sTag* ot, sTag* p )
{
    p->m0_pNext = ot->m0_pNext;
    ot->m0_pNext = p;
}
