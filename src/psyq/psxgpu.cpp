#include "psxgpu.h"
#include "libgpu.h"
#include "ofGraphics.h"



std::array<u8, VRAM_W * VRAM_H> g_vram;
ofTexture g_screen;
ofFbo l_render;
ofTexture l_render_inn;
ofTexture l_render_texture;
ofShader l_render_shader;

// rendering settings
u32 g_rendering_disp_x = 0;
u32 g_rendering_disp_y = 0;

s16 l_ofs_x = 0;
s16 l_ofs_y = 0;
u16 l_tpage = 0;
u32 l_dtd = 0;



void GPUInit()
{
    ofDisableArbTex();
 
    ofFboSettings settings;
    settings.width = VRAM_W / 2;
    settings.height = VRAM_H;
    settings.numColorbuffers = 1;
    settings.internalformat = GL_RGBA;
    l_render.allocate( settings );

    l_render_inn.allocate( VRAM_W / 2, VRAM_H, GL_RGBA, false );
    l_render.attachTexture( l_render_inn, GL_RGBA, 0 );

    GLuint texID;
    glGenTextures( 1, &texID );
    glBindTexture( GL_TEXTURE_2D, texID );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_R8UI, VRAM_W, VRAM_H, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr );
    l_render_texture.setUseExternalTextureID( texID );
    l_render_texture.texData.width = VRAM_W;
    l_render_texture.texData.height = VRAM_H;
    l_render_texture.texData.tex_w = VRAM_W;
    l_render_texture.texData.tex_h = VRAM_H;
    l_render_texture.texData.glInternalFormat = GL_R8UI;
    l_render_texture.texData.textureTarget = GL_TEXTURE_2D;
    glBindTexture( GL_TEXTURE_2D, 0 );

    l_render_shader.load( "../system/psx_render.vert", "../system/psx_render.frag" );
}



void GPUUpdateRenderFromVram()
{
    int w = l_render.getWidth();
    int h = l_render.getHeight();

    static ofPixels pixels;
    if( !pixels.isAllocated() )
    {
        pixels.allocate( w, h, OF_PIXELS_RGBA );
    }

    for( int y = 0; y < h; ++y )
    {
        for( int x = 0; x < 400/*w*/; ++x )
        {
            u16 color = (g_vram[y * VRAM_W + x * 2 + 1] << 0x8) | g_vram[y * VRAM_W + x * 2 + 0];

            u8 r = ((color >> 0x0) & 0x1f) << 3;
            u8 g = ((color >> 0x5) & 0x1f) << 3;
            u8 b = ((color >> 0xa) & 0x1f) << 3;
            u8 a = ((color >> 0xf) == 0x1) ? 0x0 : 0xff;

            pixels.setColor( x, y, ofColor(r, g, b, a) );
        }
    }

    l_render_inn.loadData( pixels );
}



void GPUUpdateVramFromRender()
{
    ofPixels pixels;
    l_render_inn.readToPixels(pixels);

    int w = pixels.getWidth();
    int h = pixels.getHeight();

    for( int y = 0; y < h; ++y )
    {
        for( int x = 0; x < 400/*w*/; ++x )
        {
            ofColor color = pixels.getColor( x, y );

            u16 r = (color.r >> 3) & 0x1f;
            u16 g = (color.g >> 3) & 0x1f;
            u16 b = (color.b >> 3) & 0x1f;
            u16 a = (color.a == 0) ? 0x1 : 0;

            u16 rgba = (a << 0xf) | (b << 0xa) | (g << 0x5) | r;

            g_vram[(y * VRAM_W) + x * 2 + 0] = rgba & 0xff;
            g_vram[(y * VRAM_W) + x * 2 + 1] = (rgba >> 0x8) & 0xff;
        }
    }

    GPUUpdateTextureFromVram();
}



void GPUUpdateScreenFromVram()
{
    // if texture not yet allocated set to whole vram by default
    if( !g_screen.isAllocated() )
    {
        g_screen.allocate( VRAM_W / 2, VRAM_H, GL_RGBA8, false );
    }

    int w = g_screen.getWidth();
    int h = g_screen.getHeight();

    ofPixels pixels;
    pixels.allocate( w, h, OF_PIXELS_RGBA );

    for( int y = 0; y < h; ++y )
    {
        for( int x = 0; x < w; ++x )
        {
            int vram_x = g_rendering_disp_x + x;
            int vram_y = g_rendering_disp_y + y;
            u16 color = (g_vram[vram_y * VRAM_W + vram_x * 2 + 1] << 0x8) | g_vram[vram_y * VRAM_W + vram_x * 2 + 0];

            u8 r = ((color >> 0x0) & 0x1f) << 3;
            u8 g = ((color >> 0x5) & 0x1f) << 3;
            u8 b = ((color >> 0xa) & 0x1f) << 3;

            pixels.setColor( x, y, ofColor(r, g, b, 255) );
        }
    }

    g_screen.loadData( pixels );
}



void GPUUpdateTextureFromVram()
{
    static ofPixels pixels;
    if( !pixels.isAllocated() )
    {
        pixels.allocate( VRAM_W, VRAM_H, OF_IMAGE_GRAYSCALE );
    }
    memcpy( pixels.getData(), g_vram.data(), g_vram.size());
    l_render_texture.loadData( pixels.getData(), VRAM_W, VRAM_H, GL_RED_INTEGER, GL_UNSIGNED_BYTE );
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
        ofLog( OF_LOG_ERROR, "Unsupported OTag: type = 0x" + ofToHex( type ) );
    }
}



void LINE_F2::execute()
{
    l_render.begin();

    static ofVboMesh mesh;
    mesh.clear();
    mesh.setMode( OF_PRIMITIVE_LINES );

    mesh.addVertex( glm::vec3(l_ofs_x + x0, l_ofs_y + y0, 0) );
    mesh.addVertex( glm::vec3(l_ofs_x + x1, l_ofs_y + y1, 0) );

    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );
    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );

    glm::mat4 projection = glm::ortho( 0.0f, l_render.getWidth(), 0.0f, l_render.getHeight(), -1.0f, 1.0f );

    ofSetLineWidth( 1 );

    glEnable( GL_SCISSOR_TEST );
    l_render_shader.begin();
    l_render_shader.setUniformMatrix4f( "g_matrix", projection );
    l_render_shader.setUniformTexture( "g_texture", l_render_texture, 0 );
    l_render_shader.setUniform1i( "g_transp", (code & 0x2) ? 1 : 0 );
    l_render_shader.setUniform1i( "g_colored", 1 );
    l_render_shader.setUniform1i( "g_textured", 0 );
    l_render_shader.setUniform1i( "g_abr", (l_tpage >> 0x5) & 0x3 );
    l_render_shader.setUniform1i( "g_dtd", l_dtd );
    mesh.draw();
    l_render_shader.end();
    glDisable( GL_SCISSOR_TEST );

    l_render.end();
}



void POLY_FT4::execute()
{
    l_render.begin();

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

    glm::mat4 projection = glm::ortho( 0.0f, l_render.getWidth(), 0.0f, l_render.getHeight(), -1.0f, 1.0f );

    glEnable( GL_SCISSOR_TEST );
    l_render_shader.begin();
    l_render_shader.setUniformMatrix4f( "g_matrix", projection );
    l_render_shader.setUniformTexture( "g_texture", l_render_texture, 0 );
    l_render_shader.setUniformTexture( "g_background", l_render_inn, 1 );
    l_render_shader.setUniform2i( "g_clut", (clut & 0x3f) * 0x10, (clut & 0xffc0) >> 0x6 );
    l_render_shader.setUniform2i( "g_tpage", (tpage << 0x6) & 0x3ff, (tpage << 0x4) & 0x100 );
    l_render_shader.setUniform1i( "g_depth", (tpage >> 0x7) & 0x3 );
    l_render_shader.setUniform1i( "g_transp", (code & 0x2) ? 1 : 0 );
    l_render_shader.setUniform1i( "g_colored", (code & 0x1) ? 0 : 1 );
    l_render_shader.setUniform1i( "g_textured", (code & 0x4) ? 1 : 0 );
    l_render_shader.setUniform1i( "g_abr", (tpage >> 0x5) & 0x3 );
    l_render_shader.setUniform1i( "g_dtd", l_dtd );
    mesh.draw();
    l_render_shader.end();
    glDisable( GL_SCISSOR_TEST );

    l_render.end();
}



void SPRT::execute()
{
    l_render.begin();

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

    glm::mat4 projection = glm::ortho( 0.0f, l_render.getWidth(), 0.0f, l_render.getHeight(), -1.0f, 1.0f );

    glEnable( GL_SCISSOR_TEST );
    l_render_shader.begin();
    l_render_shader.setUniformMatrix4f( "g_matrix", projection );
    l_render_shader.setUniformTexture( "g_texture", l_render_texture, 0 );
    l_render_shader.setUniformTexture( "g_background", l_render_inn, 1 );
    l_render_shader.setUniform2i( "g_clut", (clut & 0x3f) * 0x10, (clut & 0xffc0) >> 0x6 );
    l_render_shader.setUniform2i( "g_tpage", (l_tpage << 0x6) & 0x3ff, (l_tpage << 0x4) & 0x100 );
    l_render_shader.setUniform1i( "g_depth", (l_tpage >> 0x7) & 0x3 );
    l_render_shader.setUniform1i( "g_transp", (code & 0x2) ? 1 : 0 );
    l_render_shader.setUniform1i( "g_colored", (code & 0x1) ? 0 : 1 );
    l_render_shader.setUniform1i( "g_textured", (code & 0x4) ? 1 : 0 );
    l_render_shader.setUniform1i( "g_abr", (l_tpage >> 0x5) & 0x3 );
    l_render_shader.setUniform1i( "g_dtd", 0 );
    mesh.draw();
    l_render_shader.end();
    glDisable( GL_SCISSOR_TEST );

    l_render.end();
}



void SPRT_16::execute()
{
    l_render.begin();

    static ofVboMesh mesh;
    mesh.clear();
    mesh.setMode( OF_PRIMITIVE_TRIANGLE_FAN );

    mesh.addVertex( glm::vec3(l_ofs_x + x0,      l_ofs_y + y0,      0) );
    mesh.addVertex( glm::vec3(l_ofs_x + x0 + 16, l_ofs_y + y0,      0) );
    mesh.addVertex( glm::vec3(l_ofs_x + x0 + 16, l_ofs_y + y0 + 16, 0) );
    mesh.addVertex( glm::vec3(l_ofs_x + x0,      l_ofs_y + y0 + 16, 0) );

    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );
    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );
    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );
    mesh.addColor( ofFloatColor(r0 / 255.0f, g0 / 255.0f, b0 / 255.0f, 1.0f) );

    mesh.addTexCoord( glm::vec2(u0,      v0     ) );
    mesh.addTexCoord( glm::vec2(u0 + 16, v0     ) );
    mesh.addTexCoord( glm::vec2(u0 + 16, v0 + 16) );
    mesh.addTexCoord( glm::vec2(u0,      v0 + 16) );

    glm::mat4 projection = glm::ortho( 0.0f, l_render.getWidth(), 0.0f, l_render.getHeight(), -1.0f, 1.0f );

    glEnable( GL_SCISSOR_TEST );
    l_render_shader.begin();
    l_render_shader.setUniformMatrix4f( "g_matrix", projection );
    l_render_shader.setUniformTexture( "g_texture", l_render_texture, 0 );
    l_render_shader.setUniformTexture( "g_background", l_render_inn, 1 );
    l_render_shader.setUniform2i( "g_clut", (clut & 0x3f) * 0x10, (clut & 0xffc0) >> 0x6 );
    l_render_shader.setUniform2i( "g_tpage", (l_tpage << 0x6) & 0x3ff, (l_tpage << 0x4) & 0x100 );
    l_render_shader.setUniform1i( "g_depth", (l_tpage >> 0x7) & 0x3 );
    l_render_shader.setUniform1i( "g_transp", (code & 0x2) ? 1 : 0 );
    l_render_shader.setUniform1i( "g_colored", (code & 0x1) ? 0 : 1 );
    l_render_shader.setUniform1i( "g_textured", (code & 0x4) ? 1 : 0 );
    l_render_shader.setUniform1i( "g_abr", (l_tpage >> 0x5) & 0x3 );
    l_render_shader.setUniform1i( "g_dtd", 0 );
    mesh.draw();
    l_render_shader.end();
    glDisable( GL_SCISSOR_TEST );

    l_render.end();
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
