#include "tim.h"
#include "libgpu.h"



RECT l_tim_crect;
RECT l_tim_prect;
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
        std::vector<u8>::const_iterator timaddr = input + 8;
        timimg->mode = READ_LE_U32( input + 4 );

//        if( system_psyq_get_graph_debug() == 0x2 )
//        {
//            system_bios_printf( "id  =%08x", 0x10 );
//            system_bios_printf( "mode=%08x", timimg->mode );
//            system_bios_printf( "timaddr=%08x", texture );
//        }

        int clut_len = 0;

        if( timimg->mode & 0x8 )
        {
            l_tim_crect.x = READ_LE_S16( input + 0xc + 0 );
            l_tim_crect.y = READ_LE_S16( input + 0xc + 2 );
            l_tim_crect.w = READ_LE_S16( input + 0xc + 4 );
            l_tim_crect.h = READ_LE_S16( input + 0xc + 6 );
            timimg->crect = &l_tim_crect;
            timimg->caddr = &(*(input + 0x14));
            clut_len = READ_LE_U32( timaddr ) >> 2;
            timaddr += READ_LE_U32( timaddr ) & ~3;
        }
        else
        {
            timimg->crect = nullptr;
            timimg->caddr = nullptr;
        }

        l_tim_prect.x = READ_LE_S16( timaddr + 4 + 0 );
        l_tim_prect.y = READ_LE_S16( timaddr + 4 + 2 );
        l_tim_prect.w = READ_LE_S16( timaddr + 4 + 4 );
        l_tim_prect.h = READ_LE_S16( timaddr + 4 + 6 );
        timimg->prect = &l_tim_prect;
        timimg->paddr = &(*(timaddr + 0xc));
        return clut_len + (READ_LE_U32( timaddr ) / 4) + 2;
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



void loadTimToVram( std::vector<u8>::const_iterator ptr, short imageX, short imageY, short palX, short palY, short palW, short palH )
{
    OpenTIM(ptr);

    TIM_IMAGE timImageData;
    if (ReadTIM(&timImageData))
    {
        if (timImageData.caddr)
        {
            if (palY != -1)
            {
                timImageData.crect->x = palX;
                timImageData.crect->y = palY;
            }
            if (palW != 0)
            {
                timImageData.crect->w = palW;
            }
            if (palH != 0)
            {
                timImageData.crect->h = palH;
            }
            LoadImage(timImageData.crect, (u8*)timImageData.caddr);
        }
        if (timImageData.paddr)
        {
            timImageData.prect->x = imageX;
        }
        // huh? this seems buggy in the original game, as this should have been in the above test
        timImageData.prect->y = imageY;
        LoadImage(timImageData.prect, (u8*)timImageData.paddr);
    }
}



void LoadImage( RECT* pRect, const u8* data )
{
    int vramOffset = pRect->y * 2048 + pRect->x * 2;
    for (int y = 0; y < pRect->h; y++)
    {
        auto vramIterator = g_vram.begin() + vramOffset;
        for (int x = 0; x < pRect->w * 2; x++)
        {
            *(vramIterator++) = *(data++);
            vramOffset++;
        }
        vramOffset += 2048 - pRect->w * 2;
    }
}



void LoadImage( RECT* pRect, std::span<u16>::iterator data )
{
    LoadImage(pRect, (u8*)&data[0]);
}



void LoadImage( RECT* pRect, std::span<u8>::iterator data )
{
    for (int y = 0; y < pRect->h; y++)
    {
        auto vramIterator = g_vram.begin() + (pRect->y + y) * 2048 + pRect->x * 2;

        for (int x = 0; x < pRect->w * 2; x++)
        {
            *vramIterator = READ_LE_U8(data);
            vramIterator++;
            data = data + 1;
        }
    }
}



void StoreImage( RECT* pRect, std::vector<u16>& output )
{
    std::vector<u16>::iterator p = output.begin();
    for (int y = 0; y < pRect->h; y++)
    {
        auto vramIterator = g_vram.begin() + (pRect->y + y) * 2048 + pRect->x * 2;

        for (int x = 0; x < pRect->w; x++)
        {
            *p = READ_LE_U16(&vramIterator[0]);
            p++;
            vramIterator += 2;
        }
    }
}
