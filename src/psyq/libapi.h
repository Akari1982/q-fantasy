#pragma once

#include "typedef.h"

#define DescRC      0xf2000000
#define RCntCNT2    (DescRC|0x02)

#define EvSpINT     0x0002

#define RCntMdINTR  0x1000

typedef void(*eventFunc)();

s32 PsyqOpenEvent( u32 desc, u32 spec, u32 mode, eventFunc func = nullptr );
s32 PsyqCloseEvent( u32 event );
s32 PsyqEnableEvent( u32 event );

s32 PsyqSetRCnt( u32 spec, u16 target, u32 mode );
s32 PsyqStartRCnt( u32 spec );
s32 PsyqStopRCnt( u32 spec );

