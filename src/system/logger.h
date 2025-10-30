#pragma once

#include <string>



#define LOG_ERROR( text, ... ) printf( (std::string("[ERROR]: ") + text + "\n").c_str(), __VA_ARGS__ );
#define LOG_WARNING( text, ... ) printf( (std::string("[WARNING]: ") + text + "\n").c_str(), __VA_ARGS__ );
#define LOG_INFO( text, ... ) printf( (std::string("") + text + "\n").c_str(), __VA_ARGS__ );
