#pragma once

#include "psyq/typedef.h"
#include "psyq/libgpu.h"



struct BgAnim
{
    u8 anim_id;
    u8 frame_id;
};

struct FieldRenderData;



void FieldBackgroundInitPoly( SPRT_16* p1, SPRT* p2, BgAnim* bg_anim, DR_MODE* dm );
void FieldBackgroundAddToRender( FieldRenderData& render_data );
