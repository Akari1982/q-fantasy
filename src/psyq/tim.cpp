#include "tim.h"
#include "libgpu.h"



std::vector<u8>::const_iterator l_current_tim;



int PsyqGpuOpenTim( std::vector<u8>::const_iterator ptr )
{
    l_current_tim = ptr;
    return 0;
}



int GetTimData( std::vector<u8>::const_iterator input, TIM_IMAGE* timimg )
{
    if( READ_LE_U32( input ) == 0x10 )
    {
        std::vector<u8>::const_iterator timaddr = input + 0x8;
        timimg->mode = READ_LE_U32( input + 0x4 );

        int clut_len = 0;

        if( timimg->mode & 0x8 )
        {
            timimg->crect.x = READ_LE_S16( input + 0xc + 0x0 );
            timimg->crect.y = READ_LE_S16( input + 0xc + 0x2 );
            timimg->crect.w = READ_LE_S16( input + 0xc + 0x4 );
            timimg->crect.h = READ_LE_S16( input + 0xc + 0x6 );
            timimg->caddr = &(*(input + 0x14));
            clut_len = READ_LE_U32( timaddr ) / 4;
            timaddr += READ_LE_U32( timaddr ) & ~0x3;
        }

        timimg->prect.x = READ_LE_S16( timaddr + 0x4 + 0x0 );
        timimg->prect.y = READ_LE_S16( timaddr + 0x4 + 0x2 );
        timimg->prect.w = READ_LE_S16( timaddr + 0x4 + 0x4 );
        timimg->prect.h = READ_LE_S16( timaddr + 0x4 + 0x6 );
        timimg->paddr = &(*(timaddr + 0xc));
        return clut_len + (READ_LE_U32( timaddr ) / 4) + 0x2;
    }
    else
    {
        return -1;
    }
}



TIM_IMAGE* PsyqGpuReadTim( TIM_IMAGE* timimg )
{
    int timOffset = GetTimData( l_current_tim, timimg );

    if( timOffset != -1 )
    {
        l_current_tim += timOffset * 4;
        return timimg;
    }

    return nullptr;
}



void PsyqGpuLoadImage( SRECT* rect, std::span<u8>::iterator data )
{
    for( int y = 0; y < rect->h; ++y )
    {
        auto vram = g_vram.begin() + (rect->y + y) * 2048 + rect->x * 2;

        for( int x = 0; x < (rect->w * 2); ++x )
        {
            *vram = READ_LE_U8( data );
            vram += 1;
            data += 1;
        }
    }
}
