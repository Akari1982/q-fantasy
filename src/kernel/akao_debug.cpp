#include "akao.h"
#include "akao_debug.h"
#include "field/field.h"
#include "system/icons_font.h"
#include "psyq/filesystem.h"

#include "ofxImGui.h"
#include <format>

bool g_akao_debug = false;
bool g_akao_sequencer = false;
bool g_akao_sequencer_prev = false;
enum PlayingState
{
    PLAY,
    PAUSE,
    STOP,
};
PlayingState g_akao_playing = STOP;
u32 g_akao_playing_music = 0;
u32 g_akao_playing_file = 0;

// music id from 0x1 - 0x60
std::string g_music_names_en[] =
{
    "",                                 "(empty)",                          "Opening - Bombing Mission",            "Bombing Mission",
    "In Search of the Man in Black",    "",                                 "",                                     "Mako Reactor",
    "Let the Battles Begin!",           "",                                 "Anxiety",                              "Fight On!",
    "",                                 "",                                 "Main Theme",                           "",
    "",                                 "Rufus's Welcoming Ceremony",       "",                                     "Who...Are You?",
    "On Our Way",                       "It's Hard to Stand on Both Feet!", "",                                     "Waltz de Chocobo",
    "Don of the Slums",                 "Trail of Blood",                   "Red XIII's Theme",                     "",
    "",                                 "Lurking In the Darkness",          "Shinra, Inc",                          "Infiltrating Shinra",
    "Under the Rotting Pizza",          "Farm Boy",                         "On That Day, Five Years Ago",          "Tifa's Theme",
    "",                                 "The Oppressed",                    "Flowers Blooming in the Church",       "The Chase",
    "",                                 "",                                 "Barret's Theme",                       "",
    "",                                 "Electric de Chocobo",              "Turks' Theme ",                        "Fanfare",
    "",                                 "",                                 "",                                     "",
    "Cinco de Chocobo",                 "JENOVA",                           "",                                     "Hurry!",
    "",                                 "",                                 "Good Night, Until Tomorrow",           "",
    "",                                 "",                                 "",                                     "",
    "",                                 "Dear to the Heart",                "",                                     "",
    "Honeybee Inn",                     "",                                 "",                                     "",
    "",                                 "",                                 "",                                     "",
    "",                                 "",                                 "",                                     "",
    "",                                 "Fort Condor",                      "",                                     "",
    "",                                 "",                                 "",                                     "",
    "",                                 "",                                 "",                                     "",
    "",                                 "",                                 "",                                     "",
    "The Prelude",                      "",                                 "",                                     "",
    "",                                 "",                                 "",                                     "",
    "",                                 "",                                 "",                                     "",
    "",                                 "",                                 "",                                     ""
};
std::string g_music_names_jp[] =
{
    "",                                 "",                                 "オープニング~爆破ミッション",                 "爆破ミッション",
    "黒マントの男を追え",                  "",                               "",                                     "魔晄炉",
    "闘う者達 lit. \"Those Who Fight\"", "",                                "不安な心 lit. \"Anxious Heart\"",       "更に闘う者達 lit. \"Those Who Fight Further\"",
    "",                                 "",                                 "メインテーマ",                             "",
    "",                                 "ルーファウス歓迎式典",               "",                                     "お前は…誰だ",
    "旅の途中で",                         "二本足で立つのも難しいものだな",     "",                                     "ワルツ・デ・チョコボ",
    "スラムのドン",                         "血の跡",                          "レッドXIIIのテーマ",                       "",
    "",                                 "闇に潜む",                         "神羅カンパニー lit. \"Shinra Company\"", "神羅ビル潜入",
    "腐ったピザの下で",                     "牧場の少年",                     "5年前のあの日",                         "ティファのテーマ",
    "",                                 "虐げられた民衆",                  "教会に咲く花",                           "クレイジーモーターサイクル lit. \"Crazy Motorcycle\"",
    "",                                 "",                                 "バレットのテーマ",                         "",
    "",                                 "エレキ・デ・チョコボ",               "タークスのテーマ",                         "ファンファーレ",
    "",                                 "",                                 "",                                     "",
    "シンコ・デ・チョコボ",                "J-E-N-O-V-A",                      "",                                     "急げ!",
    "",                                 "",                                 "お休み,また明日",                         "",
    "",                                 "",                                 "",                                     "",
    "",                                 "",                                 "",                                     "",
    "蜜蜂の館",                          "",                                "",                                     "",
    "",                                 "",                                 "",                                     "",
    "",                                 "",                                 "",                                     "",
    "",                                 "鷺の砦",                          "",                                     "",
    "",                                 "",                                 "",                                     "",
    "",                                 "",                                 "",                                     "",
    "",                                 "",                                 "",                                     "",
    "プレリュード",                       "",                                 "",                                     "",
    "",                                 "",                                 "",                                     "",
    "",                                 "",                                 "",                                     "",
    "",                                 "",                                 "",                                     ""
};

std::string g_reverb_name[] = { "OFF", "ROOM", "STUDIO_A", "STUDIO_B", "STUDIO_C", "HALL", "SPACE", "ECHO", "DELAY", "PIPE" };

u8 g_op_size[] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x02, 0x02, 0x03, 0x02, 0x01, 0x01, 0x02, 0x03, 0x02, 0x03, 0x02, 0x02, 0x02, 0x02,
    0x03, 0x02, 0x02, 0x01, 0x04, 0x02, 0x01, 0x02, 0x04, 0x02, 0x01, 0x02, 0x03, 0x02, 0x01, 0x02,
    0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x01, 0x01, 0x01, 0x02, 0x02,
    0x01, 0x01, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x01, 0x02, 0x03, 0x03, 0x03,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x04, 0x03, 0x04, 0x03, 0x01, 0x00, 0x04,
    0x04, 0x04, 0x02, 0x01, 0x03, 0x01, 0x02, 0x03, 0x02, 0x01, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00
};

enum AkaoFileType
{
    FIELD,
    SND,
    LZS,
    DAT
};

struct MusicFileInfo
{
    std::string path;
    u16 id;
    AkaoFileType type;
};

struct MusicInfo
{
    u16 id;
    u16 reverb_type;
    std::string timestamp;
    std::vector<MusicFileInfo> files;
};
std::vector<MusicInfo> g_musics;



void AkaoDebugSndBrowser();
void AkaoDebugSfxBrowser();
void AkaoDebugSequencer();
void AkaoDebugInstr();



static std::string BCDByteToString( u8 bcd_byte )
{
    char high_nibble = (bcd_byte >> 4) & 0xf;
    char low_nibble = bcd_byte & 0xf;
    char high_char = '0' + high_nibble;
    char low_char = '0' + low_nibble;
    return std::string{ high_char, low_char };
}


static void AkaoDebugFillMusicInfo()
{
    std::string path = "data/FIELD";
    for( const auto& entry : std::filesystem::directory_iterator( "data/FIELD" ) )
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
                        file_info.type = FIELD;

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

    std::vector<std::string> add_files;
    add_files.push_back( "ENEMY6/FAN2.SND" );
    add_files.push_back( "ENEMY6/OVER2.SND" );
    add_files.push_back( "MINI/ASERI2.SND" );
    add_files.push_back( "MINI/SENSUI.SND" );
    add_files.push_back( "MOVIE/OVER2.SND" );

    for( int i = 0; i < add_files.size(); ++i )
    {
        std::vector<u8> snd;
        FileRead( add_files[i], snd );

        u8 check0 = READ_LE_U8( &snd[0x0] );
        u8 check1 = READ_LE_U8( &snd[0x1] );
        u8 check2 = READ_LE_U8( &snd[0x2] );
        u8 check3 = READ_LE_U8( &snd[0x3] );

        // if this is AKAO file (in ffvii tutorial files and music are together)
        if( (check0 == 'A') && (check1 == 'K') && (check2 == 'A') && (check3 == 'O') )
        {
            MusicFileInfo file_info;
            file_info.path = add_files[i];
            file_info.id = 0;
            file_info.type = SND;

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

    std::vector<std::pair<std::string, bool>> add_files2;
    add_files2.push_back( std::make_pair( "MINI/BGM.LZS", true ) );
    add_files2.push_back( std::make_pair( "MINI/BGM2.LZS", true ) );
    add_files2.push_back( std::make_pair( "MINI/CONDOR3.LZS", true ) );
//    add_files2.push_back( std::make_pair( "MINI/CHOCOBO.DAT", false ) );
    add_files2.push_back( std::make_pair( "MINI/H_XBIN.BIN", false ) );
    add_files2.push_back( std::make_pair( "MINI/XBIN2.BIN", false ) );
    add_files2.push_back( std::make_pair( "MOVIE/OPENING.BIN", false ) );
    add_files2.push_back( std::make_pair( "MOVIE/STAFF.BIN", false ) );
    add_files2.push_back( std::make_pair( "MOVIE/STAFF2.BIN", false ) );
    add_files2.push_back( std::make_pair( "WORLD/WM0.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WM0S.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WM1.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WM1S.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WM2.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WM2S.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WM3.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WM3S.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WM4.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WM4S.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WM5.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WM5S.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WM6.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WM6S.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WM7.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WM7S.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WM8.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WM8S.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WM9.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WM9S.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WMA.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WMAS.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WMB.TXZ", true ) );
    add_files2.push_back( std::make_pair( "WORLD/WMC.TXZ", true ) );

    for( size_t i = 0; i < add_files2.size(); ++i )
    {
        std::vector<u8> snd;
        AkaoFileType type;
        if( add_files2[i].second == true )
        {
            FileLZS( add_files2[i].first, snd );
            type = LZS;
        }
        else
        {
            FileRead( add_files2[i].first, snd );
            type = DAT;
        }

        u8 akao_found = 0;
        // search for AKAO magic until until header size
        for( size_t j = 0; j < (snd.size() - 0x10); ++j )
        {
            u8 check0 = READ_LE_U8( &snd[j + 0x0] );
            u8 check1 = READ_LE_U8( &snd[j + 0x1] );
            u8 check2 = READ_LE_U8( &snd[j + 0x2] );
            u8 check3 = READ_LE_U8( &snd[j + 0x3] );

            // check for AKAO magic in random place of file
            if( (check0 == 'A') && (check1 == 'K') && (check2 == 'A') && (check3 == 'O') )
            {
                MusicFileInfo file_info;
                file_info.path = add_files2[i].first;
                file_info.id = akao_found;
                file_info.type = type;
                ++akao_found;

                u16 id_to_find = READ_LE_U16( &snd[j + 0x4] );

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
                    music.id = READ_LE_U16( &snd[j + 0x4] );
                    music.reverb_type = READ_LE_U16( &snd[j + 0x8] );
                    music.timestamp =
                               "19" + BCDByteToString( READ_LE_U16( &snd[j + 0xa] ) )
                              + "-" + BCDByteToString( READ_LE_U16( &snd[j + 0xb] ) )
                              + "-" + BCDByteToString( READ_LE_U16( &snd[j + 0xc] ) )
                              + " " + BCDByteToString( READ_LE_U16( &snd[j + 0xd] ) )
                              + ":" + BCDByteToString( READ_LE_U16( &snd[j + 0xe] ) )
                              + ":" + BCDByteToString( READ_LE_U16( &snd[j + 0xf] ) );
                    music.files.push_back( file_info );
                    g_musics.push_back( music );
                }
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



u8* AkaoDebugGetSequenceEnd( u8* start )
{
    u8* akao = start;
    u8 size;

    do
    {
        u8 opcode = READ_LE_U8( akao );
        size = g_op_size[opcode];
        akao += size;
    } while( size != 0 );

    return akao + 1;
}



void AkaoDebugMusicPlay()
{
    AkaoMusicChannelsStop();
    AkaoSoundChannelsStop();

    AkaoMusicChannelsInit();

    g_akao_playing = PLAY;
    g_akao_sequencer = true;
}



void AkaoDebugMusicPause()
{
    g_akao_playing = PAUSE;
}



void AkaoDebugMusicStop()
{
    AkaoMusicChannelsStop();
    g_akao_playing = STOP;
}



void AkaoDebug()
{
    if( g_akao_debug == false ) return;

    ImGui::SetNextWindowPos( ImGui::GetWindowPos() + ImVec2( 0, 0 ), ImGuiCond_FirstUseEver );
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
                AkaoDebugSfxBrowser();
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

    AkaoDebugSequencer();
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

        AkaoDebugFillMusicInfo();
    }

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
    }
    ImGui::EndListBox();

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
        if( g_musics[selected_id].reverb_type < 0xa )
        {
            ImGui::Text( "    Reverb type: 0x%02x (%s)", g_musics[selected_id].reverb_type, g_reverb_name[g_musics[selected_id].reverb_type].c_str() );
        }
        ImGui::Text( "    Timestamp: %s", g_musics[selected_id].timestamp.c_str() );
        ImGui::Separator();
        ImGui::Text( "Used in files:" );
        for( int i = 0; i < g_musics[selected_id].files.size(); ++i )
        {
            // remove "data/" to clean output
            ImGui::Text( "    %s (%d)", g_musics[selected_id].files[i].path.c_str(), g_musics[selected_id].files[i].id );

            ImGui::PushID( i );

            ImGui::SameLine();
            if( ImGui::Button( ICON_FA_PLAY ) )
            {
                if( g_musics[selected_id].files[i].type == FIELD )
                {
                    std::vector<u8> field_dat;
                    FileLZS( g_musics[selected_id].files[i].path, field_dat );
                    u32 events_addr = READ_LE_U32( &field_dat[0x0] ) - g_field_dat_base_addr;
                    u8 entity_n = READ_LE_U8( &field_dat[events_addr + 0x2] );
                    u32 music_offset = events_addr + READ_LE_U32( &field_dat[events_addr + 0x20 + entity_n * 0x8 + g_musics[selected_id].files[i].id * 0x4] );
                    u16 music_size = READ_LE_U16( &field_dat[music_offset + 0x6] );
                    AkaoCopyMusic( &field_dat[music_offset + 0x10], music_size );
                }
                else if( g_musics[selected_id].files[i].type == SND )
                {
                    std::vector<u8> snd;
                    FileRead( g_musics[selected_id].files[i].path, snd );
                    u16 music_size = READ_LE_U16( &snd[0x6] );
                    AkaoCopyMusic( &snd[0x10], music_size );
                }
                else
                {
                    std::vector<u8> snd;

                    if( g_musics[selected_id].files[i].type == DAT )
                    {
                        FileRead( g_musics[selected_id].files[i].path, snd );
                    }
                    else
                    {
                        FileLZS( g_musics[selected_id].files[i].path, snd );
                    }

                    u8 akao_found = 0;
                    // search for AKAO magic until until header size
                    for( size_t j = 0; j < (snd.size() - 0x10); ++j )
                    {
                        u8 check0 = READ_LE_U8( &snd[j + 0x0] );
                        u8 check1 = READ_LE_U8( &snd[j + 0x1] );
                        u8 check2 = READ_LE_U8( &snd[j + 0x2] );
                        u8 check3 = READ_LE_U8( &snd[j + 0x3] );

                        // check for AKAO magic in random place of file
                        if( (check0 == 'A') && (check1 == 'K') && (check2 == 'A') && (check3 == 'O') )
                        {
                            if( akao_found == g_musics[selected_id].files[i].id )
                            {
                                u16 music_size = READ_LE_U16( &snd[j + 0x6] );
                                AkaoCopyMusic( &snd[j + 0x10], music_size );
                                break;
                            }
                            ++akao_found;
                        }
                    }
                }

                AkaoSetReverbMode( g_musics[selected_id].reverb_type );

                g_akao_playing_music = g_musics[selected_id].id;
                g_akao_playing_file = i;
                AkaoDebugMusicPlay();
            }

            if( (g_akao_playing != STOP) && (g_akao_playing_music == g_musics[selected_id].id) && (g_akao_playing_file == i) )
            {
                ImGui::SameLine();
                if( ImGui::Button( ICON_FA_PAUSE ) )
                {
                    AkaoDebugMusicPause();
                }

                ImGui::SameLine();
                if( ImGui::Button( ICON_FA_STOP ) )
                {
                    AkaoDebugMusicStop();
                }
            }

            ImGui::PopID();
        }
    }

    ImGui::EndChild();

    ImGui::Columns( 1 );
}



void AkaoDebugSfxBrowser()
{
    ImGui::TextDisabled( "Sfx List" );
    ImGui::Separator();

    for( int i = 0; i < 0x400; ++i )
    {
        ImGui::PushID( i );

        char label[256];
        snprintf( label, sizeof( label ), "0x%03x", i );

        bool enabled = ( READ_LE_U16( &g_akao_effects_all[i * 0x4] ) != 0xffff ) ? true : false;

        if( (i % 0x10) != 0 ) ImGui::SameLine();

        ImGui::BeginDisabled( !enabled );
        if( ImGui::Button( label ) )
        {
            u32 seq_1, seq_2;
            //system_akao_sound_channels_clear( 0x4, 0x1 );
            AkaoSoundGetSequence( seq_1, seq_2, i );
            AkaoSoundChannelsInit( 0x40, 0x0, seq_1, seq_2 );
            g_akao_sequencer = true;
        }
        ImGui::EndDisabled();

        ImGui::PopID();
    }
}



void AkaoDebugSequencerSeq( u8* start, u8* cur, int base_x, float width, int& add_y, bool active )
{
    u8* akao = start;
    u8* seq_end = AkaoDebugGetSequenceEnd( akao );
    bool read = true;
    ImVec4 color;
    int x = base_x;

    while( read && (akao < seq_end) )
    {
        if( active )
        {
            if( akao < cur )
            {
                color = ImVec4( 0.8f, 0.8f, 0.2f, 1.0f );
            }
            else
            {
                color = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
            }
        }
        else
        {
            color = ImVec4( 0.3f, 0.3f, 0.3f, 1.0f );
        }

        if( x >= width )
        {
            x = base_x;
            add_y += 20;
        }

        u8 opcode = READ_LE_U8( akao );

        if( opcode < 0xa0 )
        {
            ImGui::SetCursorPos( ImVec2( x, add_y ) );
            if( opcode < 0x84 ) ImGui::TextColored( color, "|" ); // note
            else if( opcode >= 0x8f ) ImGui::TextColored( color, "_" ); // stop effect and pause
            else ImGui::TextColored( color, "-" ); // continue play same note
            x += 8;
        }
        else
        {
            if( g_op_size[opcode] == 0 )
            {
                read = false;
            }
            else
            {
                akao += g_op_size[opcode] - 1;
            }

            ImGui::SetCursorPos( ImVec2( x, add_y ) );
            ImGui::TextColored( color, "%02x", opcode );
            x += 16;
        }

        akao += 0x1;
    }
}



void AkaoDebugSequencer()
{
    if( g_akao_sequencer == false ) return;

    ImGui::SetNextWindowPos( ImGui::GetWindowPos() + ImVec2( 260, 150 ), ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowSize( ImVec2( 600, 500 ), ImGuiCond_Once );
    ImGui::Begin( "Akao Sequencer", &g_akao_sequencer, ImGuiWindowFlags_NoCollapse );
    {
        float width = ImGui::GetWindowContentRegionMax().x;
        int add_y = 20;
        float base_x = 5;
        float line_height = ImGui::GetTextLineHeight();
        ImVec4 color = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );

        if( ImGui::Button( ICON_FA_PLAY ) )
        {
            //AkaoDebugMusicPlay();
        }
        ImGui::SameLine();
        if( ImGui::Button( ICON_FA_PAUSE ) )
        {
            AkaoDebugMusicPause();
        }
        ImGui::SameLine();
        if( ImGui::Button( ICON_FA_STOP ) )
        {
            AkaoDebugMusicStop();
            AkaoSoundChannelsStop();
        }
        add_y += line_height + 20;

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
            if( g_channels_1[i].seq_start != nullptr )
            {
                std::string info = "Music channel: %02x";
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
                if( g_channels_1[i].sfx_mask & AKAO_SFX_LEGATO ) info += std::string( " LEGATO" );
                if( g_channels_1[i].sfx_mask & AKAO_SFX_FULL_LENGTH ) info += std::string( " FULL_LENGTH" );

                ImGui::SetCursorPos( ImVec2( base_x, add_y ) );
                ImGui::TextColored( ImVec4( 1.0f, 1.0f, 1.0f, 1.0f ), info.c_str(), i );
                add_y += line_height;

                u8* akao = g_channels_1[i].seq_start;

                AkaoDebugSequencerSeq( akao, g_channels_1[i].seq, base_x, width, add_y, g_channels_1_config.active_mask & (0x1 << i) );

                if( i != 17 )
                {
                    add_y += line_height + 5;
                    ImGui::SetCursorPos( ImVec2( base_x, add_y ) );
                    add_y += 10;
                    ImGui::Separator();
                }

                add_y += line_height;
            }
        }

        for( size_t i = 0; i < 0x8; ++i )
        {
            if( g_channels_3[i].seq_start != nullptr )
            {
                std::string info = "Sound channel: %02x";
                if( g_channels_3[i].update_flags & AKAO_UPDATE_ALTERNATIVE )
                {
                    info += std::string( " ALT" );
                    info += ( g_channels_3[i].update_flags & AKAO_UPDATE_ALTERNATIVE_CUR ) ? std::string( "2" ) : std::string( "1" );
                }
                if( g_channels_3[i].update_flags & AKAO_UPDATE_DRUM_MODE ) info += std::string( " DRUM" );
                if( g_channels_3[i].update_flags & AKAO_UPDATE_VIBRATO ) info += std::string( " VIBRATO:" ) + std::to_string( g_channels_3[i].vibrato_type );
                if( g_channels_3[i].update_flags & AKAO_UPDATE_TREMOLO ) info += std::string( " TREMOLO:" ) + std::to_string( g_channels_3[i].tremolo_type );
                if( g_channels_3[i].update_flags & AKAO_UPDATE_OVERLAY ) info += std::string( " OVER:" ) + std::to_string( g_channels_3[i].over_voice_id );
                if( g_channels_3[i].pitch_slide_steps_cur != 0 ) info += std::string( " PITCH SLIDE:" ) + std::to_string( g_channels_3[i].pitch_slide_steps_cur );
                if( g_channels_3[i].sfx_mask & AKAO_SFX_LEGATO ) info += std::string( " LEGATO" );
                if( g_channels_3[i].sfx_mask & AKAO_SFX_FULL_LENGTH ) info += std::string( " FULL_LENGTH" );

                ImGui::SetCursorPos( ImVec2( base_x, add_y ) );
                ImGui::TextColored( ImVec4( 1.0f, 1.0f, 1.0f, 1.0f ), info.c_str(), i );
                add_y += line_height;

                u8* akao = g_channels_3[i].seq_start;

                AkaoDebugSequencerSeq( akao, g_channels_3[i].seq, base_x, width, add_y, g_channels_3_config.active_mask & (0x10000 << i) );

                if( i != 7 )
                {
                    add_y += line_height + 5;
                    ImGui::SetCursorPos( ImVec2( base_x, add_y ) );
                    add_y += 10;
                    ImGui::Separator();
                }

                add_y += line_height;
            }
        }
    }

    ImGui::End();

    if( g_akao_sequencer != g_akao_sequencer_prev )
    {
        if( !g_akao_sequencer ) AkaoDebugMusicStop();

        g_akao_sequencer_prev = g_akao_sequencer;
    }
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
