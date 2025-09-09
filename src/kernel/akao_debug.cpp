#include "akao.h"
#include "akao_debug.h"
#include "field/field.h"
#include "system/icons_font.h"
#include "psyq/filesystem.h"

#include "ofxImGui.h"
#include <format>

bool g_akao_debug = false;
bool g_akao_playing = false;

// music id from 0x1 - 0x60
std::string g_music_names_en[] =
{
    "",                                 "(empty)",                 "Opening - Bombing Mission",            "Bombing Mission",
    "",                                 "",                        "",                                     "Mako Reactor",
    "Let the Battles Begin!",           "",                        "Anxiety",                              "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "Lurking In the Darkness", "Shinra, Inc",                          "",
    "Under the Rotting Pizza",          "",                        "",                                     "Tifa's Theme",
    "",                                 "",                        "Flowers Blooming in the Church",       "",
    "",                                 "",                        "Barret's Theme",                       "",
    "",                                 "",                        "Turks' Theme ",                        "Fanfare",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "Hurry!",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "The Prelude"
};
std::string g_music_names_jp[] =
{
    "",                                 "",                        "オープニング~爆破ミッション",              "爆破ミッション",
    "",                                 "",                        "",                                     "魔晄炉",
    "闘う者達 lit. \"Those Who Fight\"", "",                        "不安な心 lit. \"Anxious Heart\"",       "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "闇に潜む",                 "神羅カンパニー lit. \"Shinra Company\"", "",
    "腐ったピザの下で",                   "",                        "",                                     "ティファのテーマ",
    "",                                 "",                        "教会に咲く花",                           "",
    "",                                 "",                        "バレットのテーマ",                       "",
    "",                                 "",                        "タークスのテーマ",                       "ファンファーレ",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "急げ!",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "",                                 "",                        "",                                     "",
    "プレリュード"
};

std::string g_reverb_name[] = { "OFF", "ROOM", "STUDIO_A", "STUDIO_B", "STUDIO_C", "HALL", "SPACE", "ECHO", "DELAY", "PIPE" };

struct MusicFileInfo
{
    std::string path;
    u16 id;
    bool is_dat;
};

struct MusicInfo
{
    u16 id;
    u16 reverb_type;
    std::string timestamp;
    std::vector<MusicFileInfo> files;
};
std::vector<MusicInfo> g_musics;



static std::string BCDByteToString( u8 bcd_byte )
{
    char high_nibble = (bcd_byte >> 4) & 0xf;
    char low_nibble = bcd_byte & 0xf;
    char high_char = '0' + high_nibble;
    char low_char = '0' + low_nibble;
    return std::string{ high_char, low_char };
}


static void AkaoDebugFillSongInfo()
{
    std::string path = "data/FIELD";
    std::vector<std::string> add_files;
    add_files.push_back( "ENEMY6/FAN2.SND" );
    add_files.push_back( "ENEMY6/OVER2.SND" );
    add_files.push_back( "MINI/ASERI2.SND" );
    add_files.push_back( "MINI/SENSUI.SND" );
    add_files.push_back( "MOVIE/OVER2.SND" );

    for( const auto& entry : std::filesystem::directory_iterator( path ) )
    {
        if( entry.is_regular_file() )
        {
            std::string filepath = entry.path().generic_string().substr( 5 );
            std::string ext = entry.path().extension().string();
            std::string filename = entry.path().filename().string();

            if( (ext == ".DAT")
                     && (filename != "BLACKBGA.DAT")
                     && (filename != "BLACKBGF.DAT")
                     && (filename != "BLACKBGG.DAT")
                     && (filename != "BLIN69_2.DAT")
                     && (filename != "CONVIL_3.DAT")
                     && (filename != "FALLP.DAT")
                     && (filename != "FSHIP_26.DAT")
                     && (filename != "HYOU14.DAT")
                     && (filename != "JUNMON.DAT")
                     && (filename != "M_ENDO.DAT")
                     && (filename != "ONNA_1.DAT")
                     && (filename != "ONNA_3.DAT")
                     && (filename != "ONNA_6.DAT")
                     && (filename != "PASS.DAT")
                     && (filename != "Q_5.DAT")
                     && (filename != "SUBIN_4.DAT")
                     && (filename != "TRAP.DAT")
                     && (filename != "WHITEBG1.DAT")
                     && (filename != "WHITEBG2.DAT")
                     && (filename.compare( 0, 2, "WM" ) != 0)
                     && (filename != "XMVTES.DAT")

                )
            {
                std::vector<u8> field_dat;
                FileLZS( filepath, field_dat );
                u32 events_addr = READ_LE_U32( &field_dat[0x0] ) - g_field_dat_base_addr;
                u8 entity_n = READ_LE_U8( &field_dat[events_addr + 0x2] );
                u16 music_n = READ_LE_U16( &field_dat[events_addr + 0x6] );

                for( int i = 0; i < music_n; ++i )
                {
                    u32 music_offset = events_addr + READ_LE_U32( &field_dat[events_addr + 0x20 + entity_n * 0x8 + i * 0x4] );

                    u8 check0 = READ_LE_U8( &field_dat[music_offset + 0x0] );
                    u8 check1 = READ_LE_U8( &field_dat[music_offset + 0x1] );
                    u8 check2 = READ_LE_U8( &field_dat[music_offset + 0x2] );
                    u8 check3 = READ_LE_U8( &field_dat[music_offset + 0x3] );

                    // if this is AKAO file (in ffvii tutorial files and misuc are together)
                    if( (check0 == 'A') && (check1 == 'K') && (check2 == 'A') && (check3 == 'O') )
                    {
                        MusicFileInfo file_info;
                        file_info.path = filepath;
                        file_info.id = i;
                        file_info.is_dat = true;

                        u16 id_to_find = READ_LE_U16( &field_dat[music_offset + 0x4] );

                        auto it = std::find_if( g_musics.begin(), g_musics.end(),
                            [id_to_find](const MusicInfo& m)
                            {
                                return m.id == id_to_find;
                            });

                        if( it != g_musics.end() )
                        {
                            MusicInfo& found_music = *it;
                            found_music.files.push_back( file_info );
                        }
                        else
                        {
                            MusicInfo music;
                            music.id = READ_LE_U16( &field_dat[music_offset + 0x4] );
                            music.reverb_type = READ_LE_U16( &field_dat[music_offset + 0x8] );
                            music.timestamp =
                                       "19" + BCDByteToString( READ_LE_U16( &field_dat[music_offset + 0xa] ) )
                                      + "-" + BCDByteToString( READ_LE_U16( &field_dat[music_offset + 0xb] ) )
                                      + "-" + BCDByteToString( READ_LE_U16( &field_dat[music_offset + 0xc] ) )
                                      + " " + BCDByteToString( READ_LE_U16( &field_dat[music_offset + 0xd] ) )
                                      + ":" + BCDByteToString( READ_LE_U16( &field_dat[music_offset + 0xe] ) )
                                      + ":" + BCDByteToString( READ_LE_U16( &field_dat[music_offset + 0xf] ) );
                            music.files.push_back( file_info );
                            g_musics.push_back( music );
                        }
                    }
                }
            }
        }
    }

    for( int i = 0; i < add_files.size(); ++i )
    {
        std::vector<u8> snd;
        FileRead( add_files[i], snd );

        u8 check0 = READ_LE_U8( &snd[0x0] );
        u8 check1 = READ_LE_U8( &snd[0x1] );
        u8 check2 = READ_LE_U8( &snd[0x2] );
        u8 check3 = READ_LE_U8( &snd[0x3] );

        // if this is AKAO file (in ffvii tutorial files and misuc are together)
        if( (check0 == 'A') && (check1 == 'K') && (check2 == 'A') && (check3 == 'O') )
        {
            MusicFileInfo file_info;
            file_info.path = add_files[i];
            file_info.id = 0;
            file_info.is_dat = false;

            u16 id_to_find = READ_LE_U16( &snd[0x4] );

            auto it = std::find_if( g_musics.begin(), g_musics.end(),
                [id_to_find](const MusicInfo& m)
                {
                    return m.id == id_to_find;
                });

            if( it != g_musics.end() )
            {
                MusicInfo& found_music = *it;
                found_music.files.push_back( file_info );
            }
            else
            {
                MusicInfo music;
                music.id = READ_LE_U16( &snd[0x4] );
                music.reverb_type = READ_LE_U16( &snd[0x8] );
                music.timestamp =
                           "19" + BCDByteToString( READ_LE_U16( &snd[0xa] ) )
                          + "-" + BCDByteToString( READ_LE_U16( &snd[0xb] ) )
                          + "-" + BCDByteToString( READ_LE_U16( &snd[0xc] ) )
                          + " " + BCDByteToString( READ_LE_U16( &snd[0xd] ) )
                          + ":" + BCDByteToString( READ_LE_U16( &snd[0xe] ) )
                          + ":" + BCDByteToString( READ_LE_U16( &snd[0xf] ) );
                music.files.push_back( file_info );
                g_musics.push_back( music );
            }
        }
    }

    std::sort( g_musics.begin(), g_musics.end(),
        []( const MusicInfo& a, const MusicInfo& b )
        {
            return a.id < b.id;
        });

    for( int i = 0; i < g_musics.size(); ++i )
    {
        std::sort( g_musics[i].files.begin(), g_musics[i].files.end(),
            []( const MusicFileInfo& a, const MusicFileInfo& b )
            {
                return a.path < b.path;
            });
    }
}



void AkaoDebug()
{
    if( g_akao_debug == false ) return;

    ImGui::SetNextWindowSize( ImVec2( 900, 650 ), ImGuiCond_Once );
    if( ImGui::Begin( "Akao Debug", &g_akao_debug, ImGuiWindowFlags_NoCollapse ) )
    {
        if( ImGui::BeginTabBar( "AkaoDebug" ) )
        {
            if( ImGui::BeginTabItem( "SND Browser" ) )
            {
                AkaoDebugSndBrowser();
                ImGui::EndTabItem();
            }

            if( ImGui::BeginTabItem( "SFX Browser" ) )
            {
                ImGui::Text("This is content 2.");
                ImGui::EndTabItem();
            }

            if( ImGui::BeginTabItem( "Instruments Browser" ) )
            {
                AkaoDebugInstr();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();   // закрыли бар
        }
    }
    ImGui::End();

    if( g_akao_playing ) AkaoDebugSequence();
}



void AkaoDebugSndBrowser()
{
    ImGui::Columns( 2, nullptr, true ); // 2 колонки, ресайз между ними
    // do not reset column width every frame
    static bool first_frame = true;
    if( first_frame )
    {
        ImGui::SetColumnWidth( 0, 250 );
        first_frame = false;

        AkaoDebugFillSongInfo();
    }

    //static char search[128];
    //ImGui::TextUnformatted( ICON_FA_SEARCH );
    //ImGui::SameLine();
    //ImGui::InputTextWithHint( "##search", "Type to filter...", search, IM_ARRAYSIZE( search ) );

    static int selected_id = 0;

    if( ImGui::BeginListBox( "##AkaoList", ImVec2( -FLT_MIN, -FLT_MIN ) ) )
    {
        ImGui::TextDisabled( "Music List" );
        ImGui::Separator();

        for( int i = 0; i < g_musics.size(); ++i )
        {
            ImGui::PushID( i );

            char label[256];
            snprintf( label, sizeof( label ), "0x%02x: %s", g_musics[i].id, g_music_names_en[g_musics[i].id].c_str() );
            if( ImGui::Selectable( label, (i == selected_id) ) )
            {
                selected_id = i;
            }

            ImGui::PopID();
        }
        ImGui::EndListBox();
    }

    ImGui::NextColumn();
    ImGui::BeginChild( "right_panel", ImVec2( 0, 0 ), true );

    if( g_musics.size() != 0 )
    {
        ImGui::Text( "Id: 0x%02x", g_musics[selected_id].id );
        if( g_music_names_jp[g_musics[selected_id].id] != "" )
        {
            ImGui::Text( "Name: %s (jp. %s)", g_music_names_en[g_musics[selected_id].id].c_str(), g_music_names_jp[g_musics[selected_id].id].c_str() );
        }
        else
        {
            ImGui::Text( "Name: %s", g_music_names_en[g_musics[selected_id].id].c_str() );
        }
        ImGui::Separator();
        ImGui::Text( "Music Info:" );
        ImGui::Text( "    Reverb type: 0x%02x (%s)", g_musics[selected_id].reverb_type, g_reverb_name[g_musics[selected_id].reverb_type].c_str() );
        ImGui::Text( "    Timestamp: %s", g_musics[selected_id].timestamp.c_str() );
        ImGui::Separator();
        ImGui::Text( "Used in files:" );
        for( int i = 0; i < g_musics[selected_id].files.size(); ++i )
        {
            // remove "data/" to clean output
            ImGui::Text( "    %s (%d)", g_musics[selected_id].files[i].path.c_str(), g_musics[selected_id].files[i].id );
            ImGui::SameLine();

            ImGui::PushID( i );
            if( ImGui::Button( ICON_FA_PLAY ) )
            {
                if( g_musics[selected_id].files[i].is_dat )
                {
                    std::vector<u8> field_dat;
                    FileLZS( g_musics[selected_id].files[i].path, field_dat );
                    u32 events_addr = READ_LE_U32( &field_dat[0x0] ) - g_field_dat_base_addr;
                    u8 entity_n = READ_LE_U8( &field_dat[events_addr + 0x2] );
                    u32 music_offset = events_addr + READ_LE_U32( &field_dat[events_addr + 0x20 + entity_n * 0x8 + g_musics[selected_id].files[i].id * 0x4] );
                    u16 music_size = READ_LE_U16( &field_dat[music_offset + 0x6] );
                    AkaoCopyMusic( &field_dat[music_offset + 0x10], music_size );
                }
                else
                {
                    std::vector<u8> snd;
                    FileRead( g_musics[selected_id].files[i].path, snd );
                    u16 music_size = READ_LE_U16( &snd[0x6] );
                    AkaoCopyMusic( &snd[0x10], music_size );
                }

                AkaoSetReverbMode( g_musics[selected_id].reverb_type );
                AkaoMusicChannelsInit();

                g_akao_playing = true;
            }
            ImGui::PopID();
        }
    }

    ImGui::EndChild();

    ImGui::Columns( 1 );
}



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
