#include "browser_field.h"
#include "browser_field_opcodes.h"
#include "system/icons_font.h"
#include "psyq/filesystem.h"
#include "field/field.h"
#include "field/field_list.h"

#include "ofxImGui.h"
#include <format>

bool g_browser_field = false;

std::vector<u8> l_dat_file;

void ParseEvents( u8 actor_id );



void BrowserField()
{
    if( g_browser_field == false ) return;

    static bool loaded = false;

    ImGui::SetNextWindowPos( ImGui::GetWindowPos() + ImVec2( 0, 0 ), ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowSize( ImVec2( 900, 650 ), ImGuiCond_Once );
    if( ImGui::Begin( "Field Browser", &g_browser_field, ImGuiWindowFlags_NoCollapse ) )
    {
        ImGui::Columns( 2, nullptr, true );
        // do not reset column width every frame
        static bool first_frame = true;
        if( first_frame )
        {
            ImGui::SetColumnWidth( 0, 250 );
            first_frame = false;
        }

        static int field_id = 0;

        if( ImGui::BeginListBox( "##FieldList", ImVec2( -FLT_MIN, -FLT_MIN ) ) )
        {
            ImGui::TextDisabled( "Fields Map List" );
            ImGui::Separator();

            for( int i = 0; i < 0x312; ++i )
            {
                ImGui::PushID( i );

                std::string name = g_field_files[i];
                bool corrupted = false;

                if( (name == "BLACKBGA")
                 || (name == "BLACKBGF")
                 || (name == "BLACKBGG")
                 || (name == "BLIN69_2")
                 || (name == "CONVIL_3")
                 || (name == "FALLP")
                 || (name == "FSHIP_26")
                 || (name == "HYOU14")
                 || (name == "JUNMON")
                 || (name == "M_ENDO")
                 || (name == "ONNA_1")
                 || (name == "ONNA_3")
                 || (name == "ONNA_6")
                 || (name == "PASS")
                 || (name == "Q_5")
                 || (name == "SUBIN_4")
                 || (name == "TRAP")
                 || (name == "WHITEBG1")
                 || (name == "WHITEBG2")
                 || (name.compare( 0, 2, "WM" ) == 0)
                 || (name == "XMVTES") )
                {
                    corrupted = true;
                    ImGui::BeginDisabled( true );
                }

                if( ImGui::Selectable( ("0x" + std::format( "{:03x} ", i ) + g_field_files[i] + ((corrupted) ? " (corrupted)": "")).c_str(), (i == field_id) ) )
                {
                    loaded = false;
                    field_id = i;
                }

                if( corrupted ) ImGui::EndDisabled();

                ImGui::PopID();
            }
        }
        ImGui::EndListBox();

        ImGui::NextColumn();
        ImGui::BeginChild( "right_panel", ImVec2( 0, 0 ), true );

        if( loaded == false )
        {
            FileLZS( "FIELD/" + g_field_files[field_id] + ".DAT", l_dat_file );
            loaded = true;
        }

        ImGui::Text( "Map: 0x%03x", field_id );

        u32 ate_addr = READ_LE_U32( &l_dat_file[0x0] ) - g_field_dat_base_addr;
        u8 actors_n = READ_LE_U8( &l_dat_file[ate_addr + 0x2] );
        u8 models_n = READ_LE_U8( &l_dat_file[ate_addr + 0x3] );
        u16 strings_ofs = READ_LE_U16( &l_dat_file[ate_addr + 0x4] );
        u16 scale = READ_LE_U16( &l_dat_file[ate_addr + 0x8] );

        ImGui::Text( "Event ver%d. Data ver%d", READ_LE_U8( &l_dat_file[ate_addr + 0x0] ), READ_LE_U8( &l_dat_file[ate_addr + 0x1] ) );
        ImGui::Text( "Entities: %d. Models: %d", actors_n, models_n );
        ImGui::Text( "Scale: 0x%04x", scale );
        ImGui::Text( "Authr: %s", &l_dat_file[ate_addr + 0x10] );
        ImGui::Text( "Event: %s", &l_dat_file[ate_addr + 0x18] );
        ImGui::Separator();

        for( int i = 0; i < actors_n; ++i )
        {
            ImGui::Text( "Actor %02d: %s", i, &l_dat_file[ate_addr + 0x20 + i * 0x8] );
        }
        ImGui::Separator();

        ParseEvents( 0x0 );

        ImGui::EndChild();

        ImGui::Columns( 1 );
    }
    ImGui::End();
}



void ArgGetU8( u32 arg_ofs )
{
    ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
    ImGui::Text( "0x%02x", READ_LE_U8( &l_dat_file[arg_ofs] ) );
    ImGui::PopStyleColor();
}



void ArgGetJump8( u32 arg_ofs )
{
    ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0, 1, 1 ) );
    ImGui::Text( "0x%02x", READ_LE_U8( &l_dat_file[arg_ofs] ) );
    ImGui::PopStyleColor();
}



void ArgGetMemory8( u8 mem_block, u32 op_ofs, u32 arg_ofs )
{
    u8 bank = 0;
    switch( mem_block )
    {
        case 0x1: bank = READ_LE_U8( &l_dat_file[op_ofs + 0x1] ) >> 0x4; break;
        case 0x2: bank = READ_LE_U8( &l_dat_file[op_ofs + 0x1] ) & 0xf; break;
        case 0x3: bank = READ_LE_U8( &l_dat_file[op_ofs + 0x2] ) >> 0x4; break;
        case 0x4: bank = READ_LE_U8( &l_dat_file[op_ofs + 0x2] ) & 0xf; break;
        case 0x5: bank = READ_LE_U8( &l_dat_file[op_ofs + 0x3] ) >> 0x4; break;
        case 0x6: bank = READ_LE_U8( &l_dat_file[op_ofs + 0x3] ) & 0xf; break;
    }

    if( bank == 0x0 )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
        ImGui::Text( "0x%02x", READ_LE_U8( &l_dat_file[arg_ofs] ) );
        ImGui::PopStyleColor();
    }
    else if( (bank == 0x1) || (bank == 0x2) )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 1, 0, 1 ) );
        ImGui::Text( "M0_1[0x%02x]", READ_LE_U8( &l_dat_file[arg_ofs] ) );
        ImGui::PopStyleColor();
    }
    else if( (bank == 0x3) || (bank == 0x3) )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 1, 0, 1 ) );
        ImGui::Text( "M1_1[0x%02x]", READ_LE_U8( &l_dat_file[arg_ofs] ) );
        ImGui::PopStyleColor();
    }
    else if( (bank == 0xb) || (bank == 0xc) )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 1, 0, 1 ) );
        ImGui::Text( "M2_1[0x%02x]", READ_LE_U8( &l_dat_file[arg_ofs] ) );
        ImGui::PopStyleColor();
    }
    else if( (bank == 0xd) || (bank == 0xe) )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 1, 0, 1 ) );
        ImGui::Text( "M3_1[0x%02x]", READ_LE_U8( &l_dat_file[arg_ofs] ) );
        ImGui::PopStyleColor();
    }
    else if( (bank == 0x7) || (bank == 0xf) )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 1, 0, 1 ) );
        ImGui::Text( "M4_1[0x%02x]", READ_LE_U8( &l_dat_file[arg_ofs] ) );
        ImGui::PopStyleColor();
    }
    else if( (bank == 0x5) || (bank == 0x6) )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 1, 0, 1 ) );
        ImGui::Text( "L_1[0x%02x]", READ_LE_U8( &l_dat_file[arg_ofs] ) );
        ImGui::PopStyleColor();
    }
}



void ArgGetMemory16( u8 mem_block, u32 op_ofs, u32 arg_ofs )
{
    u8 bank = 0;
    switch( mem_block )
    {
        case 0x1: bank = READ_LE_U8( &l_dat_file[op_ofs + 0x1] ) >> 0x4; break;
        case 0x2: bank = READ_LE_U8( &l_dat_file[op_ofs + 0x1] ) & 0xf; break;
        case 0x3: bank = READ_LE_U8( &l_dat_file[op_ofs + 0x2] ) >> 0x4; break;
        case 0x4: bank = READ_LE_U8( &l_dat_file[op_ofs + 0x2] ) & 0xf; break;
        case 0x5: bank = READ_LE_U8( &l_dat_file[op_ofs + 0x3] ) >> 0x4; break;
        case 0x6: bank = READ_LE_U8( &l_dat_file[op_ofs + 0x3] ) & 0xf; break;
    }

    if( bank == 0x0 )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
        ImGui::Text( "0x%04x", READ_LE_U16( &l_dat_file[arg_ofs] ) );
        ImGui::PopStyleColor();
    }
    else if( (bank == 0x1) || (bank == 0x2) )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 1, 0, 1 ) );
        ImGui::Text( "M0_2[0x%02x]", READ_LE_U8( &l_dat_file[arg_ofs] ) );
        ImGui::PopStyleColor();
    }
    else if( (bank == 0x3) || (bank == 0x3) )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 1, 0, 1 ) );
        ImGui::Text( "M1_2[0x%02x]", READ_LE_U8( &l_dat_file[arg_ofs] ) );
        ImGui::PopStyleColor();
    }
    else if( (bank == 0xb) || (bank == 0xc) )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 1, 0, 1 ) );
        ImGui::Text( "M2_2[0x%02x]", READ_LE_U8( &l_dat_file[arg_ofs] ) );
        ImGui::PopStyleColor();
    }
    else if( (bank == 0xd) || (bank == 0xe) )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 1, 0, 1 ) );
        ImGui::Text( "M3_2[0x%02x]", READ_LE_U8( &l_dat_file[arg_ofs] ) );
        ImGui::PopStyleColor();
    }
    else if( (bank == 0x7) || (bank == 0xf) )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 1, 0, 1 ) );
        ImGui::Text( "M4_2[0x%02x]", READ_LE_U8( &l_dat_file[arg_ofs] ) );
        ImGui::PopStyleColor();
    }
    else if( (bank == 0x5) || (bank == 0x6) )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 1, 0, 1 ) );
        ImGui::Text( "L_2[0x%02x]", READ_LE_U8( &l_dat_file[arg_ofs] ) );
        ImGui::PopStyleColor();
    }
}



void ParseEvents( u8 actor_id )
{
    u32 ate_addr = READ_LE_U32( &l_dat_file[0x0] ) - g_field_dat_base_addr;
    u8 actors_n = READ_LE_U8( &l_dat_file[ate_addr + 0x2] );
    u16 akao_n = READ_LE_S16( &l_dat_file[ate_addr + 0x6] );

    for( int i = 0; i < 0x20; ++i )
    {
        u32 event_ofs = READ_LE_U16( &l_dat_file[ate_addr + 0x20 + actors_n * 0x8 + akao_n * 0x4 + actor_id * 0x40 + i * 0x02] );
        ImGui::Text( "Event %02d: 0x%04x", i, event_ofs );

        u32 end_ofs = event_ofs;
        u8 opcode = READ_LE_U8( &l_dat_file[ate_addr + event_ofs] );

//        if (opcode == 0x00 && i != 0) // skip if script contain only RET opcode and this is not init script
//        {
//            continue;
//        }
//        if (prev_script == script)
//        {
//            continue;
//        }
//        prev_script = script;

        for (; event_ofs <= end_ofs;)
        {
            u8 opcode = READ_LE_U8( &l_dat_file[ate_addr + event_ofs] );

            ImGui::Text( "  0x%04x    ", event_ofs );
            ImGui::SameLine( 0, 0 );

            if( g_field_opcodes[opcode].init )
            {
                ImGui::Text( "%s(", g_field_opcodes[opcode].full_name.c_str() ); ImGui::SameLine( 0, 0 );

                for( int arg_id = 0; arg_id < g_field_opcodes[opcode].args.size(); ++arg_id )
                {
                    const OpcodeArg& argument = g_field_opcodes[opcode].args[arg_id];
                    const u32 arg_ofs = ate_addr + event_ofs + argument.offset;

                    if( arg_id != 0 ) ImGui::Text( "," ); ImGui::SameLine( 0, 0 );

                    ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                    ImGui::Text(" %s:", argument.name.c_str()); ImGui::SameLine(0, 0);
                    ImGui::PopStyleColor();

                    switch( argument.type )
                    {
                        case OpcodeArg::U8: ArgGetU8( arg_ofs ); break;
                        case OpcodeArg::MEMORY8: ArgGetMemory8( argument.mem_block, ate_addr + event_ofs, arg_ofs ); break;
                        case OpcodeArg::MEMORY16: ArgGetMemory16( argument.mem_block, ate_addr + event_ofs, arg_ofs ); break;

                        case OpcodeArg::JUMP8:
                        {
                            ArgGetJump8( arg_ofs );

                            u32 temp_end = event_ofs + READ_LE_U8( &l_dat_file[arg_ofs] ) - 1;
                            end_ofs = (temp_end > end_ofs) ? temp_end : end_ofs;
                        }
                        break;
                    }

                    ImGui::SameLine( 0, 0 );

                    if( (arg_id + 1) >= g_field_opcodes[opcode].args.size() ) ImGui::Text( " " ); ImGui::SameLine( 0, 0 );
                }
                ImGui::Text( ")" );

                event_ofs += g_field_opcodes[opcode].size;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;

                ImGui::SameLine( 0, 0 );
                ImGui::Text( " // end:0x%04x", end_ofs );
            }
            else
            {
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0, 0, 1 ) );
                ImGui::Text( "Failed to decode opcode 0x%02X", opcode );
                ImGui::PopStyleColor();
                break;
            }
        }
    }

/*


            export_script->Log(OffsetString(script) + " (end " + OffsetString(end) + "): ");

            u8 opcode = GetU8(script);

            if( opcode == 0x00 ) // RET
            {
                export_script->Log( "return\n" );

                if( i == 0 && one_ret_check == false ) // in init we need to meet ret at least once
                {
                    AdvanceScript( 1, script, end );
                    one_ret_check = true;
                }
                else
                {
                    script += 1;
                }
            }
            else if( opcode == 0x01 ) // REQ
            {
                u8 entity_id = GetU8( script + 1 );
                u8 priority  = GetU8( script + 2 ) >> 5;
                u8 script_id = GetU8( script + 2 ) & 0x1F;
                export_script->Log( "script:request( \"" + entity_list[ entity_id ] + "\", \"script_" + IntToString( script_id ) + "\", " + IntToString( priority ) + " )\n" );
                AdvanceScript( 3, script, end );
            }
            else if( opcode == 0x02 ) // REQSW
            {
                u8 entity_id = GetU8( script + 1 );
                u8 priority  = GetU8( script + 2 ) >> 5;
                u8 script_id = GetU8( script + 2 ) & 0x1F;
                export_script->Log( "script:request_start_sync( \"" + entity_list[ entity_id ] + "\", \"script_" + IntToString( script_id ) + "\", " + IntToString( priority ) + " )\n" );
                AdvanceScript( 3, script, end );
            }
            else if( opcode == 0x03 ) // REQEW
            {
                u8 entity_id = GetU8( script + 1 );
                u8 priority  = GetU8( script + 2 ) >> 5;
                u8 script_id = GetU8( script + 2 ) & 0x1F;
                export_script->Log( "script:request_end_sync( \"" + entity_list[ entity_id ] + "\", \"script_" + IntToString( script_id ) + "\", " + IntToString( priority ) + " )\n" );
                AdvanceScript( 3, script, end );
            }
            else if (opcode == 0x07) // RETTO
            {
                u8 priority  = GetU8(script + 1) >> 5;
                u8 script_id = GetU8(script + 1) & 0x1F;

                export_script->Log(
                    "return_to( \"script_" +
                    IntToString(script_id) +
                    "\", " +
                    IntToString(priority) +
                    " );\n"
                );

                script += 2;
            }
            else if (opcode == 0x08) // JOIN
            {
                export_script->Log(
                    "[UNREVERSED] JOIN(" +
                    ArgumentString(script + 1, 1) +
                    ");\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0x09) // SPLIT
            {
                export_script->Log(
                    "[UNREVERSED] SPLIT(" +
                    ArgumentString(script + 1, 14) +
                    ");\n"
                );

                AdvanceScript(15, script, end);
            }
            else if (opcode == 0x0A) // SPTYE
            {
                export_script->Log(
                    "game:party_set( " +
                    ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 3)) +
                    ", " +
                    ParseGetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 4)) +
                    ", " +
                    ParseGetVariable(GetU8(script + 2) >> 4, GetU8(script + 5)) +
                    " );\n"
                );
                AdvanceScript(6, script, end);
            }
            else if (opcode == 0x0B) // GTPYE
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1) >> 4, GetU8(script + 3), "game:party_get_id_of_member( 0 )") +
                    SetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 4), "game:party_get_id_of_member( 1 )") +
                    SetVariable(GetU8(script + 2) >> 4, GetU8(script + 5), "game:party_get_id_of_member( 2 )")
                );
                AdvanceScript(6, script, end);
            }
            else if (opcode == 0x0F) // SPECIAL
            {
                u8 special_opcode = GetU8(script + 1);

                if (special_opcode == 0xF5) // ARROW
                {
                    export_script->Log(
                        "game:pointer_enable( " +
                        BoolToString(!(bool)(GetU8(script + 2))) +
                        " )\n"
                    );
                    AdvanceScript(3, script, end);
                }
                else if (special_opcode == 0xF9) // FLMAT
                {
                    export_script->Log("game:fill_materia()\n");
                    AdvanceScript(2, script, end);
                }
                else if (special_opcode == 0xFB) // BTLLK
                {
                    export_script->Log(
                        "game:battle_enable( " +
                        BoolToString(!(bool)(GetU8(script + 2))) +
                        " )\n"
                    );
                    AdvanceScript(3, script, end);
                }
                else if (special_opcode == 0xFC) // MVLCK
                {
                    export_script->Log(
                        "game:movie_enable( " +
                        BoolToString(!(bool)(GetU8(script + 2))) +
                        " )\n"
                    );
                    AdvanceScript(3, script, end);
                }
                else if (special_opcode == 0xFD) // SPCNM
                {
                    export_script->Log(
                        "[UNREVERSED] SPECIAL SPCNM(" +
                        ArgumentString(script + 2, 2) +
                        ");\n"
                    );
                    AdvanceScript(4, script, end);
                }
                else if (special_opcode == 0xFE) // RSGLB
                {
                    export_script->Log("game:global_reset()\n");
                    AdvanceScript(2, script, end);
                }
                else
                {
                    export_script->Log("[SPECIAL OPCODE " + HexToString(special_opcode, 2, '0') + "]\n");
                    script += 2;
                }
            }
            else if( opcode == 0x10 ) // JMPF
            {
                u32 temp_end = script + GetU8( script + 1 ) + 1;
                export_script->Log( "jumpto( " + OffsetString( temp_end ) + " );\n" );
                end = ( temp_end > end ) ? temp_end : end;
                script += 2;
            }
            else if (opcode == 0x11) // JMPFL
            {
                u32 temp_end = script + GetU16LE(script + 1) + 1;

                export_script->Log(
                    "jumpto( " +
                    OffsetString(temp_end) +
                    " );\n"
                );

                end = (temp_end > end) ? temp_end : end;
                script += 3;
            }
            else if (opcode == 0x12) // JMPB
            {
                export_script->Log(
                    "jumpto( " +
                    OffsetString(script - GetU8(script + 1)) +
                    " );\n"
                );

                script += 2;
            }
            else if (opcode == 0x13) // JMPBL
            {
                export_script->Log(
                    "jumpto( " +
                    OffsetString(script - GetU16LE(script + 1)) +
                    " );\n"
                );

                script += 3;
            }
            else if (opcode == 0x14) // IFUB
            {
                u32 temp_end = script + GetU8(script + 5) + 5;

                export_script->Log(
                    "if ( " +
                    ParseRelation(GetU8(script + 4), ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 2)), ParseGetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 3))) +
                    " ) then continue else jumpto(" + OffsetString(temp_end) + ");\n"
                );

                end = (temp_end > end) ? temp_end : end;
                script += 6;
            }
            else if (opcode == 0x15) // IFUBL
            {
                u32 temp_end = script + GetU16LE(script + 5) + 5;

                export_script->Log(
                    "if ( " +
                    ParseRelation(GetU8(script + 4), ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 2)), ParseGetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 3))) +
                    " ) then continue else jumpto(" + OffsetString(temp_end) + ");\n"
                );

                end = (temp_end > end) ? temp_end : end;
                script += 7;
            }
            else if (opcode == 0x16) // IFSW
            {
                u32 temp_end = script + GetU8(script + 7) + 7;

                export_script->Log(
                    "if ( " +
                    ParseRelation(GetU8(script + 6), ParseGetVariable(GetU8(script + 1) >> 4, (s16)GetU16LE(script + 2)), ParseGetVariable(GetU8(script + 1) & 0x0F, (s16)GetU16LE(script + 4))) +
                    " ) then continue else jumpto(" + OffsetString(temp_end) + ");\n"
                );

                end = (temp_end > end) ? temp_end : end;
                script += 8;
            }
            else if (opcode == 0x17) // IFSWL
            {
                u32 temp_end = script + GetU16LE(script + 7) + 7;

                export_script->Log(
                    "if ( " +
                    ParseRelation(GetU8(script + 6), ParseGetVariable(GetU8(script + 1) >> 4, (s16)GetU16LE(script + 2)), ParseGetVariable(GetU8(script + 1) & 0x0F, (s16)GetU16LE(script + 4))) +
                    " ) then continue else jumpto(" + OffsetString(temp_end) + ");\n"
                );

                end = (temp_end > end) ? temp_end : end;
                script += 9;
            }
            else if (opcode == 0x18) // IFUW
            {
                u32 temp_end = script + GetU8(script + 7) + 7;

                export_script->Log(
                    "if ( " +
                    ParseRelation(GetU8(script + 6), ParseGetVariable(GetU8(script + 1) >> 4, GetU16LE(script + 2)), ParseGetVariable(GetU8(script + 1) & 0x0F, GetU16LE(script + 4))) +
                    " ) then continue else jumpto(" + OffsetString(temp_end) + ");\n"
                );

                end = (temp_end > end) ? temp_end : end;
                script += 8;
            }
            else if (opcode == 0x21) // TUTOR
            {
                export_script->Log(
                    "[UNREVERSED] TUTOR(" +
                    ArgumentString(script + 1, 1) +
                    ");\n"
                );

                AdvanceScript(2, script, end);
            }
            else if( opcode == 0x24 ) // WAIT
            {
                export_script->Log( "script:wait( " + FloatToString( GetU16LE( script + 1 ) / 30.0f ) + " )\n" );
                AdvanceScript( 3, script, end );
            }
            else if (opcode == 0x25) // NFADE
            {
                u8 type  = GetU8(script + 3);

                if (type == 0)
                {
                    export_script->Log("fade:clear();\n");
                }
                else
                {
                    export_script->Log(
                        "[NEEDS TO CHECK NFADE time needs to be divided] fade:fade(" +
                        ParseGetVariable(GetU8(script + 1) >> 4, GetU16LE(script + 4)) +
                        ", " +
                        ParseGetVariable(GetU8(script + 1) & 0x0F, GetU16LE(script + 5)) +
                        ", " +
                        ParseGetVariable(GetU8(script + 2) >> 4, GetU16LE(script + 6)) +
                        ", " +
                        ((type == 12) ? "Fade.SUBTRACT" : "Fade.ADD") +
                        ", " +
                        ParseGetVariable(GetU8(script + 2) & 0x0F, GetU16LE(script + 7)) +
                        ");\n"
                    );
                }

                AdvanceScript(9, script, end);
            }
            else if (opcode == 0x26) // BLINK
            {
                export_script->Log(
                    entity_list[i] +
                    ":enable_eye_blink(" +
                    BoolToString(!(bool)(GetU8(script + 1))) +
                    ");\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0x28) // KAWAI
            {
                u8 kawai_length = GetU8(script + 1);

                export_script->Log(
                    "[UNREVERSED] KAWAI(" +
                    ArgumentString(script + 1, kawai_length - 1) +
                    ");\n"
                );

                AdvanceScript(kawai_length, script, end);
            }
            else if (opcode == 0x2E) // WCLS
            {
                export_script->Log(
                    "[UNREVERSED] WCLS(" +
                    ArgumentString(script + 1, 1) +
                    ");\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0x2F) // WSIZW
            {
                u8 type = GetU8(script + 2);

                export_script->Log(
                    "-- resize window (id = " +
                    IntToString(GetU8(script + 1)) +
                    ", x = " +
                    IntToString(GetU16LE(script + 2)) +
                    ", y = " +
                    IntToString(GetU16LE(script + 4)) +
                    ", width = " +
                    IntToString(GetU16LE(script + 6)) +
                    ", height = " +
                    IntToString(GetU16LE(script + 8)) +
                    ");\n"
                );

                AdvanceScript(10, script, end);
            }
            else if (opcode == 0x30) // IFKEY
            {
                u32 temp_end = script + GetU8(script + 3) + 3;

                export_script->Log(
                    "if (pressed buttons \"" +
                    HexToString(GetU16LE(script + 1), 4, '0') +
                    "\") then continue else jumpto(" + OffsetString(temp_end) + ");\n"
                );

                end = (temp_end > end) ? temp_end : end;
                script += 4;
            }
            else if (opcode == 0x31) // IFKEYON
            {
                u32 temp_end = script + GetU8(script + 3) + 3;

                export_script->Log(
                    "if (first pressed buttons \"" +
                    HexToString(GetU16LE(script + 1), 4, '0') +
                    "\") then continue else jumpto(" + OffsetString(temp_end) + ");\n"
                );

                end = (temp_end > end) ? temp_end : end;
                script += 4;
            }
            else if (opcode == 0x33) // UC
            {
                export_script->Log(
                    "field:pc_lock(" +
                    BoolToString((bool)(GetU8(script + 1))) +
                    ");\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0x36) // WSPCL
            {
                export_script->Log(
                    "[UNREVERSED] WSPCL(" +
                    ArgumentString(script + 1, 4) +
                    ");\n"
                );

                AdvanceScript(5, script, end);
            }
            else if (opcode == 0x38) // STTIM
            {
                export_script->Log(
                    "game:set_timer(hours(" +
                    ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 3)) +
                    "), minutes(" +
                    ParseGetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 4)) +
                    "), seconds(" +
                    ParseGetVariable(GetU8(script + 2) & 0x0F, GetU8(script + 5)) +
                    "));\n"
                );
                AdvanceScript(6, script, end);
            }
            else if (opcode == 0x39) // GOLDU
            {
                export_script->Log(
                    "game:add_money(upper(" +
                    ParseGetVariable(GetU8(script + 1) & 0x0F, GetU16LE(script + 4)) +
                    "), lower(" +
                    ParseGetVariable(GetU8(script + 1) >> 4, GetU16LE(script + 2)) +
                    "));\n"
                );
                AdvanceScript(6, script, end);
            }
            else if (opcode == 0x3A) // GOLDD
            {
                export_script->Log(
                    "game:remove_money(upper(" +
                    ParseGetVariable(GetU8(script + 1) & 0x0F, GetU16LE(script + 4)) +
                    "), lower(" +
                    ParseGetVariable(GetU8(script + 1) >> 4, GetU16LE(script + 2)) +
                    "));\n"
                );
                AdvanceScript(6, script, end);
            }
            else if (opcode == 0x3B) // CHGLD
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1) >> 4, GetU8(script + 2), "game:get_money_lower()") +
                    SetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 3), "game:get_money_top()")
                );
                AdvanceScript(4, script, end);
            }
            else if (opcode == 0x3E) // MHMMX
            {
                export_script->Log(
                    "game:restore_hp_mp_status_to_all();\n"
                );
                AdvanceScript(1, script, end);
            }
            else if (opcode == 0x3F) // HMPMAX3
            {
                export_script->Log(
                    "game:party_restore_hp_mp();\n"
                );
                AdvanceScript(1, script, end);
            }
            else if (opcode == 0x40) // MESSAGE
            {
                m_Dialogs.push_back(GetU8(script + 2));
                export_script->Log(
                    "message:show_text_wait(" +
                    IntToString(GetU8(script + 1)) +
                    ", " +
                    IntToString(GetU8(script + 2)) +
                    ", x, y);\n"
                );

                AdvanceScript(3, script, end);
            }
            else if (opcode == 0x43) // MPNAM
            {
                m_Dialogs.push_back(GetU8(script + 1));
                export_script->Log(
                    "field:map_name(" +
                    IntToString(GetU8(script + 1)) +
                    ");\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0x48) // ASK
            {
                m_Dialogs.push_back(GetU8(script + 3));
                export_script->Log(
                    "message:show_text_wait(" +
                    IntToString(GetU8(script + 2)) +
                    ", " +
                    IntToString(GetU8(script + 3)) +
                    ", x, y); -- ASK cursor at \"" +
                    ParseGetVariable(GetU8(script + 1), GetU8(script + 6)) +
                    "\", start " +
                    IntToString(GetU8(script + 4)) +
                    ", end " +
                    IntToString(GetU8(script + 5)) +
                    "\n"
                );

                AdvanceScript(7, script, end);
            }
            else if (opcode == 0x49) // MENU
            {
                export_script->Log(
                    "[UNREVERSED] MENU(" +
                    ArgumentString(script + 1, 3) +
                    ");\n"
                );

                AdvanceScript(4, script, end);
            }
            else if (opcode == 0x4A) // MENU2
            {
                export_script->Log(
                    "field:menu_lock(" +
                    BoolToString((bool)(GetU8(script + 1))) +
                    ");\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0x4B) // BTLTB
            {
                export_script->Log(
                    "field:set_battle_table(" +
                    IntToString(GetU8(script + 1)) +
                    ");\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0x50) // WINDOW
            {
                export_script->Log(
                    "-- set window parameters (id = " +
                    IntToString(GetU8(script + 1)) +
                    ", x = " +
                    IntToString(GetU16LE(script + 2)) +
                    ", y = " +
                    IntToString(GetU16LE(script + 4)) +
                    ", width = " +
                    IntToString(GetU16LE(script + 6)) +
                    ", height = " +
                    IntToString(GetU16LE(script + 8)) +
                    ");\n"
                );

                AdvanceScript(10, script, end);
            }
            else if (opcode == 0x52) // WMODE
            {
                u8 type = GetU8(script + 2);

                export_script->Log(
                    "-- set window mode (id = " +
                    IntToString(GetU8(script + 1)) +
                    ", MessageStyle." +
                    ((type == 0) ? "SOLID" : ((type == 1) ? "NONE" : "TRANSPARENT")) +
                    ", disable input from player: " +
                    BoolToString((bool)(GetU8(script + 3))) +
                    ");\n"
                );

                AdvanceScript(4, script, end);
            }
            else if (opcode == 0x53) // WREST
            {
                export_script->Log(
                    "-- reset window to default (id = " +
                    IntToString(GetU8(script + 1)) +
                    ");\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0x54) // WCLSE
            {
                export_script->Log(
                    "[UNREVERSED] WCLSE(" +
                    ArgumentString(script + 1, 1) +
                    ");\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0x58) // STITM
            {
                export_script->Log(
                    "game:item_add(" +
                    ParseGetVariable(GetU8(script + 1) >> 4, GetU16LE(script + 2)) +
                    ", " +
                    ParseGetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 4)) +
                    ");\n"
                );

                AdvanceScript(5, script, end);
            }
            else if (opcode == 0x5A) // CKITM
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1), GetU8(script + 4), "game:get_number_of_item(" + IntToString(GetU16LE(script + 2)) + ")")
                );
                AdvanceScript(5, script, end);
            }
            else if (opcode == 0x5B) // SMTRA
            {
                export_script->Log(
                    "game:materia_add(" +
                    ParseGetVariable(GetU8(script + 1) >> 4, GetU16LE(script + 3)) +
                    ", " +
                    ParseGetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 4)) +
                    ", " +
                    ParseGetVariable(GetU8(script + 2) >> 4, GetU16LE(script + 5)) +
                    ", " +
                    ParseGetVariable(GetU8(script + 2) & 0x0F, GetU8(script + 6)) +
                    ");\n"
                );

                AdvanceScript(7, script, end);
            }
            else if (opcode == 0x5E) // SHAKE
            {
                export_script->Log(
                    "[UNREVERSED] SHAKE(" +
                    ArgumentString(script + 1, 7) +
                    ");\n"
                );

                AdvanceScript(8, script, end);
            }
            else if (opcode == 0x60) // MAPJUMP
            {
                export_script->Log(
                    "field:jump_to_map(" +
                    IntToString(GetU16LE(script + 1)) +
                    ", " +
                    IntToString(GetU16LE(script + 3)) +
                    ", " +
                    IntToString(GetU16LE(script + 5)) +
                    ", " +
                    IntToString(GetU16LE(script + 7)) +
                    ", " +
                    FloatToString((GetU8(script + 9) / 256.0f) * 360.0f) +
                    ");\n"
                );

                AdvanceScript(10, script, end);
            }
            else if (opcode == 0x62) // SCRLC
            {
                int type = GetU8(script + 4);

                export_script->Log(
                    "field:screen_set_scroll_to_pc(" +
                    ((type == 4) ? "Field.NONE" : ((type == 5) ? "Field.LINEAR" : ((type == 6) ? "Field.SMOOTH" : "Unknown_type_" + IntToString(type)))) +
                    ", " +
                    ParseGetVariable(GetU8(script + 1) & 0x0F, GetU16LE(script + 2), 0, 30.0f) +
                    ");\n"
                );

                AdvanceScript(5, script, end);
            }
            else if (opcode == 0x63) // SCRLA
            {
                int type = GetU8(script + 5);

                export_script->Log(
                    "field:screen_set_scroll_to_entity(" +
                    entity_list[GetU8(script + 4)] +
                    ", " +
                    ((type == 1) ? "Field.NONE" : ((type == 2) ? "Field.LINEAR" : ((type == 3) ? "Field.SMOOTH" : "Unknown_type_" + IntToString(type)))) +
                    ", " +
                    ParseGetVariable(GetU8(script + 1), GetU16LE(script + 2), 0, 30.0f) +
                    ");\n"
                );

                AdvanceScript(6, script, end);
            }
            else if( opcode == 0x65 ) // SCRCC
            {
                export_script->Log( "background2d:autoscroll_to_entity( --[[ player entity object here ]] )\n" );
                AdvanceScript( 1, script, end );
            }
            else if( opcode == 0x66 ) // SCR2DC
            {
                export_script->Log( "background2d:scroll_to_position( " + ParseGetVariable( GetU8( script + 1 ) >> 4, ( s16 )GetU16LE( script + 3 ) ) + ", " + ParseGetVariable( GetU8( script + 1 ) & 0x0F, ( s16 )GetU16LE( script + 5 ) ) + ", Background2D.SMOOTH, " + ParseGetVariable( GetU8( script + 2 ) & 0x0F, ( s16 )GetU16LE( script + 7 ), 0, 30.0f ) + " )\n" );
                AdvanceScript( 9, script, end );
            }
            else if( opcode == 0x67 ) // SCRLW
            {
                export_script->Log( "background2d:scroll_sync()\n" );
                AdvanceScript( 1, script, end );
            }
            else if (opcode == 0x68) // SCR2DL
            {
                export_script->Log( "background2d:scroll_to_position( " + ParseGetVariable( GetU8( script + 1 ) >> 4, ( s16 )GetU16LE( script + 3 ) ) + ", " + ParseGetVariable( GetU8( script + 1 ) & 0x0F, ( s16 )GetU16LE( script + 5 ) ) + ", Background2D.LINEAR, " + ParseGetVariable( GetU8( script + 2 ) & 0x0F, ( s16 )GetU16LE( script + 7 ), 0, 30.0f ) + " )\n" );
                AdvanceScript( 9, script, end );
            }
            else if (opcode == 0x6A) // VWOFT
            {
                export_script->Log(
                    "[UNREVERSED] VWOFT(" +
                    ArgumentString(script + 1, 6) +
                    ");\n"
                );

                AdvanceScript(7, script, end);
            }
            else if (opcode == 0x6B) // FADE
            {
                u8 type  = GetU8(script + 7);

                if (type == 4)
                {
                    export_script->Log("fade:black();\n");
                }
                else
                {
                    u8 speed = GetU8(script + 6);
                    u8 start = GetU8(script + 8);

                    export_script->Log(
                        "[NEEDS TO CHECK FADE] fade:fade( " +
                        ParseGetVariable(GetU8(script + 1) >> 4, GetU16LE(script + 3)) +
                        ", " +
                        ParseGetVariable(GetU8(script + 1) & 0x0F, GetU16LE(script + 4)) +
                        ", " +
                        ParseGetVariable(GetU8(script + 2) & 0x0F, GetU16LE(script + 5)) +
                        ", " +
                        ((type == 1 || type == 2 || type == 7 || type == 8) ? "Fade.SUBTRACT" : "Fade.ADD") +
                        ", " +
                        IntToString(speed) +
                        ", " +
                        IntToString(start) +
                        " );\n"
                    );
                }

                AdvanceScript(9, script, end);
            }
            else if (opcode == 0x6C) // FADEW
            {
                export_script->Log("fade:wait();\n");
                AdvanceScript(1, script, end);
            }
            else if (opcode == 0x6D) // IDLCK
            {
                export_script->Log(
                    "field:lock_walkmesh( " +
                    IntToString(GetU16LE(script + 1)) +
                    ", " +
                    BoolToString((bool)(GetU8(script + 3))) +
                    " );\n"
                );

                AdvanceScript(4, script, end);
            }
            else if (opcode == 0x6E) // LSTMP
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1), GetU8(script + 2), "field:get_last_map()")
                );
                AdvanceScript(3, script, end);
            }
            else if (opcode == 0x70) // BATTLE
            {
                export_script->Log(
                    "field:battle_run( " +
                    ParseGetVariable(GetU8(script + 1), GetU16LE(script + 2)) +
                    " );\n"
                );

                AdvanceScript(4, script, end);
            }
            else if (opcode == 0x71) // BTLON
            {
                export_script->Log(
                    "field:random_encounter_on( " +
                    BoolToString(!(bool)(GetU8(script + 1))) +
                    " );\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0x72) // BTLMD
            {
                export_script->Log(
                    "[UNREVERSED] BTLMD(" +
                    ArgumentString(script + 1, 2) +
                    ");\n"
                );

                AdvanceScript(3, script, end);
            }
            else if (opcode == 0x76) // PLUS!
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1) >> 4, GetU8(script + 2), "( ( " + ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 2)) + " + " + ParseGetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 3)) + " > 255 ) ? 255 : " + ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 2)) + " + " + ParseGetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 3)) + " )")
                );
                AdvanceScript(4, script, end);
            }
            else if (opcode == 0x77) // PLUS2!
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1) >> 4, GetU8(script + 2), "( ( " + ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 2)) + " + " + ParseGetVariable(GetU8(script + 1) & 0x0F, GetU16LE(script + 3)) + " > 65535 ) ? 65535 : " + ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 2)) + " + " + ParseGetVariable(GetU8(script + 1) & 0x0F, GetU16LE(script + 3)) + " )")
                );
                AdvanceScript(5, script, end);
            }
            else if (opcode == 0x78) // MINUS!
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1) >> 4, GetU8(script + 2), "( ( " + ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 2)) + " - " + ParseGetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 3)) + " < 0 ) ? 0 : " + ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 2)) + " - " + ParseGetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 3)) + " )")
                );
                AdvanceScript(4, script, end);
            }
            else if( opcode == 0x79 ) // MINUS2!
            {
                export_script->Log( SetVariable( GetU8( script + 1 ) >> 4, GetU8( script + 2 ), "( ( " + ParseGetVariable( GetU8( script + 1 ) >> 4, GetU8( script + 2 ) ) + " - " + ParseGetVariable( GetU8( script + 1 ) & 0x0F, GetU16LE( script + 3 ) ) + " < 0 ) ? 0 : " + ParseGetVariable( GetU8( script + 1 ) >> 4, GetU8( script + 2 ) ) + " - " + ParseGetVariable( GetU8( script + 1 ) & 0x0F, GetU16LE( script + 3 ) ) + " )" ) );
                AdvanceScript( 5, script, end );
            }
            else if( opcode == 0x7A ) // INC!
            {
                export_script->Log( SetVariable( GetU8( script + 1 ), GetU8( script + 2 ), "( ( " + ParseGetVariable( GetU8( script + 1 ), GetU8( script + 2 ) ) + " + 1 > 255 ) ? 255 : " + ParseGetVariable( GetU8( script + 1 ), GetU8( script + 2 ) ) + " + 1 )" ) );
                AdvanceScript( 3, script, end );
            }
            else if (opcode == 0x7E) // TALKON
            {
                export_script->Log(
                    entity_list[i] +
                    ":set_talkable( " +
                    BoolToString(!(bool)(GetU8(script + 1))) +
                    " );\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0x80) // SETBYTE
            {
                export_script->Log(
                    SetVariable( GetU8( script + 1 ) >> 4, GetU8( script + 2 ), ParseGetVariable( GetU8( script + 1 ) & 0x0F, GetU8( script + 3 ) ) )
                );
                AdvanceScript( 4, script, end );
            }
            else if( opcode == 0x81 ) // SETWORD
            {
                export_script->Log( SetVariable( GetU8( script + 1 ) >> 4, GetU8( script + 2 ), ParseGetVariable( GetU8( script + 1 ) & 0x0F, GetU16LE( script + 3 ) ) ) );
                AdvanceScript( 5, script, end );
            }
            else if (opcode == 0x82) // BITON
            {
                export_script->Log(SetBitChange(GetU8(script + 1) >> 4, GetU8(script + 2), GetU8(script + 1) & 0x0F, GetU8(script + 3), true));

                AdvanceScript(4, script, end);
            }
            else if( opcode == 0x83 ) // BITOFF
            {
                export_script->Log( SetBitChange( GetU8( script + 1 ) >> 4, GetU8( script + 2 ), GetU8( script + 1 ) & 0x0F, GetU8( script + 3 ), false ) );
                AdvanceScript( 4, script, end );
            }
            else if (opcode == 0x85) // PLUS
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1) >> 4, GetU8(script + 2), ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 2)) + " + " + ParseGetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 3)))
                );
                AdvanceScript(4, script, end);
            }
            else if (opcode == 0x87) // MINUS
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1) >> 4, GetU8(script + 2), ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 2)) + " - " + ParseGetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 3)))
                );
                AdvanceScript(4, script, end);
            }
            else if (opcode == 0x89) // MUL
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1) >> 4, GetU8(script + 2), "((" + ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 2)) + " * " + ParseGetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 3)) + " > 255) ? 255 : " + ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 2)) + " * " + ParseGetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 3)) + ")")
                );
                AdvanceScript(4, script, end);
            }
            else if (opcode == 0x8D) // MOD
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1) >> 4, GetU8(script + 2), ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 2)) + " % " + ParseGetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 3)))
                );
                AdvanceScript(4, script, end);
            }
            else if (opcode == 0x8F) // AND
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1) >> 4, GetU8(script + 2), ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 2)) + " & " + ParseGetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 3)))
                );
                AdvanceScript(4, script, end);
            }
            else if (opcode == 0x90) // AND2
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1) >> 4, GetU8(script + 2), ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 2)) + " & " + ParseGetVariable(GetU8(script + 1) & 0x0F, GetU16LE(script + 3)))
                );
                AdvanceScript(5, script, end);
            }
            else if( opcode == 0x91 ) // OR
            {
                export_script->Log( SetVariable( GetU8( script + 1 ) >> 4, GetU8( script + 2 ), ParseGetVariable( GetU8( script + 1 ) >> 4, GetU8( script + 2 ) ) + " | " + ParseGetVariable( GetU8( script + 1 ) & 0x0F, GetU8( script + 3 ) ) ) );
                AdvanceScript( 4, script, end );
            }
            else if (opcode == 0x92) // OR2
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1) >> 4, GetU8(script + 2), ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 2)) + " | " + ParseGetVariable(GetU8(script + 1) & 0x0F, GetU16LE(script + 3)))
                );
                AdvanceScript(5, script, end);
            }
            else if (opcode == 0x95) // INC
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1), GetU8(script + 2), ParseGetVariable(GetU8(script + 1), GetU8(script + 2)) + " + 1")
                );
                AdvanceScript(3, script, end);
            }
            else if (opcode == 0x97) // DEC
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1), GetU8(script + 2), ParseGetVariable(GetU8(script + 1), GetU8(script + 2)) + " - 1")
                );
                AdvanceScript(3, script, end);
            }
            else if (opcode == 0x99) // RANDOM
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1), GetU8(script + 2), "math.rand()")
                );
                AdvanceScript(3, script, end);
            }
            else if( opcode == 0xA0 ) // PC
            {
                export_script->Log( "set_entity_to_character( \"" + entity_list[ i ] + "\", " + IntToString( GetU8( script + 1 ) ) + " );\n" );
                AdvanceScript( 2, script, end );
            }
            else if( opcode == 0xA1 ) // CHAR
            {
                export_script->Log( "-- assosiate entity with model (CHAR) argument doesn't matter\n" );
                AdvanceScript( 2, script, end );
            }
            else if( opcode == 0xA2 ) // DFANM
            {
                export_script->Log( entity_list[ i ] + ":set_default_animation( \"" + IntToString( GetU8( script + 1 ) ) + "\" ) -- speed=" + FloatToString( 1.0f / GetU8( script + 2 ) ) + "\n" );
                export_script->Log( "                 " + entity_list[ i ] + ":play_animation( \"" + IntToString( GetU8( script + 1 ) ) + "\" )\n" );
                AdvanceScript( 3, script, end );
            }
            else if( opcode == 0xA3 ) // ANIME1
            {
                export_script->Log( entity_list[ i ] + ":play_animation( \"" + IntToString( GetU8( script + 1 ) ) + "\" ) -- speed=" + FloatToString( 1.0f / GetU8( script + 2 ) ) + "\n" );
                export_script->Log( "                 " + entity_list[ i ] + ":animation_sync()\n" );
                AdvanceScript( 3, script, end );
            }
            else if( opcode == 0xA4 ) // VISI
            {
                export_script->Log( entity_list[ i ] + ":set_visible( " + BoolToString( ( bool )( GetU8( script + 1 ) ) ) + " )\n" );
                AdvanceScript( 2, script, end );
            }
            else if( opcode == 0xA5 ) // XYZI
            {
                export_script->Log( entity_list[ i ] + ":set_position( " + ParseGetVariable( GetU8( script + 1 ) >> 4, ( s16 )GetU16LE( script + 3 ), 0, downscaler ) + ", " + ParseGetVariable( GetU8( script + 1 ) & 0x0F, ( s16 )GetU16LE( script + 5 ), 0, downscaler ) + ", " + ParseGetVariable( GetU8( script + 2 ) >> 4, ( s16 )GetU16LE( script + 7 ), 0, downscaler ) + " ) -- triangle_id=" + ParseGetVariable( GetU8( script + 2 ) & 0x0F, ( s16 )GetU16LE( script + 9 ) ) + "\n" );
                AdvanceScript( 11, script, end );
            }
            else if( opcode == 0xA6 ) // XYI
            {
                export_script->Log(
                    entity_list[ i ] +
                    ":set_2d_position_triangle( " +
                    ParseGetVariable( GetU8( script + 1 ) >> 4, ( s16 )GetU16LE( script + 3 ), false, downscaler ) +
                    ", " +
                    ParseGetVariable( GetU8( script + 1 ) & 0x0F, ( s16 )GetU16LE( script + 5 ), false, downscaler ) +
                    ", triangle_id=" +
                    ParseGetVariable( GetU8( script + 2 ) >> 4, ( s16 )GetU16LE( script + 7 ) ) +
                    ");\n"
                );

                AdvanceScript(9, script, end);
            }
            else if( opcode == 0xA8 ) // MOVE
            {
                export_script->Log( entity_list[ i ] + ":move_to_position( " + ParseGetVariable( GetU8( script + 1 ) >> 4, ( s16 )GetU16LE( script + 2 ), false, downscaler ) + ", " + ParseGetVariable( GetU8( script + 1 ) & 0x0F, ( s16 )GetU16LE( script + 4 ), false, downscaler ) + " )\n" );
                export_script->Log( "                 " + entity_list[ i ] + ":move_sync()\n" );
                AdvanceScript( 6, script, end );
            }
            else if( opcode == 0xA9 ) // CMOVE
            {
                export_script->Log(
                    entity_list[ i ] +
                    ":set_rotation_lock( true );\n" +
                    entity_list[ i ] +
                    ":move_to_position( " +
                    ParseGetVariable( GetU8( script + 1 ) >> 4, ( s16 )GetU16LE( script + 2), 0, downscaler ) +
                    ", " +
                    ParseGetVariable( GetU8( script + 1 ) & 0x0F, ( s16 )GetU16LE( script + 4 ), 0, downscaler ) +
                    ", false );\n" +
                    entity_list[ i ] +
                    ":set_rotation_lock( false );\n"
                );

                AdvanceScript( 6, script, end );
            }
            else if( opcode == 0xAA ) // MOVA
            {
                export_script->Log(
                    entity_list[ i ] +
                    ":move_to_model( " +
                    entity_list[ GetU8( script + 1 ) ] +
                    ", true );\n"
                );

                AdvanceScript( 2, script, end );
            }
            else if( opcode == 0xAB ) // TURA
            {
                u8 type = GetU8( script + 2 );

                export_script->Log(
                    entity_list[ i ] +
                    ":turn_to_entity( " +
                    entity_list[ GetU8( script + 1 ) ] +
                    ", " +
                    ( ( type == 0 ) ? "Entity.CLOCKWISE" : ( ( type == 1 ) ? "Entity.ANTICLOCKWISE" : "Entity.CLOSEST" ) ) +
                    ", " +
                    FloatToString( GetU8( script + 3 ) / 30.0f ) +
                    ");\n"
                );

                AdvanceScript( 4, script, end );
            }
            else if (opcode == 0xAC) // ANIMW
            {
                export_script->Log(
                    entity_list[i] +
                    ":set_animation_wait();\n"
                );

                AdvanceScript(1, script, end);
            }
            else if (opcode == 0xAD) // FMOVE
            {
                export_script->Log(
                    entity_list[i] +
                    ":move_to_position( \"" +
                    ParseGetVariable(GetU8(script + 1) >> 4, (s16)GetU16LE(script + 2)) +
                    ", " +
                    ParseGetVariable(GetU8(script + 1) & 0x0F, (s16)GetU16LE(script + 4)) +
                    ", false );\n"
                );

                AdvanceScript(6, script, end);
            }
            else if (opcode == 0xAE) // ANIME2
            {
                export_script->Log(
                    entity_list[i] +
                    ":play_animation( \"" +
                    IntToString(GetU8(script + 1)) +
                    "\", 0, -1, " +
                    FloatToString(1.0f / GetU8(script + 2)) +
                    ", false );\n"
                );

                AdvanceScript(3, script, end);
            }
            else if( opcode == 0xAF ) // ANIM!1
            {
                export_script->Log( entity_list[ i ] + ":play_animation_stop( \"" + IntToString( GetU8( script + 1 ) ) + "\" ) -- speed=" + FloatToString( 1.0f / GetU8( script + 2 ) ) + "\n" );
                AdvanceScript( 3, script, end );
            }
            else if (opcode == 0xB1) // CANM!1
            {
                export_script->Log(
                    entity_list[i] +
                    ":play_animation( \"" +
                    IntToString(GetU8(script + 1)) +
                    "\", " +
                    FloatToString(GetU8(script + 2) / 30.0f) +
                    ", " +
                    FloatToString(GetU8(script + 3) / 30.0f) +
                    ", " +
                    FloatToString(1.0f / GetU8(script + 4)) +
                    ", true );\n"
                );

                AdvanceScript(5, script, end);
            }
            else if( opcode == 0xB2 ) // MSPED
            {
                export_script->Log( entity_list[ i ] + ":set_move_auto_speed( " + ParseGetVariable( GetU8( script + 1 ), GetU16LE( script + 2 ), 0, 256.0f * downscaler / ( 30.0f ) ) + " )\n" );
                AdvanceScript( 4, script, end );
            }
            else if( opcode == 0xB3 ) // DIR
            {
                export_script->Log( entity_list[ i ] + ":set_direction( " + ParseGetVariable( GetU8( script + 1 ), GetU8( script + 2 ), 0, 256.0f / 360.0f ) + " )\n" );
                AdvanceScript(3, script, end);
            }
            else if( opcode == 0xB4 ) // TURNGEN
            {
                u8 type = GetU8( script + 3 );
                u8 calc = GetU8( script + 5 );
                export_script->Log( entity_list[ i ] + ":turn_to_direction( " + ParseGetVariable( GetU8( script + 1 ), GetU8( script + 2 ), 0, 256.0f / 360.0f ) + ", " + ( ( type == 0 ) ? "Entity.CLOCKWISE" : ( ( type == 1 ) ? "Entity.ANTICLOCKWISE" : "Entity.CLOSEST" ) ) + ", " + ( ( calc == 1 ) ? "Entity.LINEAR" : "Entity.SMOOTH" ) + ", " + FloatToString( GetU8( script + 4 ) / 30.0f ) + " )\n" );
                export_script->Log( entity_list[ i ] + ":turn_sync()\n" );
                AdvanceScript( 6, script, end );
            }
            else if (opcode == 0xB5) // TURN
            {
                u8 calc = GetU8(script + 5);

                export_script->Log(
                    entity_list[i] +
                    ":turn_to_direction( " +
                    ParseGetVariable(GetU8(script + 1), GetU16LE(script + 2), 0, 256.0f / 360.0f) +
                    ", Entity.CLOCKWISE, " +
                    ((calc == 1) ? "Field.LINEAR" : "Field.SMOOTH") +
                    ", " +
                    FloatToString(GetU8(script + 4) / 30.0f) +
                    " );\n"
                );
                export_script->Log( entity_list[ i ] + ":turn_sync()\n" );

                AdvanceScript(6, script, end);
            }
            else if( opcode == 0xB6 ) // DIRA
            {
                export_script->Log( entity_list[ i ] + ":set_direction( \"" + entity_list[ GetU8( script + 1 ) ] + "\" )\n" );
                AdvanceScript( 2, script, end );
            }
            else if (opcode == 0xB7) // GETDIR
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1), GetU8(script + 3), entity_list[GetU8(script + 2)] + ":get_direction()")
                );
                AdvanceScript(4, script, end);
            }
            else if (opcode == 0xB8) // GETAXY
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1) >> 4, GetU8(script + 3), entity_list[GetU8(script + 2)] + ":get_x()") +
                    SetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 4), entity_list[GetU8(script + 2)] + ":get_y()")
                );
                AdvanceScript(5, script, end);
            }
            else if( opcode == 0xB9 ) // GETAI
            {
                export_script->Log( SetVariable( GetU8( script + 1 ), GetU8( script + 3 ), entity_list[ GetU8( script + 2 ) ] + ":get_move_triangle_id()" ) );
                AdvanceScript( 4, script, end );
            }
            else if( opcode == 0xBA ) // ANIM!2
            {
                export_script->Log( entity_list[ i ] + ":play_animation_stop( \"" + IntToString( GetU8( script + 1 ) ) + "\" ) -- speed=" + FloatToString( 1.0f / GetU8( script + 2 ) ) + "\n" );
                export_script->Log( "                 " + entity_list[ i ] + ":animation_sync()\n" );
                AdvanceScript( 3, script, end );
            }
            else if( opcode == 0xBB ) // CANIM2
            {
                export_script->Log( entity_list[ i ] + ":play_animation( \"" + IntToString( GetU8( script + 1 ) ) + "\", " + FloatToString( GetU8( script + 2 ) / 30.0f ) + ", " + FloatToString( GetU8( script + 3 ) / 30.0f ) + " ) -- speed=" + FloatToString( 1.0f / GetU8( script + 4 ) ) + "\n" );
                export_script->Log( "                 " + entity_list[ i ] + ":animation_sync()\n" );
                AdvanceScript( 5, script, end );
            }
            else if( opcode == 0xBC ) // CANM!2
            {
                export_script->Log( entity_list[ i ] + ":play_animation_stop( \"" + IntToString( GetU8( script + 1 ) ) + "\", " + FloatToString( GetU8( script + 2 ) / 30.0f ) + ", " + FloatToString( GetU8( script + 3 ) / 30.0f ) + " ) -- speed=" + FloatToString( 1.0f / GetU8( script + 4 ) ) + "\n" );
                export_script->Log( "                 " + entity_list[ i ] + ":animation_sync()\n" );
                AdvanceScript( 5, script, end );
            }
            else if( opcode == 0xBD ) // ASPED
            {
                export_script->Log( "-- set speed of entity animation to \"" + ParseGetVariable( GetU8( script + 1 ), GetU16LE( script + 2 ), 0, 0x10 ) + "\"\n" );
                AdvanceScript( 4, script, end );
            }
            else if (opcode == 0xBF) // CC
            {
                export_script->Log(
                    "field:set_player_entity( " +
                    entity_list[GetU8(script + 1)] +
                    " );\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0xC0) // JUMP
            {
                export_script->Log(
                    entity_list[i] +
                    ":jump_to_position( " +
                    ParseGetVariable(GetU8(script + 1) >> 4, (s16)GetU16LE(script + 3)) +
                    ", " +
                    ParseGetVariable(GetU8(script + 1) & 0x0F, (s16)GetU16LE(script + 5)) +
                    ", " +
                    ParseGetVariable(GetU8(script + 2) >> 4, GetU16LE(script + 7)) +
                    ", " +
                    ParseGetVariable(GetU8(script + 2) & 0x0F, GetU16LE(script + 9), 0, 30.0f) +
                    " );\n"
                );

                AdvanceScript(11, script, end);
            }
            else if (opcode == 0xC1) // AXYZI
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1) >> 4, GetU8(script + 4), entity_list[GetU8(script + 3)] + ":get_x()") +
                    SetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 5), entity_list[GetU8(script + 3)] + ":get_y()") +
                    SetVariable(GetU8(script + 2) >> 4, GetU8(script + 6), entity_list[GetU8(script + 3)] + ":get_z()") +
                    SetVariable(GetU8(script + 2) & 0x0F, GetU8(script + 7), entity_list[GetU8(script + 3)] + ":get_move_triangle_id()")
                );
                AdvanceScript(8, script, end);
            }
            else if (opcode == 0xC2) // LADER
            {
                u8 type = GetU8(script + 11);

                export_script->Log(
                    entity_list[i] +
                    ":climb_to_position( " +
                    ParseGetVariable(GetU8(script + 1) >> 4, (s16)GetU16LE(script + 3)) +
                    ", " +
                    ParseGetVariable(GetU8(script + 1) & 0x0F, (s16)GetU16LE(script + 5)) +
                    ", " +
                    ParseGetVariable(GetU8(script + 2) >> 4, (s16)GetU16LE(script + 7)) +
                    ", " +
                    ParseGetVariable(GetU8(script + 2) & 0x0F, GetU16LE(script + 9)) +
                    ", " +
                    ((type == 0) ? "Entity.UP_TO_DOWN" : ((type == 1) ? "Entity.DOWN_TO_UP" : ((type == 2) ? "Entity.LEFT_TO_RIGHT" : ((type == 1) ? "Entity.RIGHT_TO_LEFT" : "")))) +
                    " ); -- direction needs to be seted to \"" +
                    FloatToString((GetU8(script + 13) / 256.0f) * 360.0f) +
                    "\" and animation during climbing is \"" +
                    IntToString(GetU8(script + 12)) +
                    "\" with speed \"" +
                    FloatToString(1.0f / GetU8(script + 14)) +
                    "\"\n"
                );

                AdvanceScript(15, script, end);
            }
            else if( opcode == 0xC3 ) // OFST
            {
                u8 type = GetU8( script + 3 );
                export_script->Log( entity_list[ i ] + ":offset_to_position( " + ParseGetVariable( GetU8( script + 1 ) >> 4, ( s16 )GetU16LE( script + 4 ), 0, downscaler ) + ", " + ParseGetVariable( GetU8( script + 1 ) & 0x0F, ( s16 )GetU16LE( script + 6 ), false, downscaler ) + ", " + ParseGetVariable( GetU8( script + 2 ) >> 4, ( s16 )GetU16LE( script + 8 ), false, downscaler ) + ", " + ( ( type == 2 ) ? "Entity.SMOOTH" : "Entity.LINEAR" ) + ", " + ParseGetVariable( GetU8( script + 2 ) & 0x0F, GetU16LE( script + 10 ), 0, 30.0f ) + " )\n" );
                AdvanceScript( 12, script, end );
            }
            else if( opcode == 0xC4 ) // OFSTW
            {
                export_script->Log( entity_list[ i ] + ":offset_sync()\n" );
                AdvanceScript( 1, script, end );
            }
            else if (opcode == 0xC5) // TALKR
            {
                export_script->Log(
                    entity_list[i] +
                    ":set_talk_range( " +
                    ParseGetVariable(GetU8(script + 1), GetU8(script + 2)) +
                    " );\n"
                );

                AdvanceScript(3, script, end);
            }
            else if (opcode == 0xC6) // CLIDR
            {
                export_script->Log(
                    entity_list[i] +
                    ":set_solid_range( " +
                    ParseGetVariable(GetU8(script + 1), GetU8(script + 2)) +
                    " );\n"
                );

                AdvanceScript(3, script, end);
            }
            else if( opcode == 0xC7 ) // SOLID
            {
                export_script->Log( entity_list[ i ] + ":set_solid( " + BoolToString( !( bool )( GetU8( script + 1 ) ) ) + " )\n" );
                AdvanceScript( 2, script, end );
            }
            else if (opcode == 0xC8) // PRTYP
            {
                export_script->Log(
                    "game:party_add( " +
                    IntToString(GetU8(script + 1)) +
                    " );\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0xC9) // PRTYM
            {
                export_script->Log(
                    "game:party_remove( " +
                    IntToString(GetU8(script + 1)) +
                    " );\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0xCA) // PRTYE
            {
                export_script->Log(
                    "game:party_set( " +
                    IntToString(GetU8(script + 1)) +
                    ", " +
                    IntToString(GetU8(script + 2)) +
                    ", " +
                    IntToString(GetU8(script + 3)) +
                    " );\n"
                );

                AdvanceScript(4, script, end);
            }
            else if (opcode == 0xCD) // MMBUD
            {
                export_script->Log(
                    "game:character_enable( " +
                    IntToString(GetU8(script + 2)) +
                    ", " +
                    BoolToString((bool)(GetU8(script + 1))) +
                    " );\n"
                );

                AdvanceScript(3, script, end);
            }
            else if (opcode == 0xCE) // MMBLK
            {
                export_script->Log(
                    "[UNREVERSED] MMBLK(" +
                    ArgumentString(script + 1, 1) +
                    ");\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0xD0) // LINE
            {
                export_script->Log(
                    "-- define line point1( " +
                    IntToString(GetU16LE(script + 1)) +
                    ", " +
                    IntToString(GetU16LE(script + 3)) +
                    ", " +
                    IntToString(GetU16LE(script + 5)) +
                    "), point2(" +
                    IntToString(GetU16LE(script + 7)) +
                    ", " +
                    IntToString(GetU16LE(script + 9)) +
                    ", " +
                    IntToString(GetU16LE(script + 11)) +
                    " )\n"
                );

                AdvanceScript(13, script, end);
            }
            else if (opcode == 0xD1) // LINON
            {
                export_script->Log(
                    "-- turn line " +
                    Ogre::String((GetU8(script + 1) == 1) ? "on" : "off") +
                    "\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0xD2) // MPJPO
            {
                export_script->Log(
                    "field:gateway_enable( " +
                    Ogre::String((GetU8(script + 1) == 1) ? "false" : "true") +
                    " );\n"
                );

                AdvanceScript( 2, script, end );
            }
            else if( opcode == 0xD3 ) // SLINE
            {
                u8 type = GetU8(script + 2);

                export_script->Log(
                    "-- redefine line point1( " +
                    ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 4)) +
                    ", " +
                    ParseGetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 6)) +
                    ", " +
                    ParseGetVariable(GetU8(script + 2) >> 4, GetU8(script + 8)) +
                    "), point2(" +
                    ParseGetVariable(GetU8(script + 2) & 0x0F, GetU8(script + 10)) +
                    ", " +
                    ParseGetVariable(GetU8(script + 3) >> 4, GetU8(script + 12)) +
                    ", " +
                    ParseGetVariable(GetU8(script + 3) & 0x0F, GetU8(script + 14)) +
                    " )\n"
                );

                AdvanceScript(16, script, end);
            }
            else if (opcode == 0xD8) // PMJMP
            {
                export_script->Log(
                    "[UNREVERSED] PMJMP(" +
                    ArgumentString(script + 1, 2) +
                    ");\n"
                );

                AdvanceScript(3, script, end);
            }
            else if (opcode == 0xDA) // AKAO2
            {
                u8 sub_opcode = GetU8(script + 4);

                Ogre::String sound = "music:execute_akao( " +
                    HexToString(sub_opcode, 2, '0') +
                    ", " +
                    ParseGetVariable(GetU8(script + 1) >> 4, GetU16LE(script + 5), 4) +
                    ", " +
                    ParseGetVariable(GetU8(script + 1) & 0x0F, GetU16LE(script + 7), 4) +
                    ", " +
                    ParseGetVariable(GetU8(script + 2) >> 4, GetU16LE(script + 9), 4) +
                    ", " +
                    ParseGetVariable(GetU8(script + 2) & 0x0F, GetU16LE(script + 11), 4) +
                    ", " +
                    ParseGetVariable(GetU8(script + 3) >> 4, GetU16LE(script + 13), 4) +
                    ", " +
                    ParseGetVariable(GetU8(script + 3) & 0x0F, GetU16LE(script + 15), 4) +
                    " );\n";

                m_SoundOpcodes.push_back(sound);
                export_script->Log(sound);

                AdvanceScript(15, script, end);
            }
            else if (opcode == 0xDD) // ANIMB
            {
                export_script->Log(
                    entity_list[i] +
                    ":animation_stop();\n"
                );

                AdvanceScript(1, script, end);
            }
            else if (opcode == 0xDE) // TURNW
            {
                export_script->Log(
                    entity_list[i] +
                    ":set_turn_wait();\n"
                );

                AdvanceScript(1, script, end);
            }
            else if( opcode == 0xE0 ) // BGON
            {
                export_script->Log(
                    "field:background_on( \"" +
                    ParseGetVariable( GetU8( script + 1 ) >> 4, GetU8( script + 2 ) ) +
                    "\", " +
                    ParseGetVariable( GetU8( script + 1 ) & 0x0F, GetU8( script + 3 ) ) +
                    " ); -- turns on layer of background animation with given id\n"
                );
                AdvanceScript( 4, script, end );
            }
            else if( opcode == 0xE1 ) // BGOFF
            {
                export_script->Log(
                    "field:background_off( \"" +
                    ParseGetVariable( GetU8( script + 1 ) >> 4, GetU8( script + 2 ) ) +
                    "\", " +
                    ParseGetVariable( GetU8( script + 1 ) & 0x0F, GetU8( script + 3 ) ) +
                    " ); -- turns off layer of background animation with given id\n"
                );
                AdvanceScript( 4, script, end );
            }
            else if( opcode == 0xE4 ) // BGCLR
            {
                export_script->Log(
                    "field:background_clear( \"" +
                    ParseGetVariable( GetU8( script + 1 ), GetU8( script + 2 ) ) +
                    "\" ); -- turns off all layer of background animation with given id\n"
                );
                AdvanceScript( 3, script, end );
            }
            else if( opcode == 0xE5 ) // STPAL
            {
                export_script->Log( "-- store palette \"" + ParseGetVariable( GetU8( script + 1 ) >> 4, GetU8( script + 2 ) ) + "\" to slot \"0x" + ParseGetVariable( GetU8( script + 1 ) & 0x0f, GetU8( script + 3 ), 2 ) + "\" starting from clut_x=\"0x00\" and with width=\"0x" + HexToString( GetU8( script + 4 ) + 1, 2, '0' ) + "\"\n" );
                AdvanceScript( 5, script, end );
            }
            else if( opcode == 0xE6 ) // LDPAL
            {
                export_script->Log( "-- load palette \"" + ParseGetVariable( GetU8( script + 1 ) & 0x0f, GetU8( script + 3 ) ) + "\" from slot \"0x" + ParseGetVariable( GetU8( script + 1 ) >> 4, GetU8( script + 2 ), 2 ) + "\" starting from clut_x=\"0x00\" and with width=\"0x" + HexToString( GetU8( script + 4 ) + 1, 2, '0' ) + "\"\n" );
                AdvanceScript( 5, script, end );
            }
            else if( opcode == 0xE7 ) // CPPAL
            {
                export_script->Log( "-- copy palette stored in slot \"0x" + ParseGetVariable( GetU8( script + 1 ) >> 4, GetU8( script + 2 ), 2 ) + "\" with width=\"0x" + HexToString( GetU8( script + 4 ) + 1, 2, '0' ) + " to slot \"0x" + ParseGetVariable( GetU8( script + 1 ) & 0x0f, GetU8( script + 3 ), 2 ) + "\"\n" );
                AdvanceScript( 5, script, end );
            }
            else if( opcode == 0xE9 ) // ADPAL
            {
                export_script->Log( "-- add to palette stored in slot \"0x" + ParseGetVariable( GetU8( script + 1 ) >> 4, GetU8( script + 4 ), 2 ) + "\" with width=\"0x" + HexToString( GetU8( script + 9 ) + 1, 2, '0' ) + "\" with values R=\"" + ParseGetVariable( GetU8( script + 3 ) >> 4, GetU8( script + 8 ) ) + "\", G=\"" + ParseGetVariable( GetU8( script + 2 ) & 0x0f, GetU8( script + 7 ) ) + "\", B=\"" + ParseGetVariable( GetU8( script + 2 ) >> 4, GetU8( script + 6 ) ) + "\" and store it to slot \"0x" + ParseGetVariable( GetU8( script + 1 ) & 0x0f, GetU8( script + 5 ), 2 ) + "\"\n" );
                AdvanceScript( 10, script, end );
            }
            else if( opcode == 0xEA ) // MPPAL2
            {
                export_script->Log( "-- multiply palette stored in slot \"0x" + ParseGetVariable( GetU8( script + 1 ) >> 4, GetU8( script + 4 ), 2 ) + "\" with width=\"0x" + HexToString( GetU8( script + 9 ) + 1, 2, '0' ) + " with values R=\"" + ParseGetVariable( GetU8( script + 3 ) >> 4, GetU8( script + 8 ) ) + "\", G=\"" + ParseGetVariable( GetU8( script + 2 ) & 0x0f, GetU8( script + 7 ) ) + "\", B=\"" + ParseGetVariable( GetU8( script + 2 ) >> 4, GetU8( script + 6 ) ) + "\" (note: 64 - 1.0f, 32 - 0.5f) and store it to slot \"0x" + ParseGetVariable( GetU8( script + 1 ) & 0x0f, GetU8( script + 5 ), 2 ) + "\"\n" );
                AdvanceScript( 10, script, end );
            }
            else if( opcode == 0xEB ) // STPLS
            {
                export_script->Log( "-- store palette \"" + IntToString( GetU8( script + 1 ) ) + "\" to slot \"0x" + HexToString( GetU8( script + 2 ), 2, '0' ) + "\" starting from clut_x=\"0x" + HexToString( GetU8( script + 3 ), 2, '0' ) + "\" and with width=\"0x" + HexToString( GetU8( script + 4 ) + 1, 2, '0' ) + "\"\n" );
                AdvanceScript( 5, script, end );
            }
            else if( opcode == 0xEC ) // LDPLS
            {
                export_script->Log( "-- load palette \"" + IntToString( GetU8( script + 2 ) ) + "\" from slot \"0x" + HexToString( GetU8( script + 1 ), 2, '0' ) + "\" starting from clut_x=\"0x" + HexToString( GetU8( script + 3 ), 2, '0' ) + "\" and with width=\"0x" + HexToString( GetU8( script + 4 ) + 1, 2, '0' ) + "\"\n" );
                AdvanceScript( 5, script, end );
            }
            else if( opcode == 0xEE ) // RTPAL2
            {
                export_script->Log( "[UNREVERSED] RTPAL2( " + ArgumentString( script + 1, 7 ) + " );\n" );
                AdvanceScript( 8, script, end );
            }
            else if( opcode == 0xEF ) // ADPAL2
            {
                export_script->Log( "[UNREVERSED] ADPAL2( " + ArgumentString( script + 1, 10 ) + ");\n" );
                AdvanceScript( 11, script, end );
            }
            else if (opcode == 0xF0) // MUSIC
            {
                Ogre::String sound = "music:execute_akao( " +
                    HexToString(0x10, 2, '0') +
                    ", pointer_to_field_AKAO_" +
                    IntToString(GetU8(script + 1)) +
                    " ); -- play field music\n";

                m_SoundOpcodes.push_back(sound);
                export_script->Log(sound);

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0xF1) // SOUND
            {
                Ogre::String sound = "music:execute_akao( " +
                    HexToString(0x20, 2, '0') +
                    ", " +
                    ParseGetVariable(GetU8(script + 1) & 0x0F, GetU8(script + 4), true) +
                    ", " +
                    ParseGetVariable(GetU8(script + 1) >> 4, GetU16LE(script + 2), true) +
                    " ); -- play sound\n";

                m_SoundOpcodes.push_back(sound);
                export_script->Log(sound);

                AdvanceScript(5, script, end);
            }
            else if (opcode == 0xF2) // AKAO
            {
                u8 sub_opcode = GetU8(script + 4);

                Ogre::String sound = "music:execute_akao( " +
                    HexToString(sub_opcode, 2, '0') +
                    ", " +
                    ParseGetVariable(GetU8(script + 1) >> 4, GetU8(script + 5), true) +
                    ", " +
                    ParseGetVariable(GetU8(script + 1) & 0x0F, GetU16LE(script + 7), true) +
                    ", " +
                    ParseGetVariable(GetU8(script + 2) >> 4, GetU16LE(script + 9), true) +
                    ", " +
                    ParseGetVariable(GetU8(script + 2) & 0x0F, GetU16LE(script + 11), true) +
                    ", " +
                    ParseGetVariable(GetU8(script + 3) >> 4, GetU16LE(script + 13), true) +
                    ", " +
                    ParseGetVariable(GetU8(script + 3) & 0x0F, GetU16LE(script + 15), true) +
                    " );\n";
                m_SoundOpcodes.push_back(sound);

                export_script->Log(sound);

                AdvanceScript(14, script, end);
            }
            else if (opcode == 0xF5) // MULCK
            {
                export_script->Log(
                    "music:lock( " +
                    BoolToString((bool)(GetU8(script + 1))) +
                    " );\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0xF6) // BMUSC
            {
                export_script->Log(
                    "[UNREVERSED] BMUSC(" +
                    IntToString(GetU8(script + 1)) +
                    ");\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0xF8) // PMVIE
            {
                export_script->Log(
                    "field:movie_set( " +
                    IntToString(GetU8(script + 1)) +
                    " );\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0xF9) // MOVIE
            {
                export_script->Log("field:play_movie();\n");
                AdvanceScript(1, script, end);
            }
            else if (opcode == 0xFA) // MVIEF
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1), GetU8(script + 2), "field:get_movie_frame()")
                );
                AdvanceScript(3, script, end);
            }
            else if (opcode == 0xFB) // MVCAM
            {
                export_script->Log(
                    "field:movie_camera_enable( " +
                    BoolToString(!(bool)(GetU8(script + 1))) +
                    " );\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0xFC) // FMUSC
            {
                export_script->Log(
                    "[UNREVERSED] FMUSC(" +
                    ArgumentString(script + 1, 1) +
                    ");\n"
                );

                AdvanceScript(2, script, end);
            }
            else if (opcode == 0xFE) // CHMST
            {
                export_script->Log(
                    SetVariable(GetU8(script + 1), GetU8(script + 2), "field:is_music_playing()")
                );
                AdvanceScript(3, script, end);
            }
            else
            {
                export_script->Log("[OPCODE " + HexToString(opcode, 2, '0') + "]\n");
                script += 1;
            }
        }

        export_script->Log("\n");
*/
}
