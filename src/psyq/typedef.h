#pragma once

#include <cstdint>
#include <vector>



typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;

u8 READ_LE_U8( const void* ptr );
s8 READ_LE_S8( const void* ptr );
u16 READ_LE_U16( const void* ptr );
s16 READ_LE_S16( const void* ptr );
u32 READ_LE_U32( const void* ptr );
s32 READ_LE_S32( const void* ptr );
u8 READ_LE_U8( const std::vector< u8 >::const_iterator& input );
s8 READ_LE_S8( const std::vector< u8 >::const_iterator& input );
u16 READ_LE_U16( const std::vector< u8 >::const_iterator& input );
s16 READ_LE_S16( const std::vector< u8 >::const_iterator& input );
u32 READ_LE_U32( const std::vector< u8 >::const_iterator& input );
s32 READ_LE_S32( const std::vector< u8 >::const_iterator& input );

struct DVECTOR
{
    DVECTOR() : vx( 0 ), vy( 0 ) {};
    DVECTOR( s16 x, s16 y ) : vx( x ), vy( y ) {};

    s16 vx, vy;

    s32 asS32()
    {
        u32 temp = 0;
        temp = vy;
        temp <<= 16;
        temp |= ((u32)vx) & 0xffff;
        return temp;
    }

    static DVECTOR fromS32( s32 input )
    {
        DVECTOR temp;
        temp.vx = input & 0xffff;
        input >>= 16;
        temp.vy = input & 0xffff;
        return temp;
    }
};

struct sFixedPoint
{
    s32 value;

    operator const s32&() const { return value; }
    operator s32& () { return value; }
    sFixedPoint& operator=( s32 new_val ) { value = new_val; return *this; }
    s32 getIntegerPart() { return value >> 16; }

    static sFixedPoint fromValue( s32 new_val )
    {
        sFixedPoint temp;
        temp.value = new_val;
        return temp;
    }
};


struct VECTOR
{
    sFixedPoint vx;
    sFixedPoint vy;
    sFixedPoint vz;
    s32 pad;

    sFixedPoint operator[]( size_t id ) const
    {
        switch( id )
        {
            case 0: return vx;
            case 1: return vy;
            case 2: return vz;
            default: return sFixedPoint::fromValue( 0 );
        }
    }

    sFixedPoint& operator[]( size_t id )
    {
        switch( id )
        {
        case 0: return vx;
        case 1: return vy;
        case 2: return vz;
        default: return vx;
        }
    }
};

struct SVECTOR
{
    s16 vx, vy, vz;
    s16 pad;

    SVECTOR& operator+=( const SVECTOR& rhs )
    {
        vx += rhs.vx;
        vy += rhs.vy;
        vz += rhs.vz;
        return *this;
    }

    SVECTOR& operator-=( const SVECTOR& rhs )
    {
        vx -= rhs.vx;
        vy -= rhs.vy;
        vz -= rhs.vz;
        return *this;
    }

    s16 operator[]( size_t id ) const
    {
        switch( id )
        {
            case 0: return vx;
            case 1: return vy;
            case 2: return vz;
            default: return pad;
        }
    }

    s16& operator[]( size_t id )
    {
        switch( id )
        {
            case 0: return vx;
            case 1: return vy;
            case 2: return vz;
            case 3: return pad;
            default: return pad;
        }
    }
};

struct MATRIX
{
    s16 m[ 3 ][ 3 ]; // 3 x 3 matrix coefficient value
    s32 t[ 3 ]; // Parallel transfer volume
};
