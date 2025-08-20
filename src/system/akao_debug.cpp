#include "akao.h"
#include "akao_debug.h"

#include "ofxImGui.h"



void AkaoDebugSequence()
{
    ImGui::SetNextWindowSize( ImVec2( 600, 500 ), ImGuiCond_Once );
    ImGui::Begin( "AKAO Sequencer" );

    float width = ImGui::GetWindowContentRegionMax().x;
    int add_y = 20;
    int size = 8;
    float base_x = 5;
    float line_height = ImGui::GetTextLineHeight();
    ImVec4 color = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );

    u32 channels_mask = READ_LE_U32( g_akao_music );

    for( size_t i = 0; i < 0x18; ++i )
    {
        if( channels_mask & (0x1 << i) )
        {
            ImGui::SetCursorPos( ImVec2( base_x, i * line_height + add_y ) );
            ImGui::TextColored( color, "Channel: %02x", i );
            add_y += line_height;

            u8* akao = g_channels_1[ i ].seq_start;

            int x = base_x;
            bool read = true;

            while( read && (akao < g_channels_1[ i ].seq_end) )
            {
                if( akao == g_channels_1[ i ].seq )
                {
                    color = ImVec4( 0.0f, 1.0f, 0.0f, 1.0f );
                }
                else
                {
                    if( g_channels_1_config.active_mask & (0x1 << i) )
                    {
                        color = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
                    }
                    else
                    {
                        color = ImVec4( 0.3f, 0.3f, 0.3f, 1.0f );
                    }

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
                        case 0xa0:
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
                            ImGui::SetCursorPos( ImVec2( x, i * line_height + add_y ) );
                            ImGui::TextColored( color, "%02x", opcode );
                            x += 2 * size;

                            if( opcode == 0xa0 ) read = false;
                        }
                        break;

                        case 0xa1:
                        case 0xa2:
                        case 0xa3:
                        case 0xa5:
                        case 0xa8:
                        case 0xaa:
                        case 0xad:
                        case 0xae:
                        case 0xaf:
                        case 0xb1:
                        case 0xb5:
                        case 0xc9:
                        case 0xd8:
                        case 0xd9:
                        {
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
                            ImGui::SetCursorPos( ImVec2( x, i * line_height + add_y ) );
                            ImGui::TextColored( color, "%02x", opcode );
                            x += 2 * size;
                            akao += 2;

                            if( opcode == 0xee ) read = false;
                        }
                        break;

                        case 0xb4:
                        {
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
