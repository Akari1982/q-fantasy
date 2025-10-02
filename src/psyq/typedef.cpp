#include "typedef.h"



u8 READ_LE_U8( const void* ptr )
{
    return *(u8*)ptr;
}

s8 READ_LE_S8( const void* ptr )
{
    return *(s8*)ptr;
}

u16 READ_LE_U16( const void* ptr )
{
    return *(u16*)ptr;
}

s16 READ_LE_S16( const void* ptr )
{
    return *(s16*)ptr;
}

u32 READ_LE_U32( const void* ptr )
{
    return *(u32*)ptr;
}

s32 READ_LE_S32( const void* ptr )
{
    return *(s32*)ptr;
}

u8 READ_LE_U8( const std::vector< u8 >::const_iterator& input )
{
    return *input;
}

s8 READ_LE_S8( const std::vector< u8 >::const_iterator& input )
{
    return *(s8*)&input[ 0 ];
}

u16 READ_LE_U16( const std::vector< u8 >::const_iterator& input )
{
    return READ_LE_U16( &input[ 0 ] );
}

s16 READ_LE_S16( const std::vector< u8 >::const_iterator& input )
{
    return READ_LE_S16( &input[ 0 ] );
}

u32 READ_LE_U32( const std::vector< u8 >::const_iterator& input )
{
    return READ_LE_U32( &input[ 0 ] );
}

s32 READ_LE_S32( const std::vector< u8 >::const_iterator& input )
{
    return READ_LE_S32( &input[ 0 ] );
}

u8 READ_LE_U8( const std::span<u8>::iterator& input )
{
    return *input;
}

s8 READ_LE_S8( const std::span<u8>::iterator& input )
{
    return *(s8*)&input[0];
}

u16 READ_LE_U16( const std::span<u8>::iterator& input )
{
    return READ_LE_U16( &input[0] );
}

s16 READ_LE_S16( const std::span<u8>::iterator& input )
{
    return READ_LE_S16( &input[0] );
}

u32 READ_LE_U32( const std::span<u8>::iterator& input )
{
    return READ_LE_U32( &input[0] );
}

s32 READ_LE_S32( const std::span<u8>::iterator& input )
{
    return READ_LE_S32( &input[0] );
}
