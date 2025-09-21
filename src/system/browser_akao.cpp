#include "kernel/akao.h"
#include "system/browser_akao.h"
#include "system/icons_font.h"
#include "psyq/filesystem.h"

#include "ofxImGui.h"
#include <format>

bool g_browser_akao = false;
bool g_browser_akao_sequencer = false;
bool g_browser_akao_sequencer_prev = false;
enum PlayingState
{
    PLAY,
    PAUSE,
    STOP,
};
PlayingState g_browser_akao_playing = STOP;
int g_browser_akao_playing_music = -1;
int g_browser_akao_playing_file = 0;
int g_browser_akao_playing_sound = -1;
int g_browser_akao_playing_instrument = -1;



// music id from 0x1 - 0x63
std::string g_music_names_en[] =
{
    "",                                 "(empty)",                                 "Opening - Bombing Mission",            "Bombing Mission",
    "In Search of the Man in Black",    "Other Side of the Mountain",              "",                                     "Mako Reactor",
    "Let the Battles Begin!",           "Fiddle de Chocobo",                       "Anxiety",                              "Fight On!",
    "Cait Sith's Theme",                "Aerith's Theme",                          "Main Theme",                           "",
    "",                                 "Rufus's Welcoming Ceremony",              "",                                     "Who...Are You?",
    "On Our Way",                       "It's Hard to Stand on Both Feet!",        "Open Your Heart",                      "Waltz de Chocobo",
    "Don of the Slums",                 "Trail of Blood",                          "Red XIII's Theme",                     "Cosmo Canyon",
    "The Great Warrior",                "Lurking In the Darkness",                 "Shinra, Inc",                          "Infiltrating Shinra",
    "Under the Rotting Pizza",          "Farm Boy",                                "On That Day, Five Years Ago",          "Tifa's Theme",
    "Costa del Sol",                    "The Oppressed",                           "Flowers Blooming in the Church",       "The Chase",
    "Desert Wasteland",                 "Those Chosen by the Planet",              "Barret's Theme",                       "Mining Town",
    "Lifestream",                       "Electric de Chocobo",                     "Turks' Theme ",                        "Fanfare",
    "The Highwind Takes to the Skies",  "Mako Cannon 1",                           "Words Drowned by Fireworks",           "",
    "Cinco de Chocobo",                 "JENOVA",                                  "Descendant of Shinobi",                "Hurry!",
    "Gold Saucer",                      "Provincial Town",                         "Good Night, Until Tomorrow",           "Continue",
    "Jackpot!",                         "Tango of Tears",                          "Win / Place / Show Chocobo!",          "Debut",
    "From the Edge of Despair",         "Dear to the Heart",                       "",                                     "The Nightmare Begins",
    "Honeybee Inn",                     "Forested Temple",                         "Mark of a Traitor",                    "Hurry Up!",
    "The North Cave",                   "Cid's Theme",                             "Launching a Dream Into Space",         "Listen to the Cries of the Planet",
    "Who...Am I?",                      "Wutai",                                   "Buried in Snow",                       "Stolen Materia",
    "Reunion",                          "Fort Condor",                             "One-Winged Angel",                     "Shinra's Full-Scale Assault",
    "Attack of the Weapon",             "JENOVA COMPLETE",                         "Secret of the Deep Sea",               "Judgment Day",
    "Birth of a God",                   "",                                        "Countdown",                            "Steal the Tiny Bronco!",
    "",                                 "Mako Cannon 2",                           "The Destruction of Shinra",            "",
    "The Prelude",                      "The Planet's Crisis",                     "",                                     "Ending Credits"
};
std::string g_music_names_jp[] =
{
    "",                                 "",                                        "オープニング~爆破ミッション",                 "爆破ミッション",
    "黒マントの男を追え",                  "山の向こうに",                             "",                                     "魔晄炉",
    "闘う者達 lit. \"Those Who Fight\"", "フィドル・デ・チョコボ",                    "不安な心 lit. \"Anxious Heart\"",       "更に闘う者達 lit. \"Those Who Fight Further\"",
    "ケット・シーのテーマ",                "エアリスのテーマ",                         "メインテーマ",                             "",
    "",                                 "ルーファウス歓迎式典",                      "",                                     "お前は…誰だ",
    "旅の途中で",                         "二本足で立つのも難しいものだな",            "心開けば",                                "ワルツ・デ・チョコボ",
    "スラムのドン",                         "血の跡",                                 "レッドXIIIのテーマ",                       "星降る峡谷 lit. \"Valley of the Falling Stars\"",
    "偉大なる戦士",                       "闇に潜む",                                "神羅カンパニー lit. \"Shinra Company\"", "神羅ビル潜入",
    "腐ったピザの下で",                     "牧場の少年",                            "5年前のあの日",                         "ティファのテーマ",
    "太陽の海岸  lit. \"Sun Coast\"",     "虐げられた民衆",                         "教会に咲く花",                           "クレイジーモーターサイクル lit. \"Crazy Motorcycle\"",
    "砂の流刑地",                         "星に選ばれし者",                           "バレットのテーマ",                         "炭坑の街",
    "生命の流れ lit. \"Stream of Life\"", "エレキ・デ・チョコボ",                      "タークスのテーマ",                         "ファンファーレ",
    "空駆けるハイウィンド",                "魔晄キャノン発射 1",                        "花火に消された言葉",                      "",
    "シンコ・デ・チョコボ",                "J-E-N-O-V-A",                             "忍びの末裔",                             "急げ!",
    "ゴールドソーサー",                    "偏狭の村",                                "お休み,また明日",                         "つづきから",
    "大当たりぃ",                         "涙のタンゴ",                              "本命穴チョコボ lit. \"Place Chocobo\"",   "初舞台",
    "絶望の淵から",                       "想いを胸に",                               "",                                     "悪夢の始まり",
    "蜜蜂の館",                          "樹海の神殿",                               "裏切り者の烙印",                          "もっと急げ!",
    "北の大空洞",                         "シドのテーマ",                             "宇宙への夢",                             "星の声が聞こえる",
    "俺は…誰だ",                         "ウータイ",                                 "雪に閉ざされて",                        "マテリアいただき",
    "リユニオン",                        "鷺の砦",                                   "片翼の天使",                            "神羅軍総攻撃",
    "ウェポン襲来",                       "完全なるジェノヴァ",                        "深海に眠る秘密",                         "最期の日",
    "神の誕生",                          "",                                        "秒読み開始",                            "タイニーブロンコを奪え!",
    "",                                 "魔晄キャノン発射 2",                         "神羅爆発",                              "",
    "プレリュード",                       "星の危機",                                  "",                                     "スタッフロール"
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
    DAT,
    LZS,
    LZSDAT
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



void BrowserAkaoSnd();
void BrowserAkaoSfx();
void BrowserAkaoSequencer();
void BrowserAkaoInstr();



static std::string BCDByteToString( u8 bcd_byte )
{
    char high_nibble = (bcd_byte >> 4) & 0xf;
    char low_nibble = bcd_byte & 0xf;
    char high_char = '0' + high_nibble;
    char low_char = '0' + low_nibble;
    return std::string{ high_char, low_char };
}


static void BrowserAkaoFillMusicInfo()
{
    std::vector<std::pair<std::string, bool>> files; // collection of all files that contain snd

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
                files.push_back( std::make_pair( filepath, true ) );
            }
        }
    }

    files.push_back( std::make_pair( "ENEMY6/FAN2.SND", false ) );
    files.push_back( std::make_pair( "ENEMY6/OVER2.SND", false ) );
    files.push_back( std::make_pair( "MINI/ASERI2.SND", false ) );
    files.push_back( std::make_pair( "MINI/SENSUI.SND", false ) );
    files.push_back( std::make_pair( "MOVIE/OVER2.SND", false ) );
    files.push_back( std::make_pair( "MINI/BGM.LZS", true ) );
    files.push_back( std::make_pair( "MINI/BGM2.LZS", true ) );
    files.push_back( std::make_pair( "MINI/CONDOR3.LZS", true ) );
    files.push_back( std::make_pair( "MINI/H_XBIN.BIN", false ) );
    files.push_back( std::make_pair( "MINI/XBIN2.BIN", false ) );
    files.push_back( std::make_pair( "MOVIE/OPENING.BIN", false ) );
    files.push_back( std::make_pair( "MOVIE/STAFF.BIN", false ) );
    files.push_back( std::make_pair( "MOVIE/STAFF2.BIN", false ) );
    files.push_back( std::make_pair( "WORLD/WM0.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WM0S.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WM1.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WM1S.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WM2.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WM2S.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WM3.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WM3S.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WM4.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WM4S.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WM5.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WM5S.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WM6.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WM6S.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WM7.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WM7S.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WM8.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WM8S.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WM9.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WM9S.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WMA.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WMAS.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WMB.TXZ", true ) );
    files.push_back( std::make_pair( "WORLD/WMC.TXZ", true ) );

    for( size_t i = 0; i < files.size(); ++i )
    {
        std::vector<u8> snd;
        AkaoFileType type;
        if( files[i].second == true )
        {
            FileLZS( files[i].first, snd );
            type = LZS;
        }
        else
        {
            FileRead( files[i].first, snd );
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
                file_info.path = files[i].first;
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

    {
        std::vector<u8> temp;
        FileRead( "MINI/CHOCOBO.DAT", temp );
        for( size_t i = 0; i < 0x4; ++i )
        {
            std::vector<u8> snd;

            u32 ofst = 0x11a000 + i * 0x800;
            std::vector<u8> part(temp.begin() + ofst, temp.begin() + ofst + 0x800);
            LZSExtract( part, snd );

            u8 check0 = READ_LE_U8( &snd[0x0] );
            u8 check1 = READ_LE_U8( &snd[0x1] );
            u8 check2 = READ_LE_U8( &snd[0x2] );
            u8 check3 = READ_LE_U8( &snd[0x3] );

            // check for AKAO magic in random place of file
            if( (check0 == 'A') && (check1 == 'K') && (check2 == 'A') && (check3 == 'O') )
            {
                MusicFileInfo file_info;
                file_info.path = "MINI/CHOCOBO.DAT";
                file_info.id = i;
                file_info.type = LZSDAT;

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



u8* BrowserAkaoGetSequenceEnd( u8* start )
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



void BrowserAkaoStop()
{
    AkaoMusicChannelsStop();
    AkaoSoundChannelsStop();
    g_browser_akao_playing = STOP;
}



// play music file by id and inner file id
void BrowserAkaoMusicPlay( u32 id, u32 file_id )
{
    // stop sound music and reset
    BrowserAkaoStop();
    AkaoInitData();
    g_browser_akao_playing_music = -1;
    g_browser_akao_playing_sound = -1;
    g_browser_akao_playing_instrument = -1;

    // load instruments for music
    if( g_musics[id].id == 0x52 ) // Sephiroth
    {
        std::vector<u8> instr2_all;
        FileRead( "SOUND/INSTR2.ALL", instr2_all );
        std::vector<u8> instr2_dat;
        FileRead( "SOUND/INSTR2.DAT", instr2_dat );
        AkaoLoadInstr2( &instr2_all[0], &instr2_dat[0] );
    }
    else
    {
        std::vector<u8> instr_all;
        FileRead( "SOUND/INSTR.ALL", instr_all );
        std::vector<u8> instr_dat;
        FileRead( "SOUND/INSTR.DAT", instr_dat );
        AkaoLoadInstr( &instr_all[0], &instr_dat[0] );
    }

    // load new music
    if( g_musics[id].files[file_id].type == LZSDAT )
    {
        std::vector<u8> temp;
        FileRead( g_musics[id].files[file_id].path, temp );
        std::vector<u8> snd;
        u32 ofst = 0x11a000 + g_musics[id].files[file_id].id * 0x800;
        std::vector<u8> part(temp.begin() + ofst, temp.begin() + ofst + 0x800);
        LZSExtract( part, snd );
        u16 music_size = READ_LE_U16( &snd[0x6] );
        AkaoCopyMusic( &snd[0x10], music_size );
    }
    else
    {
        std::vector<u8> snd;

        if( g_musics[id].files[file_id].type == DAT )
        {
            FileRead( g_musics[id].files[file_id].path, snd );
        }
        else
        {
            FileLZS( g_musics[id].files[file_id].path, snd );
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
                if( akao_found == g_musics[id].files[file_id].id )
                {
                    u16 music_size = READ_LE_U16( &snd[j + 0x6] );
                    AkaoCopyMusic( &snd[j + 0x10], music_size );
                    break;
                }
                ++akao_found;
            }
        }
    }

    AkaoSetReverbMode( g_musics[id].reverb_type );

    AkaoMusicChannelsInit();

    // enable sequencer
    g_browser_akao_playing = PLAY;
    g_browser_akao_sequencer = true;
    g_browser_akao_playing_music = id;
    g_browser_akao_playing_file = file_id;
}



void BrowserAkaoSoundPlay( u32 id )
{
    // stop sound music and reset
    BrowserAkaoStop();
    AkaoInitData();
    g_browser_akao_playing_music = -1;
    g_browser_akao_playing_sound = -1;
    g_browser_akao_playing_instrument = -1;

    u32 seq_1, seq_2;
    //system_akao_sound_channels_clear( 0x4, 0x1 );
    AkaoSoundGetSequence( seq_1, seq_2, id );
    AkaoSoundChannelsInit( 0x40, 0x0, seq_1, seq_2 );

    // enable sequencer
    g_browser_akao_playing = PLAY;
    g_browser_akao_sequencer = true;
    g_browser_akao_playing_sound = id;
}



void BrowserAkaoPause()
{
    g_browser_akao_playing = PAUSE;
}



void BrowserAkao()
{
    if( g_browser_akao == false ) return;

    ImGui::SetNextWindowPos( ImGui::GetWindowPos() + ImVec2( 0, 0 ), ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowSize( ImVec2( 900, 650 ), ImGuiCond_Once );
    if( ImGui::Begin( "Akao Browser", &g_browser_akao, ImGuiWindowFlags_NoCollapse ) )
    {
        if( ImGui::BeginTabBar( "Akao" ) )
        {
            if( ImGui::BeginTabItem( "SND" ) )
            {
                BrowserAkaoSnd();
                ImGui::EndTabItem();
            }

            if( ImGui::BeginTabItem( "SFX" ) )
            {
                BrowserAkaoSfx();
                ImGui::EndTabItem();
            }

            if( ImGui::BeginTabItem( "Instruments" ) )
            {
                BrowserAkaoInstr();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }
    ImGui::End();

    if( g_browser_akao == false )
    {
        // stop sound music and reset
        BrowserAkaoStop();
        AkaoInitData();
        g_browser_akao_sequencer = false;
        g_browser_akao_playing_music = -1;
        g_browser_akao_playing_sound = -1;
        g_browser_akao_playing_instrument = -1;
    }

    BrowserAkaoSequencer();
}



void BrowserAkaoSnd()
{
    ImGui::Columns( 2, nullptr, true );
    // do not reset column width every frame
    static bool first_frame = true;
    if( first_frame )
    {
        ImGui::SetColumnWidth( 0, 250 );
        first_frame = false;

        BrowserAkaoFillMusicInfo();
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
                BrowserAkaoMusicPlay( selected_id, i );
            }

            if( (g_browser_akao_playing_music == selected_id) && (g_browser_akao_playing_file == i) )
            {
                ImGui::SameLine();
                if( ImGui::Button( ICON_FA_PAUSE ) )
                {
                    BrowserAkaoPause();
                }

                ImGui::SameLine();
                if( ImGui::Button( ICON_FA_STOP ) )
                {
                    BrowserAkaoStop();
                }
            }

            ImGui::PopID();
        }
    }

    ImGui::EndChild();

    ImGui::Columns( 1 );
}



void BrowserAkaoSfx()
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

        bool push = false;
        if( g_browser_akao_playing_sound == i )
        {
            ImGui::BeginDisabled( true );
            ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 1.0f, 1.0f, 0.0f, 1.0f ) );
            push = true;
        }
        else
        {
            ImGui::BeginDisabled( !enabled );
        }

        if( ImGui::Button( label ) )
        {
            BrowserAkaoSoundPlay( i );
        }

        if( push ) ImGui::PopStyleColor( 1 );

        ImGui::EndDisabled();

        ImGui::PopID();
    }
}



void BrowserAkaoSequencerSeq( u8* start, u8* cur, int base_x, float width, int& add_y, bool active )
{
    u8* akao = start;
    u8* seq_end = BrowserAkaoGetSequenceEnd( akao );
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



void BrowserAkaoSequencer()
{
    if( g_browser_akao_sequencer == false ) return;

    ImGui::SetNextWindowPos( ImGui::GetWindowPos() + ImVec2( 260, 150 ), ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowSize( ImVec2( 600, 500 ), ImGuiCond_Once );
    ImGui::Begin( "Akao Sequencer", &g_browser_akao_sequencer, ImGuiWindowFlags_NoCollapse );
    {
        float width = ImGui::GetWindowContentRegionMax().x;
        int add_y = 20;
        float base_x = 5;
        float line_height = ImGui::GetTextLineHeight();
        ImVec4 color = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );

        if( ImGui::Button( ICON_FA_PLAY ) )
        {
            if( g_browser_akao_playing_music != -1 )
            {
                BrowserAkaoMusicPlay( g_browser_akao_playing_music, g_browser_akao_playing_file );
            }
            else if( g_browser_akao_playing_sound != -1 )
            {
                BrowserAkaoSoundPlay( g_browser_akao_playing_sound );
            }
        }
        ImGui::SameLine();
        if( ImGui::Button( ICON_FA_PAUSE ) )
        {
            BrowserAkaoPause();
        }
        ImGui::SameLine();
        if( ImGui::Button( ICON_FA_STOP ) )
        {
            BrowserAkaoStop();
        }

        // info what is playing now
        if( g_browser_akao_playing_music != -1 )
        {
            ImGui::SameLine();
            ImGui::Text( "Music:0x%02x", g_musics[g_browser_akao_playing_music].id );
            ImGui::SameLine();
            ImGui::Text( " %s (%d)", g_musics[g_browser_akao_playing_music].files[g_browser_akao_playing_file].path.c_str(), g_musics[g_browser_akao_playing_music].files[g_browser_akao_playing_file].id );
            add_y += line_height + 20;
            if( g_music_names_jp[g_musics[g_browser_akao_playing_music].id] != "" )
            {
                ImGui::Text( "Name: %s (jp. %s)", g_music_names_en[g_musics[g_browser_akao_playing_music].id].c_str(), g_music_names_jp[g_musics[g_browser_akao_playing_music].id].c_str() );
            }
            else
            {
                ImGui::Text( "Name: %s", g_music_names_en[g_musics[g_browser_akao_playing_music].id].c_str() );
            }
            add_y += line_height;
        }
        else if( g_browser_akao_playing_sound != -1 )
        {
            ImGui::SameLine();
            ImGui::Text( "Sound:0x%03x", g_browser_akao_playing_sound );
            add_y += line_height + 20;
        }

        ImGui::SetCursorPos( ImVec2( base_x, add_y ) );
        ImGui::TextColored( ImVec4( 1.0f, 1.0f, 1.0f, 1.0f ), "PLAY MASK: 0x%02x", g_channels_1_config.keyed_mask );
        add_y += line_height;

        if( g_channels_1_config.timer_lower != 0 )
        {
            ImGui::SetCursorPos( ImVec2( base_x, add_y ) );
            ImGui::TextColored( ImVec4( 1.0f, 1.0f, 1.0f, 1.0f ), "Timer: %02d:%02d:%02d", g_channels_1_config.timer_top_cur, g_channels_1_config.timer_upper_cur, g_channels_1_config.timer_lower_cur );
            add_y += line_height;
        }

        add_y += 5;
        ImGui::SetCursorPos( ImVec2( base_x, add_y ) );
        ImGui::Separator();
        add_y += 5;

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

                BrowserAkaoSequencerSeq( akao, g_channels_1[i].seq, base_x, width, add_y, g_channels_1_config.active_mask & (0x1 << i) );
                add_y += line_height;

                if( i != 17 )
                {
                    add_y += 5;
                    ImGui::SetCursorPos( ImVec2( base_x, add_y ) );
                    ImGui::Separator();
                    add_y += 5;
                }
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
                ImGui::TextColored( ImVec4( 1.0f, 1.0f, 1.0f, 1.0f ), info.c_str(), i + 0x10 );
                add_y += line_height;

                u8* akao = g_channels_3[i].seq_start;

                BrowserAkaoSequencerSeq( akao, g_channels_3[i].seq, base_x, width, add_y, g_channels_3_config.active_mask & (0x10000 << i) );
                add_y += line_height;

                if( i != 7 )
                {
                    add_y += 5;
                    ImGui::SetCursorPos( ImVec2( base_x, add_y ) );
                    ImGui::Separator();
                    add_y += 5;
                }
            }
        }
    }

    ImGui::End();

    if( g_browser_akao_sequencer != g_browser_akao_sequencer_prev )
    {
        if( !g_browser_akao_sequencer )
        {
            // stop sound music and reset
            BrowserAkaoStop();
            g_browser_akao_playing_sound = -1;
            g_browser_akao_playing_music = -1;
        }

        g_browser_akao_sequencer_prev = g_browser_akao_sequencer;
    }
}



void BrowserAkaoInstr()
{
    ImGui::Columns( 2, nullptr, true ); // 2 колонки, ресайз между ними
    // do not reset column width every frame
    static bool first_frame = true;
    if( first_frame )
    {
        ImGui::SetColumnWidth( 0, 250 );
        first_frame = false;
    }

    static int instr_id = 0;

    if( ImGui::BeginListBox( "##InstrList", ImVec2( -FLT_MIN, -FLT_MIN ) ) )
    {
        ImGui::TextDisabled( "Instruments List" );
        ImGui::Separator();

        for( int i = 0; i < 0x80; ++i )
        {
            ImGui::PushID( i );

            if( ImGui::Selectable( ("0x" + std::format( "{:0x}", i )).c_str(), (i == instr_id) ) )
            {
                instr_id = i;
            }

            ImGui::PopID();
        }
    }
    ImGui::EndListBox();

    ImGui::NextColumn();
    ImGui::BeginChild( "right_panel", ImVec2( 0, 0 ), true );

    ImGui::Text( "Instrument: 0x%02x", instr_id );

    if( ImGui::Button( ICON_FA_PLAY ) )
    {
        // stop sound music and reset
        BrowserAkaoStop();
        g_browser_akao_playing_sound = -1;
        g_browser_akao_playing_music = -1;
        g_browser_akao_sequencer = false;

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
        g_browser_akao_playing_instrument = instr_id;
    }
    if( g_browser_akao_playing_instrument == instr_id )
    {
        ImGui::SameLine();
        if( ImGui::Button( ICON_FA_STOP ) )
        {
            s32 voice_id = 0;
            PsyqSpuSetKey( SPU_OFF, 0x1 << voice_id );
        }
    }

    if( ImGui::BeginTable( "InstrumentTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg ) )
    {
        ImGui::TableSetupColumn( "Parameter" );
        ImGui::TableSetupColumn( "Value" );

        auto &inst = g_akao_instrument[instr_id];

        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text( "Start address" );
        ImGui::TableNextColumn(); ImGui::Text( "0x%02x", inst.addr );
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text( "Loop address" );
        ImGui::TableNextColumn(); ImGui::Text( "0x%02x", inst.loop_addr );
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text( "Attack mode" );
        ImGui::TableNextColumn(); ImGui::Text( "0x%02x", inst.a_mode );
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text( "Sustain mode" );
        ImGui::TableNextColumn(); ImGui::Text( "0x%02x", inst.s_mode );
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text( "Release mode" );
        ImGui::TableNextColumn(); ImGui::Text( "0x%02x", inst.r_mode );
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text( "Attack rate" );
        ImGui::TableNextColumn(); ImGui::Text( "0x%02x", inst.ar );
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text( "Decay rate" );
        ImGui::TableNextColumn(); ImGui::Text( "0x%02x", inst.dr );
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text( "Sustain level" );
        ImGui::TableNextColumn(); ImGui::Text( "0x%02x", inst.sl );
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text( "Sustain rate" );
        ImGui::TableNextColumn(); ImGui::Text( "0x%02x", inst.sr );
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text( "Release rate" );
        ImGui::TableNextColumn(); ImGui::Text( "0x%02x", inst.rr );
        for( int i = 0; i < 0xc; ++i )
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn(); ImGui::Text( "Pitch 0x%x", i );
            ImGui::TableNextColumn(); ImGui::Text( "0x%02x", inst.pitch[i] );
        }
        ImGui::EndTable();
    }

    ImGui::EndChild();

    ImGui::Columns( 1 );
}
