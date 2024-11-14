#pragma once

#include "typedef.h"



void InitGeom();

void PopMatrix();
void PushMatrix();

void SetRotMatrix( const MATRIX* m );
void SetTransMatrix( const MATRIX* m );
void SetGeomScreen( s32 );

s32 RotTransPers( SVECTOR* v0, DVECTOR* sxy, u32* p, u32* flag );
