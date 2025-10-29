#include "debug.h"
#include "debug_field.h"
#include "browser_akao.h"
#include "browser_ending.h"
#include "browser_field.h"
#include "browser_field_opcodes.h"
#include "kernel/game.h"
#include "ofxImGui.h"



void DebugInit()
{
    FieldBrowserInitOpcode();
}



void DebugUpdate()
{
    BrowserField();
    BrowserEnding();
    BrowserAkao();

    if( g_game_state == GAME_STATE_FIELD ) DebugField();

    if( ImGui::BeginMainMenuBar() )
    {
        if( ImGui::MenuItem( "Field", nullptr, &g_browser_field ) ) {}
        if( ImGui::MenuItem( "Ending", nullptr, &g_browser_ending ) ) {}
        if( ImGui::MenuItem( "Akao", nullptr, &g_browser_akao ) ) {}

        ImGui::EndMainMenuBar();
    }
}
