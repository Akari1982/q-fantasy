#include "libgpu.h"
#include "system/application.h"

#define VRAM_W 2048
#define VRAM_H 512

std::array<u8, VRAM_W * VRAM_H> g_vram;
ofFbo l_render;
ofTexture l_render_texture;
ofShader l_render_shader;

// rendering settings
u32 l_rendering_dtd = 0;
u16 l_rendering_tpage = 0;
u32 l_rendering_draw_x = 0;
u32 l_rendering_draw_y = 0;
u32 l_rendering_disp_x = 0;
u32 l_rendering_disp_y = 0;



void update_vram()
{
    // update vram from render buffer
    ofTexture& texture = l_render.getTexture();
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

            g_vram[((l_rendering_draw_y + y) * VRAM_W) + (l_rendering_draw_x + x) * 2 + 0] = rgb & 0xff;
            g_vram[((l_rendering_draw_y + y) * VRAM_W) + (l_rendering_draw_x + x) * 2 + 1] = rgb >> 0x8;
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
            int vram_x = l_rendering_disp_x + x;
            int vram_y = l_rendering_disp_y + y;
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



s32 PsyqVSync( s32 mode )
{
    GameRender();

    update_vram();

    return 1;
}



DISPENV* PsyqSetDefDispEnv( DISPENV* env, s32 x, s32 y, s32 w, s32 h )
{
    env->disp.x = x;
    env->disp.y = y;
    env->disp.w = w;
    env->disp.h = h;
    env->screen.x = 0;
    env->screen.y = 0;
    env->screen.w = 0;
    env->screen.h = 0;
    env->isrgb24 = 0;
    env->isinter = 0;
    env->pad1 = 0;
    env->pad0 = 0;
    return env;
}



DRAWENV* PsyqSetDefDrawEnv( DRAWENV* env, s32 x, s32 y, s32 w, s32 h )
{
    env->clip.x = x;
    env->clip.y = y;
    env->clip.w = w;
    env->clip.h = h;
    env->ofs[ 0 ] = x;
    env->ofs[ 1 ] = y;
    env->tw.x = 0;
    env->tw.y = 0;
    env->tw.w = 0;
    env->tw.h = 0;
    env->tpage = 0xa;
    env->dtd = 1;
    env->dfe = 0;
    env->isbg = 0;
    env->r0 = 0;
    env->g0 = 0;
    env->b0 = 0;
    return env;
}



void PsyqSetDrawEnv( DR_ENV* dr_env, DRAWENV* env )
{
/*
    A0 = h[env + 0]; // x top clip
    A1 = h[env + 2]; // y top clip
    system_gpu_set_drawing_area_top_left(); // create packet for clip
    [dr_env + 4] = w(V0);

    A0 = h[env + 0] + h[env + 4] - 1;
    A1 = h[env + 2] + h[env + 6] - 1;
    system_gpu_set_drawing_area_bottom_right(); // create packet for сlip
    [dr_env + 8] = w(V0);

    A0 = h[env + 8]; // offset x
    A1 = h[env + a]; // offset y
    system_gpu_set_drawing_offset(); // create packet for offset
    [dr_env + c] = w(V0);

    A0 = bu[env + 17]; // 0: drawing to display area is blocked, 1: drawing to display area is permitted
    A1 = bu[env + 16]; // dithering processing flag. 0: off; 1: on
    A2 = hu[env + 14]; // initial values of texture page
    system_gpu_get_draw_mode_setting_command(); // create packet
    [dr_env + 10] = w(V0);

    A0 = env + c; // texture window rect
    system_gpu_get_texture_window_setting_command(); // create packet
    [dr_env + 14] = w(V0);

    [dr_env + 18] = w(e6000000);

    [dr_env + 3] = b(6); // number of 0x4 packets to gpu

    if( bu[env + 18] != 0 )
    {
        rect_x = hu[env + 0];
        rect_y = hu[env + 2];
        rect_w = hu[env + 4];
        rect_h = hu[env + 6];

        m_width = h[80062с04] - 1;
        m_height = h[80062c06] - 1;

        rect_w = ( rect_w >= 0 ) ? ( ( m_width < rect_w ) ? m_width : rect_w ) : 0;
        rect_h = ( rect_h >= 0 ) ? ( ( m_height < rect_h ) ? m_height : rect_h ) : 0;

        if( ( rect_x & 3f ) || ( rect_w & 3f ) )
        {
            rect_x = rect_x - hu[env + 8];
            rect_y = rect_y - hu[env + a];

            [dr_env + 7 * 4] = w(60000000 | (bu[env + 1b] << 10) | (bu[env + 1a] << 8) | bu[env + 19]);
            [dr_env + 8 * 4] = w((rect_y << 10) | rect_x);
            [dr_env + 9 * 4] = w((rect_h << 10) | rect_w);
        }
        else
        {
            [dr_env + 7 * 4] = w(02000000 | (bu[env + 1b] << 10) | (bu[env + 1a] << 08); | bu[env + 19]);
            [dr_env + 8 * 4] = w((rect_y << 10) | rect_x);
            [dr_env + 9 * 4] = w((rect_h << 10) | rect_w);
        }
        [dr_env + 3] = b(9);
    }
*/
}



DISPENV* PsyqPutDispEnv( DISPENV* env )
{
    if( (g_screen.getWidth() != env->disp.w) || (g_screen.getHeight() != env->disp.h) )
    {
        g_screen.allocate( env->disp.w, env->disp.h, GL_RGBA8, false );
    }

    l_rendering_disp_x = env->disp.x;
    l_rendering_disp_y = env->disp.y;

    return env;
}



DRAWENV* PsyqPutDrawEnv( DRAWENV* env )
{
    if( (l_render.getWidth() != env->clip.w) || (l_render.getHeight() != env->clip.h) )
    {
        l_render.allocate( env->clip.w, env->clip.h, GL_RGBA );
    }

    l_rendering_draw_x = env->clip.x;
    l_rendering_draw_y = env->clip.y;

    l_render.begin();
    ofClear( env->r0, env->g0, env->b0, 255 );
    l_render.end();

    return env;
}



u32 get_mode( int dfe, int dtd, int tpage )
{
    u32 add_dtd = 0;
    u32 add_dfe = 0;
    if( dtd != 0 ) add_dtd = 0x200; // Dither 24bit to 15bit Dither Enabled
    if( dfe != 0 ) add_dfe = 0x400; // Drawing to display area Allowed
    return 0xe1000000 | add_dtd | add_dfe | (tpage & 0x9ff);
}



u32 get_tw( SRECT* tw )
{
    if( tw == nullptr ) return 0;
    u32 off_x = tw->x >> 0x3;
    u32 off_y = tw->y >> 0x3;
    u32 mask_x = ((0 - tw->w) & 0xff) >> 0x3;
    u32 mask_y = ((0 - tw->h) & 0xff) >> 0x3;
    return 0xe2000000 | (off_y << 0xf) | (off_x << 0xa) | (mask_y << 0x5) | mask_x;
}



void PsyqSetDrawMode( DR_MODE* p, int dfe, int dtd, int tpage, SRECT* tw )
{
    p->size = 0x2;
    p->code[0] = get_mode( dfe, dtd, tpage );
    p->code[1] = get_tw( tw );
}



OTag* PsyqClearOTagR( OTag* ot, s32 n )
{
    OTag* current = ot;
    for( int i = 0; i < n - 1; ++i )
    {
        ++current;
        current->next = current - 1;
        current->size = 0;
    }

    PsyqTermPrim( ot );

    return ot;
}



OTag* PsyqClearOTag( OTag* ot, s32 n )
{
    OTag* current = ot;
    for( int i = n - 1; i != 0; --i )
    {
        ++current;
        (current - 1)->next = current;
        (current - 1)->size = 0;
    }

    PsyqTermPrim( ot );

    return ot;
}



void PsyqDrawOTag( OTag* ot )
{
    while( ot )
    {
        ot->execute();
        ot = ot->next;
    }
}



void PsyqSetLineF2( LINE_F2* p )
{
    p->size = 0x3;
    p->code = 0x40;
}



void PsyqSetPolyFT4( POLY_FT4* p )
{
    p->size = 0x9;
    p->code = 0x2c;
}




void PsyqSetSprt( SPRT* p )
{
    p->size = 0x4;
    p->code = 0x64;
}



void PsyqAddPrim( OTag* ot, OTag* p )
{
    p->next = ot->next;
    ot->next = p;
}



void PsyqTermPrim( OTag* p )
{
    p->next = nullptr;
    p->size = 0;
}



u16 PsyqGetClut( s32 x, s32 y )
{
    return (y << 0x6) | ((x >> 0x4) & 0x3f);
}



u16 PsyqGetTPage( int tp, int abr, int x, int y )
{
    return ((y & 0x200) << 0x2) | ((tp & 0x3) << 0x7) | ((abr & 0x3) << 0x5) | ((y & 0x100) >> 0x4) | ((x & 0x3ff) >> 0x6);
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
    l_render.begin();
    ofSetColor( r0, g0, b0, (code & 0x2) ? 0x3f : 0xff );
    ofSetLineWidth( 1 );
    ofDrawLine( glm::vec3( 0xa0 + x0, 0x78 + y0, 0 ), glm::vec3( 0xa0 + x1, 0x78 + y1, 0 ) );
    l_render.end();
}



void POLY_FT4::execute()
{
    l_render.begin();

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

    glm::mat4 projection = glm::ortho( 0.0f, l_render.getWidth(), 0.0f, l_render.getHeight(), -1.0f, 1.0f );

    l_render_shader.begin();
    l_render_shader.setUniformMatrix4f( "g_matrix", projection );
    l_render_shader.setUniformTexture( "g_texture", l_render_texture, 0 );
    l_render_shader.setUniformTexture( "g_background", l_render.getTexture(), 1 );
    l_render_shader.setUniform2i( "g_clut", (clut & 0x3f) * 0x10, (clut & 0xffc0) >> 0x6 );
    l_render_shader.setUniform2i( "g_tpage", (tpage << 0x6) & 0x3ff, (tpage << 0x4) & 0x100 );
    l_render_shader.setUniform1i( "g_depth", (tpage >> 0x7) & 0x3 );
    l_render_shader.setUniform1i( "g_transp",(code & 0x2) ? 1 : 0 );
    l_render_shader.setUniform1i( "g_abr", (tpage >> 0x5) & 0x3 );
    l_render_shader.setUniform1i( "g_dtd", l_rendering_dtd );
    mesh.draw();
    l_render_shader.end();

    l_render.end();
}



void SPRT::execute()
{
    l_render.begin();

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

    glm::mat4 projection = glm::ortho( 0.0f, l_render.getWidth(), 0.0f, l_render.getHeight(), -1.0f, 1.0f );

    l_render_shader.begin();
    l_render_shader.setUniformMatrix4f( "g_matrix", projection );
    l_render_shader.setUniformTexture( "g_texture", l_render_texture, 0 );
    l_render_shader.setUniformTexture( "g_background", l_render.getTexture(), 1 );
    l_render_shader.setUniform2i( "g_clut", (clut & 0x3f) * 0x10, (clut & 0xffc0) >> 0x6 );
    l_render_shader.setUniform2i( "g_tpage", (l_rendering_tpage << 0x6) & 0x3ff, (l_rendering_tpage << 0x4) & 0x100 );
    l_render_shader.setUniform1i( "g_depth", (l_rendering_tpage >> 0x7) & 0x3 );
    l_render_shader.setUniform1i( "g_transp",(code & 0x2) ? 1 : 0 );
    l_render_shader.setUniform1i( "g_abr", (l_rendering_tpage >> 0x5) & 0x3 );
    l_render_shader.setUniform1i( "g_dtd", 0 );
    mesh.draw();
    l_render_shader.end();

    l_render.end();
}



void DR_MODE::execute()
{
    l_rendering_dtd = (code[0] & 0x200) ? 1 : 0;
    l_rendering_tpage = code[0] & 0x9ff;
}
