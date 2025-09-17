#include "filesystem.h"

#include "ofMain.h"
#include "zlib.h"



void LZSExtract( std::vector<u8>& input, std::vector<u8>& output )
{
    u32 input_length = READ_LE_U32( &input[ 0 ] );

    u32 extract_size = input_length + 0x100;
    output.resize( extract_size );

    int input_offset = 4;
    int output_offset = 0;
    u8 control_byte = 0;
    u8 control_bit = 0;

    while( input_offset < input_length )
    {
        if( control_bit == 0 )
        {
            control_byte = input[ input_offset++ ];
            control_bit = 8;
        }

        if( control_byte & 1 )
        {
            output[ output_offset++ ] = input[ input_offset++ ];

            if( output_offset == extract_size )
            {
                extract_size += 0x100;
                output.resize( extract_size );
            }
        }
        else
        {
            u8 reference1 = input[ input_offset++ ];
            u8 reference2 = input[ input_offset++ ];

            u16 reference_offset = reference1 | ((reference2 & 0xf0) << 4);
            u8 reference_length = (reference2 & 0x0f) + 3;

            int real_offset = output_offset - ((output_offset - 0x12 - reference_offset) & 0xfff);

            for( int j = 0; j < reference_length; ++j )
            {
                if( real_offset < 0 )
                {
                    output[ output_offset++ ] = 0;
                }
                else
                {
                    output[ output_offset++ ] = output[ real_offset ];
                }

                if( output_offset == extract_size )
                {
                    extract_size += 0x100;
                    output.resize( extract_size );
                }

                ++real_offset;
            }
        }

        control_byte >>= 1;
        control_bit--;
    }

    output.resize( output_offset );
}



bool GZIPExtract( std::vector<u8>& input, std::vector<u8>& output )
{
    // Setup z_stream for gzip unpack
    z_stream strm{};
    strm.next_in   = input.data();
    strm.avail_in  = (u32)input.size();

    // 16+MAX_WBITS - gzip support
    if( inflateInit2( &strm, 16 + MAX_WBITS ) != Z_OK )
    {
        printf( "Can't initialize zlib.\n" );
        return false;
    }

    // Prepare buffer
    output.resize( 1024 * 1024 ); // set up for 1Mb
    strm.next_out = output.data();
    strm.avail_out = (u32)output.size();

    int ret;
    while( ( ret = inflate( &strm, Z_NO_FLUSH ) ) == Z_OK )
    {
        if( strm.avail_out == 0 )
        {
            // increase buffer size
            size_t oldSize = output.size();
            output.resize( oldSize * 2 );
            strm.next_out  = output.data() + oldSize;
            strm.avail_out = (u32)oldSize;
        }
    }

    if( ret != Z_STREAM_END )
    {
        inflateEnd( &strm );
        printf( "Unpacking error (code %d).\n", ret );
        return false;
    }

    output.resize( strm.total_out );

    inflateEnd( &strm );

    return true;
}



void FileLZS( const std::string& name, std::vector<u8>& output )
{
    std::vector<u8> temp_dat;
    FileRead( name, temp_dat );
    LZSExtract( temp_dat, output );

    //WriteFile( name + "_orig", temp_dat );
    //WriteFile( name + "_extr", output );
}



void FileBINGZIP( const std::string& name, std::vector<u8>& output )
{
    std::vector<u8> temp_dat;
    FileRead( name, temp_dat );
    // remove first 0x8 byte (they are header data to clean working memory for executable)
    if( temp_dat.size() >= 0x8 ) temp_dat.erase( temp_dat.begin(), temp_dat.begin() + 0x8 );
    GZIPExtract( temp_dat, output );
}



void FileRead( const std::string& name, std::vector<u8>& output )
{
    printf( "Load file: %s\n", name.c_str() );

    FILE* file = fopen( ("data/" + name).c_str(), "rb" );

    if( file == NULL )
    {
        printf( "Cant read file: %s\n", name.c_str() );
        return;
    }

    fseek( file, 0, SEEK_END );
    u32 size = ftell( file );
    output.resize( size );

    fseek( file, 0, SEEK_SET );
    fread( &output[ 0 ], 1, size, file );
    fclose( file );
}



void FileWrite( const std::string& name, std::vector<u8>& input )
{
    printf( "Write file: %s\n", name.c_str() );

    FILE* file = fopen( name.c_str(), "ab" );

    if( file == NULL )
    {
        printf( "Cant write file: %s\n", name.c_str() );
        return;
    }

    fwrite( &input[ 0 ], 1, input.size(), file );
    fclose( file );
}
