#include "psxgpu.h"
#include "libgpu.h"



#define VRAM_W 2048
#define VRAM_H 512

std::array<u8, VRAM_W * VRAM_H> g_vram;
ofTexture g_screen;
ofFbo g_render;
ofTexture l_render_texture;
ofShader l_render_shader;

// rendering settings
u32 l_rendering_dtd = 0;
u16 l_rendering_tpage = 0;
u32 g_rendering_draw_x = 0;
u32 g_rendering_draw_y = 0;
u32 g_rendering_disp_x = 0;
u32 g_rendering_disp_y = 0;



void GPUUpdateVram()
{
    // update vram from render buffer
    ofTexture& texture = g_render.getTexture();
    ofPixels pixels;
    texture.readToPixels(pixels);

    int w = pixels.getWidth();
    int h = pixels.getHeight();

    for( int y = 0; y < h; ++y )
    {
        for( int x = 0; x < w; ++x )
        {
            ofColor color = pixels.getColor( x, y );

            u16 r = (color.r >> 3) & 0x1f;
            u16 g = (color.g >> 3) & 0x1f;
            u16 b = (color.b >> 3) & 0x1f;

            u16 rgb = (b << 0xa) | (g << 0x5) | r;

            g_vram[((g_rendering_draw_y + y) * VRAM_W) + (g_rendering_draw_x + x) * 2 + 0] = rgb & 0xff;
            g_vram[((g_rendering_draw_y + y) * VRAM_W) + (g_rendering_draw_x + x) * 2 + 1] = rgb >> 0x8;
        }
    }

    // update draw tex from vram
    if( !g_screen.isAllocated() )
    {
        g_screen.allocate( VRAM_W / 2, VRAM_H, GL_RGBA8, false );
    }

    w = g_screen.getWidth();
    h = g_screen.getHeight();
    pixels.allocate( w, h, OF_PIXELS_RGBA );

    for( int y = 0; y < h; ++y )
    {
        for( int x = 0; x < w; ++x )
        {
            int vram_x = g_rendering_disp_x + x;
            int vram_y = g_rendering_disp_y + y;
            u16 color = (g_vram[vram_y * VRAM_W + vram_x * 2 + 1] << 0x8) | g_vram[vram_y * VRAM_W + vram_x * 2 + 0];

            u8 b = ((color >> 0xa) & 0x1f) << 3;
            u8 g = ((color >> 0x5) & 0x1f) << 3;
            u8 r = ((color >> 0x0) & 0x1f) << 3;

            pixels.setColor( x, y, ofColor(r, g, b, 255) );
        }
    }

    g_screen.loadData( pixels );

    // update vram texture for rendering
    if( !l_render_texture.isAllocated() )
    {
        l_render_shader.load( "../system/psx_render.vert", "../system/psx_render.frag" );

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
    }

    const u16* src = reinterpret_cast<const u16*>(g_vram.data());

    pixels.allocate( VRAM_W, VRAM_H, OF_IMAGE_GRAYSCALE );
    memcpy( pixels.getData(), g_vram.data(), g_vram.size());
    l_render_texture.loadData( pixels.getData(), VRAM_W, VRAM_H, GL_RED_INTEGER, GL_UNSIGNED_BYTE );
}



void OTag::execute()
{
    if( size == 0 ) return;

    u8 code = *((u8*)this + sizeof(OTag) + 0x3);

    if( (code & 0xfc) == 0x2c )
    {
        ((POLY_FT4*)this)->execute();
    }
    else if( (code & 0xfc) == 0x40 )
    {
        ((LINE_F2*)this)->execute();
    }
    else if( (code & 0xfc) == 0x64 )
    {
        ((SPRT*)this)->execute();
    }
    else if( (code == 0xe1) && (size == 0x2) )
    {
        ((DR_MODE*)this)->execute();
    }
    else
    {
        ofLog( OF_LOG_ERROR, "Unsupported OTag: 0x" + ofToHex( code ) );
    }
}



void LINE_F2::execute()
{
    g_render.begin();
    ofSetColor( r0, g0, b0, (code & 0x2) ? 0x3f : 0xff );
    ofSetLineWidth( 1 );
    ofDrawLine( glm::vec3( 0xa0 + x0, 0x78 + y0, 0 ), glm::vec3( 0xa0 + x1, 0x78 + y1, 0 ) );
    g_render.end();
}



void POLY_FT4::execute()
{
    g_render.begin();

    ofVboMesh mesh;

    std::vector<glm::vec3> vertices =
    {
        { x0, y0, 0 },
        { x1, y1, 0 },
        { x3, y3, 0 },
        { x2, y2, 0 }
    };

    std::vector<ofFloatColor> colors =
    {
        { r0 / 128.0f, g0 / 128.0f, b0 / 128.0f, 1.0f },
        { r0 / 128.0f, g0 / 128.0f, b0 / 128.0f, 1.0f },
        { r0 / 128.0f, g0 / 128.0f, b0 / 128.0f, 1.0f },
        { r0 / 128.0f, g0 / 128.0f, b0 / 128.0f, 1.0f }
    };

    std::vector<glm::vec3> normals =
    {
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 }
    };

    std::vector<glm::vec2> texCoords =
    {
        { u0, v0 },
        { u1, v1 },
        { u3, v3 },
        { u2, v2 }
    };

    mesh.clear();
    mesh.setMode( OF_PRIMITIVE_TRIANGLE_FAN );
    mesh.addVertices( vertices );
    mesh.addColors( colors );
    mesh.addNormals( normals );
    mesh.addTexCoords( texCoords );

    glm::mat4 projection = glm::ortho( 0.0f, g_render.getWidth(), 0.0f, g_render.getHeight(), -1.0f, 1.0f );

    l_render_shader.begin();
    l_render_shader.setUniformMatrix4f( "g_matrix", projection );
    l_render_shader.setUniformTexture( "g_texture", l_render_texture, 0 );
    l_render_shader.setUniformTexture( "g_background", g_render.getTexture(), 1 );
    l_render_shader.setUniform2i( "g_clut", (clut & 0x3f) * 0x10, (clut & 0xffc0) >> 0x6 );
    l_render_shader.setUniform2i( "g_tpage", (tpage << 0x6) & 0x3ff, (tpage << 0x4) & 0x100 );
    l_render_shader.setUniform1i( "g_depth", (tpage >> 0x7) & 0x3 );
    l_render_shader.setUniform1i( "g_transp",(code & 0x2) ? 1 : 0 );
    l_render_shader.setUniform1i( "g_abr", (tpage >> 0x5) & 0x3 );
    l_render_shader.setUniform1i( "g_dtd", l_rendering_dtd );
    mesh.draw();
    l_render_shader.end();

    g_render.end();
}



void SPRT::execute()
{
    g_render.begin();

    ofVboMesh mesh;

    std::vector<glm::vec3> vertices =
    {
        { x0,     y0,     0 },
        { x0 + w, y0,     0 },
        { x0 + w, y0 + h, 0 },
        { x0,     y0 + h, 0 }
    };

    if( code & 0x01 )
    {
        r0 = 0x80;
        g0 = 0x80;
        b0 = 0x80;
    }

    std::vector<ofFloatColor> colors =
    {
        { r0 / 128.0f, g0 / 128.0f, b0 / 128.0f, 1.0f },
        { r0 / 128.0f, g0 / 128.0f, b0 / 128.0f, 1.0f },
        { r0 / 128.0f, g0 / 128.0f, b0 / 128.0f, 1.0f },
        { r0 / 128.0f, g0 / 128.0f, b0 / 128.0f, 1.0f }
    };

    std::vector<glm::vec3> normals =
    {
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 }
    };

    std::vector<glm::vec2> texCoords =
    {
        { u0,     v0     },
        { u0 + w, v0     },
        { u0 + w, v0 + h },
        { u0,     v0 + h }
    };

    mesh.clear();
    mesh.setMode( OF_PRIMITIVE_TRIANGLE_FAN );
    mesh.addVertices( vertices );
    mesh.addColors( colors );
    mesh.addNormals( normals );
    mesh.addTexCoords( texCoords );

    glm::mat4 projection = glm::ortho( 0.0f, g_render.getWidth(), 0.0f, g_render.getHeight(), -1.0f, 1.0f );

    l_render_shader.begin();
    l_render_shader.setUniformMatrix4f( "g_matrix", projection );
    l_render_shader.setUniformTexture( "g_texture", l_render_texture, 0 );
    l_render_shader.setUniformTexture( "g_background", g_render.getTexture(), 1 );
    l_render_shader.setUniform2i( "g_clut", (clut & 0x3f) * 0x10, (clut & 0xffc0) >> 0x6 );
    l_render_shader.setUniform2i( "g_tpage", (l_rendering_tpage << 0x6) & 0x3ff, (l_rendering_tpage << 0x4) & 0x100 );
    l_render_shader.setUniform1i( "g_depth", (l_rendering_tpage >> 0x7) & 0x3 );
    l_render_shader.setUniform1i( "g_transp",(code & 0x2) ? 1 : 0 );
    l_render_shader.setUniform1i( "g_abr", (l_rendering_tpage >> 0x5) & 0x3 );
    l_render_shader.setUniform1i( "g_dtd", 0 );
    mesh.draw();
    l_render_shader.end();

    g_render.end();
}



void DR_MODE::execute()
{
    l_rendering_dtd = (code[0] & 0x200) ? 1 : 0;
    l_rendering_tpage = code[0] & 0x9ff;
}
