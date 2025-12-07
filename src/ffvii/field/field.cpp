#include "field.h"
#include "field_list.h"
#include "model.h"
#include "rain.h"
#include "fade.h"
#include "event.h"
#include "entity.h"
#include "kernel/akao.h"
#include "kernel/game.h"
#include "kernel/file.h"
#include "kernel/buttons.h"
#include "psyq/libgte.h"
#include "system/logger.h"

#include "battle/swirl.h"

#include <vector>



FieldControl g_field_control;

u32 g_field_dat_base_addr = 0x80114fe4;
std::vector<u8> g_field_dat;
std::vector<u8> l_field_mim;
u16 g_field_map_id = 0;

u16 g_field_rb = 0;
FieldRenderData l_field_render_data[0x2];
FieldCamera l_field_camera;
DISPENV g_main_dispenv[0x2];
DRAWENV l_main_drawenv[0x2];
DRAWENV l_drawenv[0xa];

u16 l_field_ofs_x = 0;
u16 l_field_ofs_y = 0;
s16 l_debug_ofs_x = 0;
s16 l_debug_ofs_y = 0;

u32 l_buttons_state = 0;

struct sWalkMesh
{
    SVECTOR p1;
    SVECTOR p2;
    SVECTOR p3;
};
std::vector< sWalkMesh > field_walkmesh;

struct sWalkMeshAccess
{
    u16 p1;
    u16 p2;
    u16 p3;
};
std::vector< sWalkMeshAccess > field_walkmesh_access;



std::vector<u8> g_field_random =
{
    0xb1, 0xca, 0xee, 0x6c, 0x5a, 0x71, 0x2e, 0x55, 0xd6, 0x00, 0xcc, 0x99, 0x90, 0x6b, 0x7d, 0xeb,
    0x4f, 0xa0, 0x07, 0xac, 0xdf, 0x8a, 0x56, 0x9e, 0xf1, 0x9a, 0x63, 0x75, 0x11, 0x91, 0xa3, 0xb8,
    0x94, 0x73, 0xf7, 0x54, 0xd9, 0x6e, 0x72, 0xc0, 0xf4, 0x80, 0xde, 0xb9, 0xbb, 0x8d, 0x66, 0x26,
    0xd0, 0x36, 0xe1, 0xe9, 0x70, 0xdc, 0xcd, 0x2f, 0x4a, 0x67, 0x5d, 0xd2, 0x60, 0xb5, 0x9d, 0x7f,
    0x45, 0x37, 0x50, 0x44, 0x78, 0x04, 0x19, 0x2c, 0xef, 0xfd, 0x64, 0x81, 0x03, 0xda, 0x95, 0x4c,
    0x7a, 0x0b, 0xad, 0x1f, 0xba, 0xdd, 0x3e, 0xf9, 0xd7, 0x1a, 0x29, 0xf8, 0x18, 0xb3, 0x20, 0xf6,
    0xd1, 0x5e, 0x34, 0x92, 0x7b, 0x24, 0x43, 0x88, 0x97, 0xd4, 0x0f, 0x35, 0xaa, 0x83, 0x68, 0x27,
    0xa8, 0xd5, 0xbe, 0xfa, 0x14, 0x31, 0xaf, 0x10, 0x0d, 0xd8, 0x6a, 0xce, 0x23, 0x61, 0xf3, 0x3d,
    0xa4, 0x08, 0x33, 0xe3, 0xa9, 0x38, 0xe6, 0x93, 0x1d, 0x1c, 0xf0, 0x0e, 0x87, 0x59, 0x65, 0x82,
    0xbc, 0xff, 0xfe, 0x7e, 0x8f, 0xc1, 0x1e, 0xf5, 0xcb, 0x49, 0x02, 0x32, 0x09, 0xc4, 0x8e, 0xc6,
    0x2b, 0x40, 0xa7, 0x17, 0x76, 0x3b, 0x16, 0x2a, 0xc8, 0xfb, 0xb2, 0x58, 0xa5, 0x15, 0xae, 0x25,
    0xcf, 0x46, 0xc7, 0x48, 0xb4, 0x0a, 0x3f, 0xc9, 0x06, 0x85, 0x51, 0x89, 0x62, 0x4d, 0x12, 0x8c,
    0xea, 0xa2, 0x98, 0x4b, 0x79, 0x6f, 0x5c, 0x47, 0x30, 0x1b, 0xe7, 0xc5, 0x22, 0x9c, 0xe8, 0x96,
    0x3a, 0xe4, 0x7c, 0xe0, 0x69, 0xa1, 0xb7, 0x05, 0x39, 0x74, 0x01, 0x9f, 0xbd, 0xc3, 0x84, 0xfc,
    0x77, 0x86, 0x13, 0x4e, 0xbf, 0xf2, 0x53, 0x5b, 0xed, 0x21, 0x8b, 0x6d, 0xc2, 0x41, 0xb6, 0xdb,
    0x3c, 0xd3, 0x28, 0xec, 0x2d, 0xe2, 0x9b, 0xa6, 0x42, 0x52, 0x57, 0x5f, 0xe5, 0xab, 0xb0, 0x0c,
};



void FieldMainLoop();
void FieldUpdateDrawEnv();
void FieldLoadMimDatFiles();
void FieldLoadMimToVram();
void FieldUpdateEnv();
void FieldCameraAssign();
u32 FieldButtonsUpdate();



void FieldCorruptedFilesCheck()
{
    std::string name = g_field_files[g_field_map_id];

    if ((name == "BLACKBGA")
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
     || (name == "TUNNEL_6") // DAT file is loading, but MIM seems to broken
     || (name.compare(0, 2, "WM") == 0)
     || (name == "XMVTES"))
    {
        LOG_ERROR("Field map \"%s\" (0x%x) corrupted. Replace it with \"DUMMY\" (0x0)", name.c_str(), g_field_map_id);
        g_field_map_id = 0;
    }
}



void FieldMain()
{
    PsyqClearOTagR(&l_field_render_data[0].ot_fade_drawenv, 0x1);
    PsyqClearOTagR(&l_field_render_data[1].ot_fade_drawenv, 0x1);
    PsyqSetDrawEnv(&l_field_render_data[0].fade_drawenv, &l_main_drawenv[0]);
    PsyqSetDrawEnv(&l_field_render_data[1].fade_drawenv, &l_main_drawenv[1]);
    PsyqAddPrim(&l_field_render_data[0].ot_fade_drawenv, &l_field_render_data[0].fade_drawenv);
    PsyqAddPrim(&l_field_render_data[1].ot_fade_drawenv, &l_field_render_data[1].fade_drawenv);

    PsyqSetDefDrawEnv(&l_drawenv[0], 0x0, 0x8, 0x140, 0xe0);
    l_drawenv[0].dtd = 0x1;
    l_drawenv[0].isbg = 0;
    PsyqSetDefDrawEnv(&l_drawenv[1], 0x0, 0xf0, 0x140, 0xe0);
    l_drawenv[1].dtd = 0x1;
    l_drawenv[1].isbg = 0;

    PsyqClearOTagR(&l_field_render_data[0].ot_scene_drawenv, 0x1);
    PsyqClearOTagR(&l_field_render_data[1].ot_scene_drawenv, 0x1);
    PsyqSetDrawEnv(&l_field_render_data[0].scene_drawenv, &l_drawenv[0]);
    PsyqSetDrawEnv(&l_field_render_data[1].scene_drawenv, &l_drawenv[1]);
    PsyqAddPrim(&l_field_render_data[0].ot_scene_drawenv, &l_field_render_data[0].scene_drawenv);
    PsyqAddPrim(&l_field_render_data[1].ot_scene_drawenv, &l_field_render_data[1].scene_drawenv);

    // It's not nessesary to call init every time field is loading
    // but original code do it.
    FadeInitPoly();

    if ((g_gamestate_prev != GAME_STATE_FIELD))
    {
        SRECT rect;
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x1e0;
        rect.h = 0x1d8;
        PsyqClearImage(&rect, 0, 0, 0);
    }

    while (true)
    {
        // Some files in FFVII are in some strange state. They not used in game
        // but they are in file list and has their own ID
        // to prevent crush this is check for those files.
        // If try to load corrupt file it will be replaced with DUMMY map.
        FieldCorruptedFilesCheck();

        if ((g_gamestate_prev == GAME_STATE_FIELD) || (g_gamestate_prev == GAME_STATE_WORLDMAP))
        {
            if (g_field_control.fade_type == FADE_TYPE_NONE)
            {
                FadeCopyScreen();

                g_field_control.fade_type = FADE_TYPE_BG_SLOW_SUB;
                g_field_control.fade_steps = 0;
            }
        }

        if ((g_gamestate_prev != GAME_STATE_MENU) && (g_gamestate_prev != GAME_STATE_MENU_FUNC))
        {
            FieldLoadMimDatFiles();
        }

        // in real game background fade rendering processed in separate vsync thread
        // there are few variables that enable or disable this update
        while (g_field_control.fade_type != FADE_TYPE_NONE) FieldFadeBgDraw();

        PsyqDrawSync(0);

        if (g_gamestate_prev != GAME_STATE_MENU_FUNC)
        {
            g_field_control.fade_type = FADE_TYPE_DIS_GRAD_SUB;
            g_field_control.fade_steps = 0x100;
            g_field_control.fade_step = 0x10;
            g_field_control.fade_r = 0x0;
            g_field_control.fade_g = 0x0;
            g_field_control.fade_b = 0x0;
        }

        if ((g_gamestate_prev != GAME_STATE_MENU) && (g_gamestate_prev != GAME_STATE_MENU_FUNC))
        {
            FieldLoadMimToVram();
        }

        FieldEventInit();

        g_rain_force = (g_rain_enable & 0x80) ? 0xff : 0;

        FieldMainLoop();

        while (PsyqDrawSync(0x1) != 0) {}
        PsyqVSync(0x1);

        g_main_dispenv[0x0].isrgb24 = 0;
        g_main_dispenv[0x1].isrgb24 = 0;

        PsyqPutDispEnv(&g_main_dispenv[g_field_rb]);
        PsyqPutDrawEnv(&l_main_drawenv[g_field_rb]);

        g_gamestate_prev = GAME_STATE_FIELD;

        if (g_field_control.cmd == FIELD_CMD_RESET)
        {
            PsyqVSync(0);
            return;
        }

        if (g_field_control.cmd == FIELD_CMD_MAP)
        {
            g_field_map_id = g_field_control.arg;
        }
    }
}



void FieldMainLoop()
{
    l_field_ofs_x = 0xa0;
    l_field_ofs_y = 0x78;

    if ((g_gamestate_prev != GAME_STATE_MENU) && (g_gamestate_prev != GAME_STATE_MENU_FUNC))
    {
        FieldModelLoad();
    }

    if ((g_gamestate_prev != GAME_STATE_MENU) && (g_gamestate_prev != GAME_STATE_BATTLE) && (g_gamestate_prev != GAME_STATE_MENU_FUNC))
    {
        FieldEntityInitPos();
    }

    FieldBackgroundInitPoly(l_field_render_data[0].bg_1, l_field_render_data[0].bg_2, l_field_render_data[0].bg_anim, l_field_render_data[0].bg_dm);
    FieldBackgroundInitPoly(l_field_render_data[1].bg_1, l_field_render_data[1].bg_2, l_field_render_data[1].bg_anim, l_field_render_data[1].bg_dm);

    FieldRainInit(&l_field_render_data[0]);
    FieldRainInit(&l_field_render_data[1]);

    bool first_frame = true;

    while (true)
    {
        if (first_frame == false) g_field_rb += 0x1;
        g_field_rb &= 0x1;

        FieldRenderData& render_data = l_field_render_data[g_field_rb];

        PsyqClearOTagR(render_data.ot_scene, 0x1000);

        FieldCameraAssign();
        l_buttons_state = FieldButtonsUpdate();

        if (l_buttons_state == BUTTON_LEFT) l_debug_ofs_x += 0x5;
        if (l_buttons_state == BUTTON_RIGHT) l_debug_ofs_x -= 0x5;
        if (l_buttons_state == BUTTON_UP) l_debug_ofs_y += 0x5;
        if (l_buttons_state == BUTTON_DOWN) l_debug_ofs_y -= 0x5;

        FieldUpdateEnv();

        u32 reset_check = BUTTON_START | BUTTON_SELECT | BUTTON_R1 | BUTTON_L1 | BUTTON_R2 | BUTTON_L2;
        if ((g_field_control.btn_pressed & reset_check) == reset_check)
        {
            g_field_control.cmd = FIELD_CMD_RESET;
            return;
        }

        if (g_field_control.cmd == FIELD_CMD_MAP)
        {
            return;
        }

        FieldBackgroundAddToRender(render_data);

        FieldRainUpdate();
        FieldRainAddToRender(render_data.ot_scene, render_data.rain.data(), &l_field_camera.m, &render_data.rain_dm);

        FadeUpdate();

        FieldEntityDebugAddToRender(render_data.ot_scene, &l_field_camera.m);

        while (PsyqDrawSync(0x1) != 0) {}

        PsyqVSync(0x2);

        if (first_frame == true)
        {
            first_frame = false;
            PsyqSetDispMask(0x1);
        }

        PsyqPutDispEnv(&g_main_dispenv[g_field_rb]);
        PsyqPutDrawEnv(&l_main_drawenv[g_field_rb]);

        PsyqClearImage(&(l_main_drawenv[g_field_rb].clip), 0, 0, 0);

        if (g_field_control.disable_render == 0)
        {
            PsyqDrawOTag(&render_data.ot_scene_drawenv);
            PsyqDrawOTag(render_data.ot_scene + 0x1000 - 0x1);
            PsyqDrawOTag(&render_data.ot_fade_drawenv);

            if (g_field_control.fade_type != FADE_TYPE_NONE)
            {
                PsyqDrawOTag(&g_fade_ot[g_field_rb]);
            }
        }
    }
}



void FieldFadeBgDraw()
{
    g_field_rb += 0x1;
    g_field_rb &= 0x1;

    FadeUpdate();

    PsyqDrawSync(0);

    PsyqPutDispEnv(&g_main_dispenv[g_field_rb]);
    PsyqPutDrawEnv(&l_main_drawenv[g_field_rb]);
    PsyqDrawOTag(&g_fade_ot[g_field_rb]);
}



void FieldUpdateDrawEnv()
{
}



void FieldLoadMimDatFiles()
{
    FileLZS("FIELD/" + g_field_files[g_field_map_id] + ".MIM", l_field_mim);
    FileLZS("FIELD/" + g_field_files[g_field_map_id] + ".DAT", g_field_dat);

    u32 wm_addr = READ_LE_U32(&g_field_dat[0x4]) - g_field_dat_base_addr;
    u32 id_n = READ_LE_U32(&g_field_dat[wm_addr]);
    wm_addr += 0x4;
    u32 wm_link_addr = wm_addr + 0x4 + id_n * 0x18;

    g_field_wm.clear();
    g_field_wm_link.clear();

    for (u32 i = 0; i < id_n; ++i)
    {
        FieldWalkmesh wm;
        wm.p1.vx = READ_LE_S16(&g_field_dat[wm_addr + i * 0x18 + 0x0] );
        wm.p1.vy = READ_LE_S16(&g_field_dat[wm_addr + i * 0x18 + 0x2] );
        wm.p1.vz = READ_LE_S16(&g_field_dat[wm_addr + i * 0x18 + 0x4] );
        wm.p2.vx = READ_LE_S16(&g_field_dat[wm_addr + i * 0x18 + 0x8] );
        wm.p2.vy = READ_LE_S16(&g_field_dat[wm_addr + i * 0x18 + 0xa] );
        wm.p2.vz = READ_LE_S16(&g_field_dat[wm_addr + i * 0x18 + 0xc] );
        wm.p3.vx = READ_LE_S16(&g_field_dat[wm_addr + i * 0x18 + 0x10]);
        wm.p3.vy = READ_LE_S16(&g_field_dat[wm_addr + i * 0x18 + 0x12]);
        wm.p3.vz = READ_LE_S16(&g_field_dat[wm_addr + i * 0x18 + 0x14]);
        g_field_wm.push_back(wm);

        FieldWalkmeshLink wm_link;
        wm_link.p1_id = READ_LE_U16(&g_field_dat[wm_link_addr + i * 0x6 + 0x0]);
        wm_link.p2_id = READ_LE_U16(&g_field_dat[wm_link_addr + i * 0x6 + 0x2]);
        wm_link.p3_id = READ_LE_U16(&g_field_dat[wm_link_addr + i * 0x6 + 0x4]);
        g_field_wm_link.push_back(wm_link);
    }
}



void FieldLoadMimToVram()
{
    u32 ofs = 0;

    PsyqDrawSync(0);

    // load palette to vram
    SRECT rect;
    rect.x = 0;
    rect.y = 0x1e0;
    rect.w = 0x100;
    rect.h = 0x10;
    PsyqLoadImage(&rect, &l_field_mim[ofs + 0xc]);
    PsyqDrawSync(0);

    // load 1st image to vram
    ofs += (READ_LE_U32(&l_field_mim[ofs + 0x0]) >> 0x2) << 0x2;
    PsyqLoadTPage(
        &l_field_mim[ofs + 0xc],
        0x1,
        0,
        READ_LE_U16(&l_field_mim[ofs + 0x4]),
        READ_LE_U16(&l_field_mim[ofs + 0x6]),
        READ_LE_U16(&l_field_mim[ofs + 0x8]) * 0x2,
        READ_LE_U16(&l_field_mim[ofs + 0xa])
    );
    PsyqDrawSync(0);

    // load 2nd image to vram
    ofs += (READ_LE_U32(&l_field_mim[ofs + 0x0]) >> 0x2) << 0x2;
    u32 next = READ_LE_U32(&l_field_mim[ofs + 0x0]);
    if (next != 0)
    {
        PsyqLoadTPage(
            &l_field_mim[ofs + 0xc],
            0x1,
            0,
            READ_LE_U16(&l_field_mim[ofs + 0x4]),
            READ_LE_U16(&l_field_mim[ofs + 0x6]),
            READ_LE_U16(&l_field_mim[ofs + 0x8]) * 0x2,
            READ_LE_U16(&l_field_mim[ofs + 0xa])
        );
        PsyqDrawSync(0);
    }
}



void FieldInitEnv()
{
    PsyqSetDefDispEnv(&g_main_dispenv[0x0], 0, 0xe8, 0x140, 0xf0);
    PsyqSetDefDispEnv(&g_main_dispenv[0x1], 0,    0, 0x140, 0xf0);

    PsyqSetDefDrawEnv(&l_main_drawenv[0x0], 0, 0x8, 0x140, 0xe0);
    l_main_drawenv[0x0].dtd = 0x1;
    l_main_drawenv[0x0].isbg = 0;

    PsyqSetDefDrawEnv(&l_main_drawenv[0x1], 0, 0xf0, 0x140, 0xe0);
    l_main_drawenv[0x1].dtd = 0x1;
    l_main_drawenv[0x1].isbg = 0;

    PsyqPutDispEnv(&g_main_dispenv[0]);
    PsyqPutDrawEnv(&l_main_drawenv[0]);
}



void FieldUpdateEnv()
{
    DRAWENV* drawenv1 = &l_drawenv[0x0];
    DRAWENV* drawenv2 = &l_drawenv[0x1];

    PsyqSetGeomScreen(l_field_camera.h);
    PsyqSetGeomOffset(0, 0);

    s16 ofsx = l_field_ofs_x - l_field_camera.x + l_debug_ofs_x;
    s16 ofsy = l_field_ofs_y + l_field_camera.y + l_debug_ofs_y;

    if (g_field_rb == 0)
    {
        drawenv1->ofs[0] = ofsx;
        drawenv1->ofs[1] = ofsy;
        PsyqSetDrawEnv(&l_field_render_data[0x0].scene_drawenv, drawenv1);
    }
    else
    {
        drawenv2->ofs[0] = ofsx;
        drawenv2->ofs[1] = ofsy + 0xe8;
        PsyqSetDrawEnv(&l_field_render_data[0x1].scene_drawenv, drawenv2);
    }
}



void FieldCameraAssign()
{
    u32 camera_ofs = READ_LE_U32(&g_field_dat[0xc]) - g_field_dat_base_addr;

    l_field_camera.m.m[0][0] = READ_LE_S16(&g_field_dat[camera_ofs + 0x0]);
    l_field_camera.m.m[0][1] = READ_LE_S16(&g_field_dat[camera_ofs + 0x2]);
    l_field_camera.m.m[0][2] = READ_LE_S16(&g_field_dat[camera_ofs + 0x4]);
    l_field_camera.m.m[1][0] = READ_LE_S16(&g_field_dat[camera_ofs + 0x6]);
    l_field_camera.m.m[1][1] = READ_LE_S16(&g_field_dat[camera_ofs + 0x8]);
    l_field_camera.m.m[1][2] = READ_LE_S16(&g_field_dat[camera_ofs + 0xa]);
    l_field_camera.m.m[2][0] = READ_LE_S16(&g_field_dat[camera_ofs + 0xc]);
    l_field_camera.m.m[2][1] = READ_LE_S16(&g_field_dat[camera_ofs + 0xe]);
    l_field_camera.m.m[2][2] = READ_LE_S16(&g_field_dat[camera_ofs + 0x10]);
    l_field_camera.m.t[0] = READ_LE_S16(&g_field_dat[camera_ofs + 0x14]);
    l_field_camera.m.t[1] = READ_LE_S16(&g_field_dat[camera_ofs + 0x18]);
    l_field_camera.m.t[2] = READ_LE_S16(&g_field_dat[camera_ofs + 0x1c]);
    l_field_camera.x = READ_LE_U16(&g_field_dat[camera_ofs + 0x20]);
    l_field_camera.y = READ_LE_U16(&g_field_dat[camera_ofs + 0x22]);
    l_field_camera.h = READ_LE_S16(&g_field_dat[camera_ofs + 0x24]);
}



u32 FieldButtonsUpdate()
{
    l_buttons_state = ButtonsGetCurrent();

    u32 prev = g_field_control.btn_pressed;
    g_field_control.btn_pressed = l_buttons_state;
    g_field_control.btn_prev = prev;
    g_field_control.btn_new = (l_buttons_state ^ prev) & l_buttons_state;
    g_field_control.btn_released = (l_buttons_state ^ prev) & ~l_buttons_state;

    return l_buttons_state;
}
