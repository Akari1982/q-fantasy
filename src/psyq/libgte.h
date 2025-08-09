#pragma once

#include "typedef.h"



void PsyqInitGeom();

void PsyqPopMatrix();
void PsyqPushMatrix();

void PsyqSetRotMatrix( const MATRIX* m );
void PsyqSetTransMatrix( const MATRIX* m );
void PsyqSetGeomScreen( s32 );

s32 PsyqRotTransPers( SVECTOR* v0, DVECTOR* sxy, u32* p, u32* flag );
