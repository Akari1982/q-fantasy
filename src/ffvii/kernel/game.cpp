#include "akao.h"
#include "fade.h"
#include "file.h"
#include "game.h"
#include "ending/ending.h"
#include "menu/menu.h"
#include "menu/newgame.h"
#include "field/field.h"
#include "psyq/libgte.h"
#include "psyq/libgpu.h"
#include "psyq/libetc.h"
#include "psyq/libspu.h"
#include "system/logger.h"



#define BG_RENDER_NONE 0x0
#define BG_RENDER_FADE 0x1
#define BG_RENDER_BATTLE 0x2
#define BG_RENDER_BATTLE_SWIRL 0x3
#define BG_RENDER_BATTLE_RESULT 0x4

u16 g_bg_render = BG_RENDER_NONE;

u16 g_game_state = GAME_STATE_NONE;
s16 g_game_state_prev = GAME_STATE_NONE;



void GameInitBase();
void GameInitKernel();
void GameInitAkaoEngine();
void GameInitNewGame();
void GameInitFieldFromSaveMap();
void GameBackgroundRender();



void GameMain()
{
    GameInitBase();

    EndingMainLogo();

    GameInitKernel();

    while (true)
    {
        GameInitAkaoEngine();

        SRECT rect;
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x1e0;
        rect.h = 0x1d8;
        PsyqClearImage(&rect, 0, 0, 0);

        if (MenuNewGameMain() == 0x1)
        {
            GameInitNewGame();
        }

        FieldInitEnv();

        GameInitFieldFromSaveMap();

        g_game_state_prev = GAME_STATE_NONE;

        bool restart = false;
        while (restart == false)
        {
            switch (g_game_state)
            {
                case GAME_STATE_FIELD:
                {
                    FieldMain();
                }
            }

            //AkaoQuit();
        }
    }
}



void GameInitBase()
{
    PsyqStopCallback();
    PsyqResetCallback();
    PsyqSpuInit();

    PsyqVsyncCallback(GameBackgroundRender);

    PsyqSetDispMask(0);
    PsyqInitGeom();
}



void GameInitAkaoEngine()
{
    std::vector<u8> instr_all;
    FileRead("SOUND/INSTR.ALL", instr_all);

    std::vector<u8> instr_dat;
    FileRead("SOUND/INSTR.DAT", instr_dat);

    std::vector<u8> effect_all;
    FileRead("SOUND/EFFECT.ALL", effect_all);

    AkaoInit(&instr_all[0], &instr_dat[0]);
    AkaoLoadEffect(&effect_all[0]);
}



void GameInitKernel()
{
    std::vector<u8> window_bin;
    FileRead("INIT/WINDOW.BIN", window_bin);

    GZIPSetDataBlock(window_bin);

    while (true)
    {
        u16 type = GZIPGetType();

        if (type == 0xffff) break;

        if (type == 0) // image
        {
            std::vector<u8> temp;
            GZIPPackDecompressNextBlock(temp);

            SRECT rect;
            u32 offset = 0;
            if (READ_LE_U32(&temp[0x4]) & 0x8)
            {
                offset = (READ_LE_U32(&temp[0x8]) >> 0x2) << 0x2;

                rect.x = READ_LE_S16(&temp[0xc]);
                rect.y = READ_LE_S16(&temp[0xe]);
                rect.w = READ_LE_S16(&temp[0x10]);
                rect.h = READ_LE_S16(&temp[0x12]);
                PsyqLoadImage(&rect, &temp[0x14]);
            }

            rect.x = READ_LE_S16(&temp[offset + 0xc]);
            rect.y = READ_LE_S16(&temp[offset + 0xe]);
            rect.w = READ_LE_S16(&temp[offset + 0x10]);
            rect.h = READ_LE_S16(&temp[offset + 0x12]);
            PsyqLoadImage(&rect, &temp[offset + 0x14]);
        }
        else if (type == 0x1)
        {
            GZIPPackDecompressNextBlock(g_font_paddings);
        }
    }
}



void GameInitNewGame()
{
}


void GameInitFieldFromSaveMap()
{
    g_game_state = GAME_STATE_FIELD;
    g_game_state_prev = GAME_STATE_FIELD;

    g_field_map_id = 0x74;
}



void GameBackgroundFadeRender()
{
    if (g_bg_fade_type != 0)
    {
        g_field_rb += 0x1;
        g_field_rb &= 0x1;

        LOG_WARNING("Fade Check");

//        system_fade_bg_update();

//        PsyqPutDispenv(&g_field_disp_env[g_field_rb]);
//        PsyqPutDrawenv(&g_field_draw_env[g_field_rb]);
//        PsyqDrawOTag(&g_fade_ot[g_field_rb]);
    }
}



void GameBackgroundRender()
{
    switch (g_bg_render)
    {
        case BG_RENDER_FADE: GameBackgroundFadeRender; break;
    }
}
