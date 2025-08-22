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

    ImGui::SetCursorPos( ImVec2( base_x, add_y ) );
    ImGui::TextColored( ImVec4( 1.0f, 1.0f, 1.0f, 1.0f ), "PL:%02x ON:%02x OFF%02x", g_channels_1_config.keyed_mask, g_channels_1_config.on_mask, g_channels_1_config.off_mask );
    add_y += line_height;
    if( g_channels_1_config.timer_lower != 0 )
    {
        ImGui::SetCursorPos( ImVec2( base_x, add_y ) );
        ImGui::TextColored( ImVec4( 1.0f, 1.0f, 1.0f, 1.0f ), "%02x:%02x:%02x", g_channels_1_config.timer_top_cur, g_channels_1_config.timer_upper_cur, g_channels_1_config.timer_lower_cur );
        add_y += line_height;
    }

    add_y += line_height + 5;
    ImGui::SetCursorPos( ImVec2( base_x, add_y ) );
    add_y += 10;
    ImGui::Separator();

    for( size_t i = 0; i < 0x18; ++i )
    {
        if( channels_mask & (0x1 << i) )
        {
            std::string info = "Channel: %02x";
            if( g_channels_1[i].update_flags & AKAO_UPDATE_ALTERNATIVE )
            {
                info += std::string( " ALT" );
                info += ( g_channels_1[i].update_flags & AKAO_UPDATE_ALTERNATIVE_CUR ) ? std::string( "2" ) : std::string( "1" );
            }
            if( g_channels_1[i].update_flags & AKAO_UPDATE_DRUM_MODE ) info += std::string( " DRUM" );
            if( g_channels_1[i].update_flags & AKAO_UPDATE_VIBRATO ) info += std::string( " VIBRATO_" ) + std::to_string( g_channels_1[i].vibrato_type );
            if( g_channels_1[i].update_flags & AKAO_UPDATE_TREMOLO ) info += std::string( " TREMOLO_" ) + std::to_string( g_channels_1[i].tremolo_type );
            if( g_channels_1[i].update_flags & AKAO_UPDATE_OVERLAY ) info += std::string( " OVER_" ) + std::to_string( g_channels_1[i].over_voice_id );
            if( g_channels_1[i].pitch_slide_steps_cur != 0 ) info += std::string( " PITCH SLIDE:" ) + std::to_string( g_channels_1[i].pitch_slide_steps_cur );

            ImGui::SetCursorPos( ImVec2( base_x, add_y ) );
            ImGui::TextColored( ImVec4( 1.0f, 1.0f, 1.0f, 1.0f ), info.c_str(), i );
            add_y += line_height;

            u8* akao = g_channels_1[i].seq_start;

            int x = base_x;
            bool read = true;

            while( read && (akao < g_channels_1[i].seq_end) )
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
                    ImGui::SetCursorPos( ImVec2( x, add_y ) );
                    if( opcode < 0x84 ) ImGui::TextColored( color, "|" );
                    else if( opcode >= 0x8f ) ImGui::TextColored( color, ":" );
                    else ImGui::TextColored( color, "." );
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
                        case 0xba:
                        case 0xc2:
                        case 0xc3:
                        case 0xc8:
                        case 0xcc:
                        case 0xcd:
                        case 0xd0:
                        case 0xd1:
                        case 0xed:
                        case 0xf9:
                        {
                            ImGui::SetCursorPos( ImVec2( x, add_y ) );
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
                        case 0xb2:
                        case 0xb5:
                        case 0xb9:
                        case 0xc0:
                        case 0xc1:
                        case 0xc9:
                        case 0xd8:
                        case 0xd9:
                        case 0xf2:
                        case 0xf8:
                        {
                            ImGui::SetCursorPos( ImVec2( x, add_y ) );
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
                        case 0xec:
                        case 0xee:
                        case 0xfd:
                        case 0xfe:
                        {
                            ImGui::SetCursorPos( ImVec2( x, add_y ) );
                            ImGui::TextColored( color, "%02x", opcode );
                            x += 2 * size;
                            akao += 2;

                            if( opcode == 0xee ) read = false;
                        }
                        break;

                        case 0xb4:
                        case 0xb8:
                        case 0xf0:
                        {
                            ImGui::SetCursorPos( ImVec2( x, add_y ) );
                            ImGui::TextColored( color, "%02x", opcode );
                            x += 2 * size;
                            akao += 3;
                        }
                        break;

                        default:
                        {
                            color = ImVec4( 1.0f, 0.0f, 0.0f, 1.0f );
                            ImGui::SetCursorPos( ImVec2( x, add_y ) );
                            ImGui::TextColored( color, "%02x", opcode );
                            x += 2 * size;
                        }
                    }
                }

                akao += 0x1;
            }

            channels_mask ^= (0x1 << i);

            if( channels_mask != 0 )
            {
                add_y += line_height + 5;
                ImGui::SetCursorPos( ImVec2( base_x, add_y ) );
                add_y += 10;
                ImGui::Separator();
            }

            add_y += line_height;
        }
    }

    ImGui::End();
}
