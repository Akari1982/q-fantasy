#include "akao.h"
#include "akao_debug.h"

#include "ofxImGui.h"

#include <format>



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
    ImGui::TextColored( ImVec4( 1.0f, 1.0f, 1.0f, 1.0f ), "PLAY MASK:%02x", g_channels_1_config.keyed_mask );
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
            if( g_channels_1[i].update_flags & AKAO_UPDATE_VIBRATO ) info += std::string( " VIBRATO:" ) + std::to_string( g_channels_1[i].vibrato_type );
            if( g_channels_1[i].update_flags & AKAO_UPDATE_TREMOLO ) info += std::string( " TREMOLO:" ) + std::to_string( g_channels_1[i].tremolo_type );
            if( g_channels_1[i].update_flags & AKAO_UPDATE_OVERLAY ) info += std::string( " OVER:" ) + std::to_string( g_channels_1[i].over_voice_id );
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
                        case 0xc6:
                        case 0xc7:
                        case 0xc8:
                        case 0xca:
                        case 0xcc:
                        case 0xcd:
                        case 0xd0:
                        case 0xd1:
                        case 0xed:
                        case 0xf4:
                        case 0xf5:
                        case 0xf9:
                        {
                            ImGui::SetCursorPos( ImVec2( x, add_y ) );
                            ImGui::TextColored( color, "%02x", opcode );
                            x += 2 * size;

                            if( opcode == 0xa0 ) read = false;
                            if( opcode == 0xca ) read = false;
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
                        case 0xb7:
                        case 0xb9:
                        case 0xbb:
                        case 0xbd:
                        case 0xbf:
                        case 0xc0:
                        case 0xc1:
                        case 0xc9:
                        case 0xd8:
                        case 0xd9:
                        case 0xf2:
                        case 0xf6:
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
                        case 0xbc:
                        case 0xdd:
                        case 0xde:
                        case 0xe8:
                        case 0xea:
                        case 0xec:
                        case 0xee:
                        case 0xfd:
                        case 0xf7:
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



void AkaoDebugInstr()
{
    ImGui::SetNextWindowSize( ImVec2( 600, 500 ), ImGuiCond_Once );
    ImGui::Begin( "AKAO Instruments" );

    static int instr_id = 0;

    ImGui::BeginChild( "LeftPane", ImVec2( 100, 0 ), true );
    for( int i = 0; i < 0x80; ++i )
    {
        // Элемент списка (например, кнопка или selectable)
        if( ImGui::Selectable( ("0x" + std::format( "{:0x}", i )).c_str(), (i == instr_id) ) )
        {
            instr_id = i;
        }
    }
    ImGui::EndChild();

    // Правая часть (занимает всё оставшееся место)
    ImGui::SameLine();
    ImGui::BeginChild( "RightPane", ImVec2( 0, 0 ), true ); // 0,0 = вся оставшаяся область

    ImGui::Text( "Instrument:    0x%02x", instr_id );
    ImGui::Text( "Start address: 0x%02x", g_akao_instrument[instr_id].addr );
    ImGui::Text( "Loop address:  0x%02x", g_akao_instrument[instr_id].loop_addr );
    ImGui::Text( "Attack mode:   0x%02x", g_akao_instrument[instr_id].a_mode );
    ImGui::Text( "Sustain mode:  0x%02x", g_akao_instrument[instr_id].s_mode );
    ImGui::Text( "Release mode:  0x%02x", g_akao_instrument[instr_id].r_mode );
    ImGui::Text( "Attack rate:   0x%02x", g_akao_instrument[instr_id].ar );
    ImGui::Text( "Decay rate:    0x%02x", g_akao_instrument[instr_id].dr );
    ImGui::Text( "Sustain level: 0x%02x", g_akao_instrument[instr_id].sl );
    ImGui::Text( "Sustain rate:  0x%02x", g_akao_instrument[instr_id].sr );
    ImGui::Text( "Release rate:  0x%02x", g_akao_instrument[instr_id].rr );

    float button_w = 100.0f;
    float button_h = 50.0f;
    float panel_w = ImGui::GetContentRegionAvail().x;
    float panel_h = ImGui::GetContentRegionAvail().y;
    ImGui::SetCursorPosX( ImGui::GetCursorPosX() + panel_w - button_w );
    ImGui::SetCursorPosY( ImGui::GetCursorPosY() + panel_h - button_h );

    if( ImGui::Button( "PLAY" ) )
    {
        s32 voice_id = 0;
        PsyqSpuSetVoicePitch( voice_id, g_akao_instrument[instr_id].pitch[0] );
        PsyqSpuSetVoiceVolume( voice_id, (s16)0x3f80, (s16)0x4080 );
        PsyqSpuSetVoiceStartAddr( voice_id, g_akao_instrument[instr_id].addr );
        PsyqSpuSetVoiceLoopStartAddr( voice_id, g_akao_instrument[instr_id].loop_addr );
        PsyqSpuSetVoiceSRAttr( voice_id, g_akao_instrument[instr_id].sr, g_akao_instrument[instr_id].s_mode );
        PsyqSpuSetVoiceARAttr( voice_id, g_akao_instrument[instr_id].ar, g_akao_instrument[instr_id].a_mode );
        PsyqSpuSetVoiceRRAttr( voice_id, g_akao_instrument[instr_id].rr, g_akao_instrument[instr_id].r_mode );
        PsyqSpuSetVoiceDR( voice_id, g_akao_instrument[instr_id].dr );
        PsyqSpuSetVoiceSL( voice_id, g_akao_instrument[instr_id].sl );

        PsyqSpuSetKey( SPU_ON, 0x1 << voice_id );
    }

    ImGui::EndChild();

    ImGui::End();
}
