#include "debug_field.h"
#include "system/icons_font.h"
#include "field/field.h"
#include "field/field_list.h"
#include "field/rain.h"

#include "ofxImGui.h"
#include <format>



void DebugField()
{
    ImGui::SetNextWindowPos( ImGui::GetWindowPos() + ImVec2( 900, 0 ), ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowSize( ImVec2( 400, 650 ), ImGuiCond_Once );
    if( ImGui::Begin( "Field Control", nullptr, 0 ) )
    {
        // rain control
        bool rain_enable = (g_rain_enable & 0x80) != 0;
        if ( ImGui::Checkbox( "Enable Rain", &rain_enable ) )
        {
            if( rain_enable ) g_rain_enable |= 0x80;
            else              g_rain_enable &= ~0x80;
        }
        ImGui::SameLine();
        ImGui::Text( "(0x%02x)", g_rain_force );

        // map selection combo box
        // map loaded directly after selection
        const char* selected_map = g_field_files[g_field_map_id].c_str();
        if( ImGui::BeginCombo( "Jump to Map", selected_map ) )
        {
            for( int i = 0; i < g_field_files.size(); ++i )
            {
                const bool is_selected = (g_field_map_id == i);
                if( ImGui::Selectable( g_field_files[i].c_str(), is_selected ) )
                {
                    g_field_control.cmd = FIELD_CMD_LOAD;
                    g_field_control.map_id = i;
                }

                if( is_selected ) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
    ImGui::End();
}
