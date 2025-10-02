#pragma once

#include "typedef.h"
#include <span>



struct TIM_IMAGE
{
    u32 mode;
    SRECT crect;
    const void* caddr;
    SRECT prect;
    const void* paddr;
};



int PsyqGpuOpenTim( std::vector<u8>::const_iterator ptr );
TIM_IMAGE* PsyqGpuReadTim( TIM_IMAGE* timimg );

void PsyqGpuLoadImage( SRECT* rect, std::span<u8>::iterator data );
