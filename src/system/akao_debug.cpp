#include "akao.h"
#include "akao_debug.h"

#include "ofxImGui.h"



void AkaoDebugSequence()
{
    ImGui::SetNextWindowSize( ImVec2( 500, 400 ), ImGuiCond_Once );
    ImGui::Begin( "AKAO Sequencer" );

    float width = ImGui::GetWindowContentRegionMax().x;
    int add_y = 20;
    int size = 8;
    float base_x = 5;
    float line_height = ImGui::GetTextLineHeight();
    ImVec4 color = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );

    for( size_t i = 0; i < 0x18; ++i )
    {
        if( g_channels_1_config.active_mask & (0x1 << i) )
        {
            ImGui::SetCursorPos( ImVec2( base_x, i * line_height + add_y ) );
            ImGui::TextColored( color, "Channel: %02x", i );
            add_y += line_height;

            u8* akao = g_channels_1[ i ].seq_start;

            int x = base_x;

            while( akao < g_channels_1[ i ].seq_end )
            {
                if( akao == g_channels_1[ i ].seq )
                {
                    color = ImVec4( 0.0f, 1.0f, 0.0f, 1.0f );
                }
                else
                {
                    color = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
                }

                if( x >= width )
                {
                    x = base_x;
                    add_y += 20;
                }

                u8 opcode = READ_LE_U8( akao );

                if( opcode  < 0xa0 )
                {
                    ImGui::SetCursorPos( ImVec2( x, i * line_height + add_y ) );
                    ImGui::TextColored( color, "|" );
                    x += 1 * size;
                }
                else
                {
                    switch( opcode )
                    {
                        case 0xa6:
                        case 0xa7:
                        case 0xb6:
                        case 0xc2:
                        case 0xc8:
                        case 0xcc:
                        case 0xcd:
                        case 0xd0:
                        case 0xd1:
                        {
                            color = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
                            ImGui::SetCursorPos( ImVec2( x, i * line_height + add_y ) );
                            ImGui::TextColored( color, "%02x", opcode );
                            x += 2 * size;
                        }
                        break;

                        case 0xa1:
                        case 0xa2:
                        case 0xa3:
                        case 0xa5:
                        case 0xa8:
                        case 0xaa:
                        case 0xad:
                        case 0xb5:
                        case 0xc9:
                        case 0xd8:
                        case 0xd9:
                        {
                            color = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
                            ImGui::SetCursorPos( ImVec2( x, i * line_height + add_y ) );
                            ImGui::TextColored( color, "%02x", opcode );
                            x += 2 * size;
                            akao += 1;
                        }
                        break;

                        case 0xa4:
                        case 0xa9:
                        case 0xab:
                        case 0xe8:
                        case 0xea:
                        case 0xee:
                        {
                            color = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
                            ImGui::SetCursorPos( ImVec2( x, i * line_height + add_y ) );
                            ImGui::TextColored( color, "%02x", opcode );
                            x += 2 * size;
                            akao += 2;
                        }
                        break;

                        case 0xb4:
                        {
                            color = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
                            ImGui::SetCursorPos( ImVec2( x, i * line_height + add_y ) );
                            ImGui::TextColored( color, "%02x", opcode );
                            x += 2 * size;
                            akao += 3;
                        }
                        break;

                        default:
                        {
                            color = ImVec4( 1.0f, 0.0f, 0.0f, 1.0f );
                            ImGui::SetCursorPos( ImVec2( x, i * line_height + add_y ) );
                            ImGui::TextColored( color, "%02x", opcode );
                            x += 2 * size;
                        }
                    }
                }


    //                switch( opcode )
    //                {
    //                    case 0x91:
    //                    case 0x95:
    //                    case 0x96:
    //                    case 0x99:
    //                    case 0x9a:
    //                    case 0xae:
    //                    case 0xb0:
    //                    case 0xb1:
    //                    case 0xba:
    //                    case 0xbb:
    //                    case 0xc0:
    //                    case 0xdb:
    //                    {
    //                        cur_offset += 1;
    //                        if( opcode == 0xae || opcode == 0xb0 || opcode == 0xb1 || opcode == 0xdb )
    //                        {
    //                            DEBUG_DRAW.SetColour( Ogre::ColourValue( 1.0f, 0.0f, 0.0f, 1.0f ) );
    //                        }
    //                        DEBUG_DRAW.Text( x, i * 20 + add_y, "" + HexToString( opcode, 2, '0' ) ); x += 2 * size;
    //                    }
    //                    break;
    //
    //                    case 0x80:
    //                    case 0x81:
    //                    case 0x94:
    //                    case 0x98:
    //                    case 0xa0:
    //                    case 0xa1:
    //                    case 0xa9:
    //                    case 0xac:
    //                    case 0xad:
    //                    case 0xc2:
    //                    case 0xc4:
    //                    case 0xc5:
    //                    case 0xc9:
    //                    case 0xca:
    //                    case 0xd0:
    //                    case 0xd1:
    //                    case 0xd7:
    //                    case 0xe0:
    //                    case 0xe1:
    //                    case 0xe8:
    //                    case 0xfe:
    //                    {
    //                        u8 data = m_Music->GetU8( cur_offset + 1 );
    //                        cur_offset += 2;
    //                        //DEBUG_DRAW.Text( x, i * 20 + add_y, " " + HexToString( opcode, 2, '0' ) + HexToString( data, 2, '0' ) ); x += 5 * size;
    //
    //                        if( opcode == 0xd7 || opcode == 0xfe )
    //                        {
    //                            DEBUG_DRAW.SetColour( Ogre::ColourValue( 1.0f, 0.0f, 0.0f, 1.0f ) );
    //                        }
    //                        DEBUG_DRAW.Text( x, i * 20 + add_y, "" + HexToString( opcode, 2, '0' ) ); x += 2 * size;
    //                    }
    //                    break;
    //
    //                    case 0x97:
    //                    case 0xa2:
    //                    case 0xd4:
    //                    case 0xe2:
    //                    case 0xfc:
    //                    {
    //                        u8 data1 = m_Music->GetU8( cur_offset + 1 );
    //                        u8 data2 = m_Music->GetU8( cur_offset + 2 );
    //                        cur_offset += 3;
    //                        // DEBUG_DRAW.Text( x, i * 20 + add_y, " " + HexToString( opcode, 2, '0' ) + HexToString( data1, 2, '0' ) + HexToString( data2, 2, '0' ) ); x += 7 * size;
    //
    //                        if( opcode == 0x97 )
    //                        {
    //                            DEBUG_DRAW.SetColour( Ogre::ColourValue( 1.0f, 0.0f, 0.0f, 1.0f ) );
    //                        }
    //                        DEBUG_DRAW.Text( x, i * 20 + add_y, "" + HexToString( opcode, 2, '0' ) ); x += 2 * size;
    //                    }
    //                    break;
    //
    //                    case 0xd8:
    //                    case 0xd9:
    //                    case 0xe4:
    //                    case 0xe5:
    //                    case 0xec:
    //                    case 0xed:
    //                    case 0xf8:
    //                    {
    //                        u8 data1 = m_Music->GetU8( cur_offset + 1 );
    //                        u8 data2 = m_Music->GetU8( cur_offset + 2 );
    //                        u8 data3 = m_Music->GetU8( cur_offset + 3 );
    //                        cur_offset += 4;
    //                        //DEBUG_DRAW.Text( x, i * 20 + add_y, " " + HexToString( opcode, 2, '0' ) + HexToString( data1, 2, '0' ) + HexToString( data2, 2, '0' ) + HexToString( data3, 2, '0' ) ); x += 9 * size;
    //
    //                        if( opcode == 0xd8 || opcode == 0xd9 || opcode == 0xe4 || opcode == 0xe5 || opcode == 0xec || opcode == 0xed )
    //                        {
    //                            DEBUG_DRAW.SetColour( Ogre::ColourValue( 1.0f, 0.0f, 0.0f, 1.0f ) );
    //                        }
    //                        DEBUG_DRAW.Text( x, i * 20 + add_y, "" + HexToString( opcode, 2, '0' ) ); x += 2 * size;
    //                    }
    //                    break;
    //
    //                    case 0x90:
    //                    {
    //                        cur_offset += 1;
    //                        DEBUG_DRAW.Text( x, i * 20 + add_y, "" + HexToString( opcode, 2, '0' ) ); x += 2 * size;
    //                        read = false;
    //                    }
    //                    break;
    //
    //                    default:
    //                    {
    //                        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1.0f, 0.0f, 0.0f, 1.0f ) );
    //                        DEBUG_DRAW.Text( x, i * 20 + add_y, " UNKNOWN " + HexToString( opcode, 2, '0' ) );
    //                        read = false;
    //                    }
    //                }
    //            }

                akao += 0x1;
            }

            add_y += line_height + 5;
            ImGui::SetCursorPos( ImVec2( base_x, i * line_height + add_y ) );
            add_y += 15;
            ImGui::Separator();
        }
    }

    ImGui::End();
}
