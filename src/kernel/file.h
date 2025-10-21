#pragma once

#include "psyq/typedef.h"

#include <string>
#include <vector>



u32 FileGetPackPointer( std::vector<u8>& file, u32 id );

void GZIPSetDataBlock( std::vector<u8>& data );
u16 GZIPGetType();
u16 GZIPGetSize();
void GZIPPackDecompressNextBlock( std::vector<u8>& output );

void FileLZS( const std::string& name, std::vector<u8>& output );
void FileLZSExtract( std::vector<u8>& input, std::vector<u8>& output );
void FileBINGZIP( const std::string& name, std::vector<u8>& output );
void FileRead( const std::string& name, std::vector<u8>& output );
void FileWrite( const std::string& name, std::vector<u8>& input );
