#include "libgpu.h"
#include "system/application.h"



std::array<u8, 2048 * 512> g_vram;



s32 PsyqVSync( s32 mode )
{
    GameRender();

    return 1;
}



DISPENV* PsyqSetDefDispEnv( DISPENV* env, s32 x, s32 y, s32 w, s32 h )
{
    (env->disp).x = x;
    (env->disp).y = y;
    (env->disp).w = w;
    (env->disp).h = h;
    (env->screen).x = 0;
    (env->screen).y = 0;
    (env->screen).w = 0;
    (env->screen).h = 0;
    env->isrgb24 = 0;
    env->isinter = 0;
    env->pad1 = 0;
    env->pad0 = 0;
    return env;
}



DRAWENV* PsyqSetDefDrawEnv( DRAWENV* env, s32 x, s32 y, s32 w, s32 h )
{
    (env->clip).x = x;
    (env->clip).y = y;
    (env->clip).w = w;
    (env->clip).h = h;
    env->ofs[ 0 ] = x;
    env->ofs[ 1 ] = y;
    (env->tw).x = 0;
    (env->tw).y = 0;
    (env->tw).w = 0;
    (env->tw).h = 0;
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



DISPENV copyDispEnv;
DRAWENV copyDrawEnv;

DISPENV* PsyqPutDispEnv( DISPENV* env )
{
    if( ( g_screen.getWidth() != (env->disp).w ) || ( g_screen.getHeight() != (env->disp).h ) )
    {
        g_screen.allocate( (env->disp).w, (env->disp).h, GL_RGBA );
    }

    copyDispEnv = *env;
    return env;
}



DRAWENV* PsyqPutDrawEnv( DRAWENV* env )
{
    copyDrawEnv = *env;
    return env;
}



void OTag::execute()
{
    sColorAndCode colorAndCode = *(sColorAndCode*)(((u8*)this) + sizeof(OTag));

    u8 code = colorAndCode.code;
    if( size == 0 )
    {
    }
    else if( ( code & 0xe0 ) == 0x20 ) //poly
    {
        switch( code & 0x3c )
        {
            //case 0x20: ((POLY_F3*)this)->execute(); break;
            //case 0x24: ((POLY_FT3*)this)->execute(); break;
            //case 0x28: ((POLY_F4*)this)->execute(); break;
            case 0x2c: ((POLY_FT4*)this)->execute(); break;
            //case 0x30: ((POLY_G3*)this)->execute(); break;
            //case 0x34: ((POLY_GT3*)this)->execute(); break;
            //case 0x38: ((POLY_G4*)this)->execute(); break;
            default: break;
        }
    }
    else if( ( code & 0xe0 ) == 0x40 ) // line
    {
        switch( code & 0x4c )
        {
            case 0x40: ((LINE_F2*)this)->execute(); break;
            //case 0x42: break;
            //case 0x48: ((LINE_F3*)this)->execute(); break;
            default: break;
        }
    }
    else if( ( code & 0xe0 ) == 0x60 ) // rect
    {
        switch( code & 0x7c )
        {
            //case 0x60: ((TILE*)this)->execute(); break;
            //case 0x64: ((SPRT*)this)->execute(); break;
            //case 0x74:
            //case 0x7c: ((SPRT_8*)this)->execute(); break;
            default: break;
        }
    }
    else
    {
        assert( code == 0xe1 );
        if( size == 1 )
        {
            //((DR_TPAGE*)this)->execute();
        }
        else
        {
            assert( size == 2 );
            //((DR_MODE*)this)->execute();
        }
    }
}



void
LINE_F2::execute()
{
    g_screen.begin();
    ofSetColor( r0, g0, b0, (code & 0x2) ? 0x3f : 0xff );
    ofSetLineWidth( 1 );
    ofDrawLine( glm::vec3( 0xa0 + x0y0.vx, 0x78 + x0y0.vy, 0 ), glm::vec3( 0xa0 + x1y1.vx, 0x78 + x1y1.vy, 0 ) );
    g_screen.end();
}



void
POLY_FT4::execute()
{
    g_screen.begin();
 
    static ofTexture texture;
    static ofShader psxShader;
    if( !texture.isAllocated() )
    {
        psxShader.load( "../system/psx_render.vert", "../system/psx_render.frag" );

        ofDisableArbTex();

        int textureWidth = 2048;
        int textureHeight = 512;

        GLuint texID;
        glGenTextures( 1, &texID );
        glBindTexture( GL_TEXTURE_2D, texID );
        
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

        glTexImage2D( GL_TEXTURE_2D, 0, GL_R8UI, textureWidth, textureHeight, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr );

        texture.setUseExternalTextureID( texID );
        
        texture.texData.width = textureWidth;
        texture.texData.height = textureHeight;
        texture.texData.tex_w = textureWidth;
        texture.texData.tex_h = textureHeight;
        texture.texData.glInternalFormat = GL_R8UI;
        texture.texData.textureTarget = GL_TEXTURE_2D;

        glBindTexture( GL_TEXTURE_2D, 0 );
    }

    const u16* src = reinterpret_cast<const u16*>(g_vram.data());

    ofPixels pixels;
    pixels.allocate( 2048, 512, OF_IMAGE_GRAYSCALE );
    memcpy( pixels.getData(), g_vram.data(), g_vram.size());
    texture.loadData( pixels.getData(), 2048, 512, GL_RED_INTEGER, GL_UNSIGNED_BYTE );

    ofVboMesh mesh;

    std::vector<glm::vec3> vertices =
    {
        {  0x6f, 0x54, 0 },
        { 0x119, 0x54, 0 },
        { 0x119, 0xea, 0 },
        {  0x6f, 0xea, 0 }
    };

    std::vector<ofFloatColor> colors =
    {
        { 0x60 / 256.0f, 0x60 / 256.0f, 0x60 / 256.0f, 1.0f },
        { 0x60 / 256.0f, 0x60 / 256.0f, 0x60 / 256.0f, 1.0f },
        { 0x60 / 256.0f, 0x60 / 256.0f, 0x60 / 256.0f, 1.0f },
        { 0x60 / 256.0f, 0x60 / 256.0f, 0x60 / 256.0f, 1.0f }
    };

    std::vector<glm::vec3> normals =
    {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    };

    std::vector<glm::vec2> texCoords =
    {
        {  0x0,  0x0 },
        { 0x96,  0x0 },
        { 0x96, 0x96 },
        {  0x0, 0x96 }
    };

    mesh.clear();
    mesh.setMode( OF_PRIMITIVE_TRIANGLE_FAN );
    mesh.addVertices( vertices );
    mesh.addColors( colors );
    mesh.addNormals( normals );
    mesh.addTexCoords( texCoords );

    glm::mat4 projection = glm::ortho(0.0f, 1024.0f, 0.0f, 512.0f, -1.0f, 1.0f);

    psxShader.begin();
    psxShader.setUniformMatrix4f("projectionMatrix", projection);
    psxShader.setUniformTexture( "texture0", texture, 0 );
    psxShader.setUniform2i( "clut", 0, 0x1e0 );
    psxShader.setUniform4i( "tpage", 0x380, 0, 1, 0 );
    mesh.draw();
    psxShader.end();

    g_screen.end();
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
