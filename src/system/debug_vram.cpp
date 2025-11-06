#include "debug_vram.h"
#include "psyq/psxgpu.h"
#include "ofxImGui.h"
#include "application.h"

bool g_debug_vram = false;

ofFbo l_debug_fbo;



void DebugVram()
{
    if( g_debug_vram == false ) return;

    if( l_debug_fbo.isAllocated() == false )
    {
        l_debug_fbo.allocate( VRAM_W, VRAM_H, GL_RGBA );
    }

    ImGui::SetNextWindowPos( ImGui::GetWindowPos() + ImVec2( 0, 0 ), ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowSize( ImVec2( VRAM_W, VRAM_H + 24 ), ImGuiCond_Once );

    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );

    if( ImGui::Begin( "Debug Vram", &g_debug_vram, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse ) )
    {
        ImGui::BeginGroup();

        l_debug_fbo.begin();
        ofClear( 0, 0, 0, 0 );

        static ofVboMesh mesh;
        mesh.clear();
        mesh.setMode( OF_PRIMITIVE_TRIANGLE_FAN );

        mesh.addVertex( glm::vec3(  0.0f,   0.0f, 0.0f) );
        mesh.addVertex( glm::vec3(VRAM_W,   0.0f, 0.0f) );
        mesh.addVertex( glm::vec3(VRAM_W, VRAM_H, 0.0f) );
        mesh.addVertex( glm::vec3(  0.0f, VRAM_H, 0.0f) );

        mesh.addTexCoord( glm::vec2(  0.0f,   0.0f) );
        mesh.addTexCoord( glm::vec2(VRAM_W,   0.0f) );
        mesh.addTexCoord( glm::vec2(VRAM_W, VRAM_H) );
        mesh.addTexCoord( glm::vec2(  0.0f, VRAM_H) );

        glm::mat4 projection = glm::ortho( 0.0f, (float)VRAM_W, 0.0f, (float)VRAM_H, -1.0f, 1.0f );

        g_display_shader.begin();
        g_display_shader.setUniformTexture( "tex0", g_vram_texture, 0 );
        g_display_shader.setUniformMatrix4f( "g_matrix", projection );
        mesh.draw();
        g_display_shader.end();

        l_debug_fbo.end();

        GLuint textureId = l_debug_fbo.getTexture().getTextureData().textureID;
        ImGui::Image( (void*)(intptr_t)textureId, ImVec2( VRAM_W, VRAM_H ) );

        g_display_shader.end();

        ImGui::EndGroup();
    }
    ImGui::End();

    ImGui::PopStyleVar( 1 );
}
