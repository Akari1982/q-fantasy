#include "psxgpu.h"
#include "libgpu.h"
#include "system/logger.h"
#include "ofGraphics.h"



ofTexture g_vram_texture;
ofShader g_display_shader;
ofShader g_render_shader;
GLuint g_fbo_id;

// rendering settings
u32 g_rendering_disp_enable = 0;
u32 g_rendering_disp_x = 0;
u32 g_rendering_disp_y = 0;
u32 g_rendering_disp_w = 0;
u32 g_rendering_disp_h = 0;

s16 l_ofs_x = 0;
s16 l_ofs_y = 0;
u16 l_tpage = 0;
u32 l_dtd = 0;



void GPUInit()
{
    ofDisableArbTex();

    // init texture for vram
    GLuint tex_id;
    glGenTextures( 1, &tex_id );
    glBindTexture( GL_TEXTURE_2D, tex_id );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    std::vector<unsigned short> texture_data(VRAM_W * VRAM_H, 0x0000);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_R16UI, VRAM_W, VRAM_H, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, texture_data.data() );
    glBindTexture( GL_TEXTURE_2D, 0 );

    // link with ofTexture
    g_vram_texture.setUseExternalTextureID( tex_id );
    g_vram_texture.texData.width = VRAM_W;
    g_vram_texture.texData.height = VRAM_H;
    g_vram_texture.texData.tex_w = VRAM_W;
    g_vram_texture.texData.tex_h = VRAM_H;
    g_vram_texture.texData.glInternalFormat = GL_R16UI;
    g_vram_texture.texData.textureTarget = GL_TEXTURE_2D;
    g_vram_texture.texData.bFlipTexture = false;
    g_vram_texture.texData.bAllocated = true;

    // create framebuffer
    glGenFramebuffers( 1, &g_fbo_id );
    glBindFramebuffer( GL_FRAMEBUFFER, g_fbo_id );
 
    // attach vram to framebuffer
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_id, 0 );
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers( 1, drawBuffers );

    GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
    if( status != GL_FRAMEBUFFER_COMPLETE )
    {
        LOG_ERROR( "FBO is incomplete! Status: %04x", (int)status );
    }
    else
    {
        LOG_INFO( "VRAM FBO successfully created and is complete." );
    }

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    // load shaders to render polygons to GL_R16UI vram
    g_render_shader.load( "../system/shader_render.vert", "../system/shader_render.frag" );

    // load shader for rendering GL_R16UI vram
    g_display_shader.load( "../system/shader_display.vert", "../system/shader_display.frag" );

}



void OTag::execute()
{
    if( type == GPU_OTAG )
    {
        return;
    }
    else if( type == GPU_TERMINATE )
    {
        return;
    }
    else if( type == GPU_POLY_FT4 )
    {
        ((POLY_FT4*)this)->execute();
    }
    else if( type == GPU_LINE_F2 )
    {
        ((LINE_F2*)this)->execute();
    }
    else if( type == GPU_SPRT )
    {
        ((SPRT*)this)->execute();
    }
    else if( type == GPU_SPRT_16 )
    {
        ((SPRT_16*)this)->execute();
    }
    else if( type == GPU_DR_MODE )
    {
        ((DR_MODE*)this)->execute();
    }
    else if( type == GPU_DR_ENV )
    {
        ((DR_ENV*)this)->execute();
    }
    else
    {
        LOG_ERROR( "Unsupported OTag: type = 0x0x", type );
    }
}



void LINE_F2::execute()
{
    static ofVboMesh mesh;
    mesh.clear();
    mesh.setMode( OF_PRIMITIVE_LINES );

    mesh.addVertex( glm::vec3(l_ofs_x + x0, l_ofs_y + y0, 0.0f) );
    mesh.addVertex( glm::vec3(l_ofs_x + x1, l_ofs_y + y1, 0.0f) );

    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );
    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );

    glm::mat4 projection = glm::ortho(0.0f, (float)VRAM_W, 0.0f, (float)VRAM_H, -1.0f, 1.0f);

    ofSetLineWidth( 1 );

    g_render_shader.setUniformMatrix4f( "g_matrix", projection );
    g_render_shader.setUniformTexture( "g_texture", g_vram_texture, 0 );
    g_render_shader.setUniform1i( "g_transp", (code & 0x2) ? 1 : 0 );
    g_render_shader.setUniform1i( "g_colored", 1 );
    g_render_shader.setUniform1i( "g_textured", 0 );
    g_render_shader.setUniform1i( "g_abr", (l_tpage >> 0x5) & 0x3 );
    g_render_shader.setUniform1i( "g_dtd", l_dtd );
    mesh.draw();
}



void POLY_FT4::execute()
{
    static ofVboMesh mesh;
    mesh.clear();
    mesh.setMode( OF_PRIMITIVE_TRIANGLE_FAN );

    mesh.addVertex( glm::vec3(l_ofs_x + x0, l_ofs_y + y0, 0) );
    mesh.addVertex( glm::vec3(l_ofs_x + x1, l_ofs_y + y1, 0) );
    mesh.addVertex( glm::vec3(l_ofs_x + x3, l_ofs_y + y3, 0) );
    mesh.addVertex( glm::vec3(l_ofs_x + x2, l_ofs_y + y2, 0) );

    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );
    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );
    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );
    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );

    mesh.addTexCoord( glm::vec2(u0, v0) );
    mesh.addTexCoord( glm::vec2(u1, v1) );
    mesh.addTexCoord( glm::vec2(u3, v3) );
    mesh.addTexCoord( glm::vec2(u2, v2) );

    glm::mat4 projection = glm::ortho(0.0f, (float)VRAM_W, 0.0f, (float)VRAM_H, -1.0f, 1.0f);

    g_render_shader.setUniformMatrix4f( "g_matrix", projection );
    g_render_shader.setUniformTexture( "g_texture", g_vram_texture, 0 );
    g_render_shader.setUniform2i( "g_clut", (clut & 0x3f) * 0x10, (clut & 0xffc0) >> 0x6 );
    g_render_shader.setUniform2i( "g_tpage", (tpage << 0x6) & 0x3ff, (tpage << 0x4) & 0x100 );
    g_render_shader.setUniform1i( "g_depth", (tpage >> 0x7) & 0x3 );
    g_render_shader.setUniform1i( "g_transp", (code & 0x2) ? 1 : 0 );
    g_render_shader.setUniform1i( "g_colored", (code & 0x1) ? 0 : 1 );
    g_render_shader.setUniform1i( "g_textured", (code & 0x4) ? 1 : 0 );
    g_render_shader.setUniform1i( "g_abr", (tpage >> 0x5) & 0x3 );
    g_render_shader.setUniform1i( "g_dtd", l_dtd );
    mesh.draw();
}



void SPRT::execute()
{
    static ofVboMesh mesh;
    mesh.clear();
    mesh.setMode( OF_PRIMITIVE_TRIANGLE_FAN );

    mesh.addVertex( glm::vec3(l_ofs_x + x0,     l_ofs_y + y0,     0) );
    mesh.addVertex( glm::vec3(l_ofs_x + x0 + w, l_ofs_y + y0,     0) );
    mesh.addVertex( glm::vec3(l_ofs_x + x0 + w, l_ofs_y + y0 + h, 0) );
    mesh.addVertex( glm::vec3(l_ofs_x + x0,     l_ofs_y + y0 + h, 0) );

    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );
    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );
    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );
    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );

    mesh.addTexCoord( glm::vec2(u0,     v0    ) );
    mesh.addTexCoord( glm::vec2(u0 + w, v0    ) );
    mesh.addTexCoord( glm::vec2(u0 + w, v0 + h) );
    mesh.addTexCoord( glm::vec2(u0,     v0 + h) );

    glm::mat4 projection = glm::ortho(0.0f, (float)VRAM_W, 0.0f, (float)VRAM_H, -1.0f, 1.0f);

    g_render_shader.setUniformMatrix4f( "g_matrix", projection );
    g_render_shader.setUniformTexture( "g_texture", g_vram_texture, 0 );
    g_render_shader.setUniform2i( "g_clut", (clut & 0x3f) * 0x10, (clut & 0xffc0) >> 0x6 );
    g_render_shader.setUniform2i( "g_tpage", (l_tpage << 0x6) & 0x3ff, (l_tpage << 0x4) & 0x100 );
    g_render_shader.setUniform1i( "g_depth", (l_tpage >> 0x7) & 0x3 );
    g_render_shader.setUniform1i( "g_transp", (code & 0x2) ? 1 : 0 );
    g_render_shader.setUniform1i( "g_colored", (code & 0x1) ? 0 : 1 );
    g_render_shader.setUniform1i( "g_textured", (code & 0x4) ? 1 : 0 );
    g_render_shader.setUniform1i( "g_abr", (l_tpage >> 0x5) & 0x3 );
    g_render_shader.setUniform1i( "g_dtd", 0 );
    mesh.draw();
}



void SPRT_16::execute()
{
    static ofVboMesh mesh;
    mesh.clear();
    mesh.setMode( OF_PRIMITIVE_TRIANGLE_FAN );

    mesh.addVertex( glm::vec3(l_ofs_x + x0,         l_ofs_y + y0,         0.0f) );
    mesh.addVertex( glm::vec3(l_ofs_x + x0 + 16.0f, l_ofs_y + y0,         0.0f) );
    mesh.addVertex( glm::vec3(l_ofs_x + x0 + 16.0f, l_ofs_y + y0 + 16.0f, 0.0f) );
    mesh.addVertex( glm::vec3(l_ofs_x + x0,         l_ofs_y + y0 + 16.0f, 0.0f) );

    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );
    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );
    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );
    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );

    mesh.addTexCoord( glm::vec2(u0,         v0        ) );
    mesh.addTexCoord( glm::vec2(u0 + 16.0f, v0        ) );
    mesh.addTexCoord( glm::vec2(u0 + 16.0f, v0 + 16.0f) );
    mesh.addTexCoord( glm::vec2(u0,         v0 + 16.0f) );

    glm::mat4 projection = glm::ortho(0.0f, (float)VRAM_W, 0.0f, (float)VRAM_H, -1.0f, 1.0f);

    g_render_shader.setUniformMatrix4f( "g_matrix", projection );
    g_render_shader.setUniformTexture( "g_texture", g_vram_texture, 0 );
    g_render_shader.setUniform2i( "g_clut", (clut & 0x3f) * 0x10, (clut & 0xffc0) >> 0x6 );
    g_render_shader.setUniform2i( "g_tpage", (l_tpage << 0x6) & 0x3ff, (l_tpage << 0x4) & 0x100 );
    g_render_shader.setUniform1i( "g_depth", (l_tpage >> 0x7) & 0x3 );
    g_render_shader.setUniform1i( "g_transp", (code & 0x2) ? 1 : 0 );
    g_render_shader.setUniform1i( "g_colored", (code & 0x1) ? 0 : 1 );
    g_render_shader.setUniform1i( "g_textured", (code & 0x4) ? 1 : 0 );
    g_render_shader.setUniform1i( "g_abr", (l_tpage >> 0x5) & 0x3 );
    g_render_shader.setUniform1i( "g_dtd", 0 );
    mesh.draw();
}



void DR_MODE::execute()
{
    l_dtd = dtd;
    l_tpage = tpage;
}



void DR_ENV::execute()
{
    glScissor( env.clip.x, env.clip.y, env.clip.w, env.clip.h );
    l_tpage = env.tpage;
    l_dtd = env.dtd;
    l_ofs_x = env.ofs[0];
    l_ofs_y = env.ofs[1];

    if( env.isbg == 1 )
    {
        PsyqClearImage( &env.clip, env.r0, env.g0, env.b0 );
    }
}
