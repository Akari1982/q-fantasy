#include "psxgte.h"
#include <array>



std::array< MATRIX, 20 > matrix_stack;
int matrix_stack_id = 0;



void InitGeom()
{
    setCopControlWord( 2, 0xe800, 0x155 ); // ZSF3
    setCopControlWord( 2, 0xf000, 0x100 ); // ZSF4
    setCopControlWord( 2, 0xd000, 1000 ); // H
    setCopControlWord( 2, 0xd800, 0xffffef9e ); // DQA
    setCopControlWord( 2, 0xe000, 0x1400000 ); // DQB
    setCopControlWord( 2, 0xc000, 0 ); // OFX
    setCopControlWord( 2, 0xc800, 0 ); // OFY
}



void PushMatrix()
{
    matrix_stack[ matrix_stack_id ].m[ 0 ][ 0 ] = DVECTOR::fromS32( getCopControlWord( 2, 0x0000 ) ).vx;
    matrix_stack[ matrix_stack_id ].m[ 0 ][ 1 ] = DVECTOR::fromS32( getCopControlWord( 2, 0x0000 ) ).vy;
    matrix_stack[ matrix_stack_id ].m[ 0 ][ 2 ] = DVECTOR::fromS32( getCopControlWord( 2, 0x0800 ) ).vx;
    matrix_stack[ matrix_stack_id ].m[ 1 ][ 0 ] = DVECTOR::fromS32( getCopControlWord( 2, 0x0800 ) ).vy;
    matrix_stack[ matrix_stack_id ].m[ 1 ][ 1 ] = DVECTOR::fromS32( getCopControlWord( 2, 0x1000 ) ).vx;
    matrix_stack[ matrix_stack_id ].m[ 1 ][ 2 ] = DVECTOR::fromS32( getCopControlWord( 2, 0x1000 ) ).vy;
    matrix_stack[ matrix_stack_id ].m[ 2 ][ 0 ] = DVECTOR::fromS32( getCopControlWord( 2, 0x1800 ) ).vx;
    matrix_stack[ matrix_stack_id ].m[ 2 ][ 1 ] = DVECTOR::fromS32( getCopControlWord( 2, 0x1800 ) ).vy;
    matrix_stack[ matrix_stack_id ].m[ 2 ][ 2 ] = DVECTOR::fromS32( getCopControlWord( 2, 0x2000 ) ).vx;

    matrix_stack[ matrix_stack_id ].t[ 0 ] = getCopControlWord( 2, 0x2800 );
    matrix_stack[ matrix_stack_id ].t[ 1 ] = getCopControlWord( 2, 0x3000 );
    matrix_stack[ matrix_stack_id ].t[ 2 ] = getCopControlWord( 2, 0x3800 );

    matrix_stack_id++;
}



void PopMatrix()
{
    const MATRIX* m = &matrix_stack[ matrix_stack_id ];
    setCopControlWord( 2, 0x0000, DVECTOR( m->m[ 0 ][ 0 ], m->m[ 0 ][ 1 ] ) );
    setCopControlWord( 2, 0x0800, DVECTOR( m->m[ 0 ][ 2 ], m->m[ 1 ][ 0 ] ) );
    setCopControlWord( 2, 0x1000, DVECTOR( m->m[ 1 ][ 1 ], m->m[ 1 ][ 2 ] ) );
    setCopControlWord( 2, 0x1800, DVECTOR( m->m[ 2 ][ 0 ], m->m[ 2 ][ 1 ] ) );
    setCopControlWord( 2, 0x2000, DVECTOR( m->m[ 2 ][ 2 ], 0 ) );
    setCopControlWord( 2, 0x2800, m->t[ 0 ] );
    setCopControlWord( 2, 0x3000, m->t[ 1 ] );
    setCopControlWord( 2, 0x3800, m->t[ 2 ] );
    matrix_stack_id--;
}



void SetRotMatrix( const MATRIX* m )
{
    setCopControlWord( 2, 0x0000, DVECTOR( m->m[ 0 ][ 0 ], m->m[ 0 ][ 1 ] ) );
    setCopControlWord( 2, 0x0800, DVECTOR( m->m[ 0 ][ 2 ], m->m[ 1 ][ 0 ] ) );
    setCopControlWord( 2, 0x1000, DVECTOR( m->m[ 1 ][ 1 ], m->m[ 1 ][ 2 ] ) );
    setCopControlWord( 2, 0x1800, DVECTOR( m->m[ 2 ][ 0 ], m->m[ 2 ][ 1 ] ) );
    setCopControlWord( 2, 0x2000, DVECTOR( m->m[ 2 ][ 2 ], 0 ) );
}



void SetTransMatrix( const MATRIX* m )
{
    setCopControlWord( 2, 0x2800, m->t[ 0 ] );
    setCopControlWord( 2, 0x3000, m->t[ 1 ] );
    setCopControlWord( 2, 0x3800, m->t[ 2 ] );
}



void SetGeomScreen( s32 h )
{
    setCopControlWord( 2, 0xd000, h );
}



s32 RotTransPers( SVECTOR* v0, DVECTOR* sxy, u32* p, u32* flag )
{
    setCopReg( 2, 0x0000, DVECTOR( v0->vx, v0->vy ) );
    setCopReg( 2, 0x0800, DVECTOR( v0->vz, 0 ) );
    copFunction( 2, 0x180001 );
    *sxy = DVECTOR::fromS32( getCopReg( 2, 0xe ) );
    *p = getCopReg(2, 8);
    *flag = getCopControlWord( 2, 0xf800 );;
    return getCopReg( 2, 0x9800 ) >> 2;
}
