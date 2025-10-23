#pragma once

#include "field.h"
#include "psyq/typedef.h"
#include "psyq/libgpu.h"



struct Rain
{
    SVECTOR p1;
    SVECTOR p2;
    u16 wait;
    s16 seed;
    s16 z;
    bool render;
};



void FieldRainInit( FieldRenderData* data );
void FieldRainUpdate();
void FieldRainAddToRender( OTag* ot, LINE_F2* rain, MATRIX* m, DR_MODE* dm );
