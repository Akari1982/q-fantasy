#include "browser_ending.h"
#include "system/icons_font.h"
#include "kernel/file.h"

#include "ofxImGui.h"
#include <format>

bool g_browser_ending = false;

std::vector<u8> l_ending_file;
u8 l_event_id = 0;
void ParseEndingEvents( u8 id );

std::vector<u8> l_opening_file;



void BrowserEnding()
{
    if( g_browser_ending == false ) return;

    static bool loaded = false;
    if( loaded == false )
    {
        //FileLZS( "FIELD/ENDING.X", l_dat_file );
        FileRead( "FIELD/ENDING.X", l_ending_file );
        FileRead( "MOVIE/OPENING.BIN", l_opening_file );
        {
            u32 part = FileGetPackPointer(l_opening_file, 0);
            std::vector<u8> part_file(l_opening_file.begin() + part, l_opening_file.end());
            std::vector<u8> extracted;
            FileLZSExtract(part_file, extracted);
            FileWrite("0.tim", extracted);
        }
        {
            u32 part = FileGetPackPointer( l_opening_file, 1 );
            std::vector<u8> part_file(l_opening_file.begin() + part, l_opening_file.end());
            std::vector<u8> extracted;
            FileLZSExtract( part_file, extracted );
            FileWrite( "1.tim", extracted );
        }
        {
            u32 part = FileGetPackPointer( l_opening_file, 2 );
            std::vector<u8> part_file(l_opening_file.begin() + part, l_opening_file.end());
            std::vector<u8> extracted;
            FileLZSExtract( part_file, extracted );
            FileWrite( "2.tim", extracted );
        }
        {
            u32 part = FileGetPackPointer( l_opening_file, 3 );
            std::vector<u8> part_file(l_opening_file.begin() + part, l_opening_file.end());
            std::vector<u8> extracted;
            FileLZSExtract( part_file, extracted );
            FileWrite( "3.tim", extracted );
        }
        {
            u32 part = FileGetPackPointer( l_opening_file, 4 );
            std::vector<u8> part_file(l_opening_file.begin() + part, l_opening_file.end());
            std::vector<u8> extracted;
            FileLZSExtract( part_file, extracted );
            FileWrite( "4.tim", extracted );
        }
        {
            u32 part = FileGetPackPointer( l_opening_file, 5 );
            std::vector<u8> part_file(l_opening_file.begin() + part, l_opening_file.end());
            std::vector<u8> extracted;
            FileLZSExtract( part_file, extracted );
            FileWrite( "5.tim", extracted );
        }
        {
            u32 part = FileGetPackPointer( l_opening_file, 6 );
            std::vector<u8> part_file(l_opening_file.begin() + part, l_opening_file.end());
            std::vector<u8> extracted;
            FileLZSExtract( part_file, extracted );
            FileWrite( "6.tim", extracted );
        }
        {
            u32 part = FileGetPackPointer( l_opening_file, 7 );
            std::vector<u8> part_file(l_opening_file.begin() + part, l_opening_file.end());
            std::vector<u8> extracted;
            FileLZSExtract( part_file, extracted );
            FileWrite( "7.tim", extracted );
        }
        {
            u32 part = FileGetPackPointer( l_opening_file, 0x11 );
            std::vector<u8> part_file(l_opening_file.begin() + part, l_opening_file.end());
            std::vector<u8> extracted;
            FileLZSExtract( part_file, extracted );
            FileWrite( "0x11.tim", extracted );
        }
        {
            u32 part = FileGetPackPointer( l_opening_file, 0x12 );
            std::vector<u8> part_file(l_opening_file.begin() + part, l_opening_file.end());
            std::vector<u8> extracted;
            FileLZSExtract( part_file, extracted );
            FileWrite( "0x12.tim", extracted );
        }
        loaded = true;
    }

    ImGui::SetNextWindowPos( ImGui::GetWindowPos() + ImVec2( 0, 0 ), ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowSize( ImVec2( 900, 650 ), ImGuiCond_Once );
    if( ImGui::Begin( "Ending Browser", &g_browser_ending, ImGuiWindowFlags_NoCollapse ) )
    {
        ImGui::BeginDisabled( l_event_id == 0 );
        if( ImGui::Button( "Event 0" ) )
        {
            l_event_id = 0;
        }
        ImGui::EndDisabled();

        ImGui::SameLine();

        ImGui::BeginDisabled( l_event_id == 1 );
        if( ImGui::Button( "Event 1" ) )
        {
            l_event_id = 1;
        }
        ImGui::EndDisabled();

        ParseEndingEvents( l_event_id );
    }
    ImGui::End();
}



void ParseEndingEvents( u8 id )
{
    u32 event_ofs = (id == 0) ? 0x3934 : 0x5048;

    u32 end_ofs = event_ofs;
    u16 opcode = READ_LE_U16( &l_ending_file[event_ofs] );

    for (; event_ofs <= end_ofs;)
    {
        u16 opcode = READ_LE_U16( &l_ending_file[event_ofs] );

        ImGui::Text( "  0x%04x    ", event_ofs );
        ImGui::SameLine( 0, 0 );

        switch( opcode )
        {
            case 0x1:
            {
                ImGui::Text( "LOAD_FILE( " ); ImGui::SameLine( 0, 0 );

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " file:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );

                std::string file;
                u16 id = READ_LE_U16( &l_ending_file[event_ofs + 0x2] );
                if( id == 0 ) file = "MOVIE/STAFF.BIN";
                else if( id == 1 ) file = "MOVIE/STAFF2.BIN";
                else file = "MOVIE/OPENING.BIN";

                ImGui::Text( "\"%s\"", file.c_str() ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::Text( " )" );

                event_ofs += 0x4;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0x3:
            {
                ImGui::Text( "LOAD_FILE_SYNC()" );

                event_ofs += 0x2;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0x5:
            {
                ImGui::Text( "LOAD_TIM_FROM_LZS( " ); ImGui::SameLine( 0, 0 );

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " pack_id:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x2] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::Text( " )" );

                event_ofs += 0x4;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0x6:
            {
                ImGui::Text( "PLAY_MOVIE( " ); ImGui::SameLine( 0, 0 );

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " id:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x2] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::Text( " )" );

                event_ofs += 0x4;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0x7:
            {
                ImGui::Text( "MOVIE_SYNC()" );

                event_ofs += 0x2;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0x8:
            {
                ImGui::Text( "RENDER_INIT( " ); ImGui::SameLine( 0, 0 );

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " a:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x2] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( ", b:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x4] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( ", c:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x6] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( ", d:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x8] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( ", e:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0xa] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::Text( " )" );

                event_ofs += 0xe;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0x9:
            {
                ImGui::Text( "SET_DISP_MASK( " ); ImGui::SameLine( 0, 0 );

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " mask:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x2] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::Text( " )" );

                event_ofs += 0x4;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0xa:
            {
                ImGui::Text( "WAIT( " ); ImGui::SameLine( 0, 0 );

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " wait:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x2] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::Text( " )" );

                event_ofs += 0x4;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0xc:
            {
                ImGui::Text( "LIST_4_ITEM_REMOVE()" );

                event_ofs += 0x2;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0xf:
            {
                ImGui::Text( "SET_COLOR_CHANGE( " ); ImGui::SameLine( 0, 0 );

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " data_id:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x2] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( ", steps:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x4] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( ", desc_col:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x6] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::Text( " )" );

                event_ofs += 0x8;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0x10:
            {
                ImGui::Text( "FRAME_SKIP( " ); ImGui::SameLine( 0, 0 );

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " skip:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x2] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::Text( " )" );

                event_ofs += 0x4;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0x11:
            {
                ImGui::Text( "RENDER_INIT_2()" );

                event_ofs += 0x2;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0x12:
            {
                ImGui::Text( "OP_12( " ); ImGui::SameLine( 0, 0 );

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " file_id:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x2] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( ", ???:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x4] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::Text( " ) // setup struct 0x88" );

                event_ofs += 0x6;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0x13:
            {
                ImGui::Text( "RETURN()" );

                event_ofs += 0x2;
            }
            break;

            case 0x14:
            {
                ImGui::Text( "RENDER_INIT_3()" );

                event_ofs += 0x2;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0x15:
            {
                ImGui::Text( "OP_15( " ); ImGui::SameLine( 0, 0 );

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " data_id:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x2] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( ", pack_id:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x4] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( ", c:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x6] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( ", d:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x8] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( ", x:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0xa] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " y:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0xc] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " z:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0xe] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " r:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x10] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " g:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x12] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " b:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x14] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();
                ImGui::Text( " ) // init struct 0x88" );

                event_ofs += 0x16;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0x16:
            {
                ImGui::Text( "OP_16( " ); ImGui::SameLine( 0, 0 );

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " id:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x2] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::Text( " ) // set 0x0 for given data id to 0x0000" );

                event_ofs += 0x4;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0x17:
            {
                ImGui::Text( "OP_17( " ); ImGui::SameLine( 0, 0 );

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " data_id:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x2] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( ", x:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x4] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( ", y:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x6] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( ", z:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x8] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( ", scale:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0xa] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::Text( " ) // setup struct 0x88" );

                event_ofs += 0xc;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0x18:
            {
                ImGui::Text( "PLAY_MUSIC( " ); ImGui::SameLine( 0, 0 );

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " id:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x2] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::Text( " )" );

                event_ofs += 0x4;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0x19:
            {
                ImGui::Text( "OP_19_SYNC()" );

                event_ofs += 0x2;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0x1a:
            {
                ImGui::Text( "AKAO( " ); ImGui::SameLine( 0, 0 );

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " command:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x2] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( ", arg1:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x4] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( ", arg2:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x6] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::Text( " )" );

                event_ofs += 0x8;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0x1b:
            {
                ImGui::Text( "OP_1B_CHECK( " ); ImGui::SameLine( 0, 0 );

                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5, 0.5, 0.5, 1 ) );
                ImGui::Text( " check:" ); ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0.5, 0, 1 ) );
                ImGui::Text( "0x%04x", READ_LE_U16( &l_ending_file[event_ofs + 0x2] ) ); ImGui::SameLine( 0, 0 );
                ImGui::PopStyleColor();

                ImGui::Text( " )" );

                event_ofs += 0x4;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0x1c:
            {
                ImGui::Text( "INIT_LINES()" );

                event_ofs += 0x2;
                end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;
            }
            break;

            case 0x1d:
            {
                ImGui::Text( "DRAW_LINES()" );

                event_ofs += 0x2;
            }
            break;

            default:
            {
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 0, 0, 1 ) );
                ImGui::Text( "Failed to decode opcode 0x%04x", opcode );
                ImGui::PopStyleColor();
                event_ofs += 0x2;
            }
        }
    }
}
