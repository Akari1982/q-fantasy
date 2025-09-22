#pragma once

#include "psyq/typedef.h"

#include <string>



struct OpcodeArg
{
    enum type
    {
        UNKNOWN,
        U8,
        READ_MEMORY8,
        READ_MEMORY16,
        WRITE_MEMORY16,
        JUMP8
    } type = UNKNOWN;
    std::string name;
    u8 mem_block;
    u8 offset;
};

struct OpcodeInfo
{
    bool init = false;
    u8 size = 1;
    std::string short_name;
    std::string full_name;
    std::string desc;
    bool can_break = false;
    std::vector<OpcodeArg> args;

    void end()
    {
        init = true;
    }

    OpcodeInfo& set_size( u8 _size )
    {
        size = _size;
        return *this;
    }

    OpcodeInfo& set_short_name( const char* name )
    {
        short_name = name;
        return *this;
    }

    OpcodeInfo& set_full_name( const char* name )
    {
        full_name = name;
        return *this;
    }

    OpcodeInfo& set_desc( const char* text )
    {
        desc = text;
        return *this;
    }

    OpcodeInfo& set_can_break()
    {
        can_break = true;
        return *this;
    }

    OpcodeInfo& add_argument_u8( const char* arg_name, u8 offset )
    {
        args.push_back( OpcodeArg() );
        OpcodeArg& new_arg = args[args.size() - 1];
        new_arg.name = arg_name;
        new_arg.type = OpcodeArg::U8;
        new_arg.offset = offset;
        return *this;
    }

    OpcodeInfo& add_argument_read_memory8( const char* arg_name, u8 block, u8 offset )
    {
        args.push_back( OpcodeArg() );
        OpcodeArg& new_arg = args[args.size() - 1];
        new_arg.name = arg_name;
        new_arg.type = OpcodeArg::READ_MEMORY8;
        new_arg.mem_block = block;
        new_arg.offset = offset;
        return *this;
    }

    OpcodeInfo& add_argument_read_memory16( const char* arg_name, u8 block, u8 offset )
    {
        args.push_back( OpcodeArg() );
        OpcodeArg& new_arg = args[args.size() - 1];
        new_arg.name = arg_name;
        new_arg.type = OpcodeArg::READ_MEMORY16;
        new_arg.mem_block = block;
        new_arg.offset = offset;
        return *this;
    }

    OpcodeInfo& add_argument_write_memory16( const char* arg_name, u8 block, u8 offset )
    {
        args.push_back( OpcodeArg() );
        OpcodeArg& new_arg = args[args.size() - 1];
        new_arg.name = arg_name;
        new_arg.type = OpcodeArg::WRITE_MEMORY16;
        new_arg.mem_block = block;
        new_arg.offset = offset;
        return *this;
    }

    OpcodeInfo& add_argument_jump8( const char* arg_name, u8 offset )
    {
        args.push_back( OpcodeArg() );
        OpcodeArg& new_arg = args[args.size() - 1];
        new_arg.name = arg_name;
        new_arg.type = OpcodeArg::JUMP8;
        new_arg.offset = offset;
        return *this;
    }
};



void FieldBrowserInitOpcode();



extern std::array<OpcodeInfo, 256> g_field_opcodes;
