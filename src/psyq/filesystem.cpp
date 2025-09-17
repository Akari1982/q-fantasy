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



bool GZIPExtract( const std::string& inputPath, const std::string& outputPath )
{
    // 1. Открыть входной файл побайтово
    FILE* fIn = fopen( inputPath.c_str(), "rb");
    if (!fIn) {
        printf( "Can't open file: %s\n", inputPath.c_str() );
        return false;
    }

    // 2. Считать весь файл в память
    fseek(fIn, 0, SEEK_END);
    long inSize = ftell(fIn);
    fseek(fIn, 0, SEEK_SET);

    std::vector<unsigned char> inBuffer(inSize);
    if (fread(inBuffer.data(), 1, inSize, fIn) != (size_t)inSize) {
        fclose(fIn);
        ofLogError("extractGzipFile") << "Ошибка чтения файла";
        return false;
    }
    fclose(fIn);

    // 3. Настроить z_stream для распаковки gzip
    z_stream strm{};
    strm.next_in   = inBuffer.data();
    strm.avail_in  = (uInt)inBuffer.size();

    if (inflateInit2(&strm, 16 + MAX_WBITS) != Z_OK) { // 16+MAX_WBITS → поддержка gzip
        ofLogError("extractGzipFile") << "Не удалось инициализировать zlib";
        return false;
    }

    // 4. Подготовить буфер для результата
    std::vector<unsigned char> outBuffer;
    outBuffer.resize(1024 * 1024); // начальный размер 1 MB
    strm.next_out  = outBuffer.data();
    strm.avail_out = (uInt)outBuffer.size();

    int ret;
    while ((ret = inflate(&strm, Z_NO_FLUSH)) == Z_OK) {
        if (strm.avail_out == 0) {
            // увеличиваем буфер
            size_t oldSize = outBuffer.size();
            outBuffer.resize(oldSize * 2);
            strm.next_out  = outBuffer.data() + oldSize;
            strm.avail_out = (uInt)oldSize;
        }
    }

    if (ret != Z_STREAM_END) {
        inflateEnd(&strm);
        ofLogError("extractGzipFile") << "Ошибка при распаковке (код " << ret << ")";
        return false;
    }

    size_t outSize = strm.total_out;
    inflateEnd(&strm);

    // 5. Записать результат на диск
    FILE* fOut = fopen(ofToDataPath(outputPath, true).c_str(), "wb");
    if (!fOut) {
        ofLogError("extractGzipFile") << "Не удалось открыть выходной файл " << outputPath;
        return false;
    }

    fwrite(outBuffer.data(), 1, outSize, fOut);
    fclose(fOut);

    ofLogNotice("extractGzipFile") << "Распаковано " << inSize << " → " << outSize << " байт";
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
    GZIPExtract( std::string( "CHOCOBO.BIN" ), std::string( "CHOCOBO.BIN_u" ) );
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
