#pragma once

#include "typedef.h"

#include <string>
#include <vector>



void FileLZS( const std::string& name, std::vector<u8>& output );
void FileBINGZIP( const std::string& name, std::vector<u8>& output );
void FileRead( const std::string& name, std::vector<u8>& output );
void FileWrite( const std::string& name, std::vector<u8>& input );
