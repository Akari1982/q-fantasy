#include "debug_vram.h"
#include "psyq/psxgpu.h"

#include "ofxImGui.h"

bool g_debug_vram = false;

#include "application.h"

void DebugVram()
{
    if( g_debug_vram == false ) return;

    ImGui::SetNextWindowPos( ImGui::GetWindowPos() + ImVec2( 0, 0 ), ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowSize( ImVec2( 1024, 534 ), ImGuiCond_Once );

    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );

    if( ImGui::Begin( "Debug Vram", &g_debug_vram, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse ) )
    {
        {
            static ofTexture texture;
            if( !texture.isAllocated() )
            {
                ofDisableArbTex();
                texture.allocate( 1024, 512, GL_RGBA8, false );
            }

            const u16* src = reinterpret_cast<const u16*>(g_vram.data());

            ofPixels pixels;
            pixels.allocate( 1024, 512, OF_PIXELS_RGBA );

            for( int i = 0; i < 1024 * 512; ++i )
            {
                u16 color = src[i];

                u8 b = ((color >> 0xa) & 0x1f) << 3;
                u8 g = ((color >> 0x5) & 0x1f) << 3;
                u8 r = ((color >> 0x0) & 0x1f) << 3;

                int index = i * 4;
                pixels[index + 0] = r; // Red
                pixels[index + 1] = g; // Green
                pixels[index + 2] = b; // Blue
                pixels[index + 3] = 255; // Alpha
            }

            texture.loadData(pixels.getData(), 1024, 512, GL_RGBA);
            GLuint textureId = texture.getTextureData().textureID;
            texture.getTextureData().textureTarget = GL_TEXTURE_2D;

            ImGui::BeginGroup();
            ImGui::Image( (void*)(intptr_t)textureId, ImVec2( 1024, 512 ) );
            ImGui::EndGroup();
        }
    }
    ImGui::End();

    ImGui::PopStyleVar( 1 );
}
