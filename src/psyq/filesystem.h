#pragma once

#include "typedef.h"

#include <string>
#include <vector>



void ReadFileLZS( const std::string& name, std::vector<u8>& output );
void ReadFile( const std::string& name, std::vector<u8>& output );
void WriteFile( const std::string& name, std::vector<u8>& input );
void LzsExtract( std::vector<u8>& input, std::vector<u8>& output );
