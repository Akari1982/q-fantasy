#include "gpuprims.h"
#include "../game.h"



DR_MODE gCurrentDrawMode;

void sTag::execute()
{
    sColorAndCode colorAndCode = *(sColorAndCode*)(((u8*)this) + sizeof(sTag));

    u8 code = colorAndCode.m3_code;
    if( m3_size == 0 )
    {

    }
    else if( ( code & 0xe0 ) == 0x20 )
    {
        //poly
        switch( code & 0x3c )
        {
            case 0x20: ((POLY_F3*)this)->execute(); break;
            case 0x24: ((POLY_FT3*)this)->execute(); break;
            case 0x28: ((POLY_F4*)this)->execute(); break;
            case 0x2C: ((POLY_FT4*)this)->execute(); break;
            case 0x30: ((POLY_G3*)this)->execute(); break;
            case 0x34: ((POLY_GT3*)this)->execute(); break;
            case 0x38: ((POLY_G4*)this)->execute(); break;
            default: break;
        }
    }
    else if( ( code & 0xe0 ) == 0x40 )
    {
        // line
        switch( code & 0x4c )
        {
            case 0x40: ((LINE_F2*)this)->execute(); break;
            case 0x42: break;
            case 0x48: ((LINE_F3*)this)->execute(); break;

            default: break;
        }
    }
    else if( ( code & 0xe0 ) == 0x60 )
    {
        // quad
        switch( code & 0x7c )
        {
            case 0x60: ((TILE*)this)->execute(); break;
            case 0x64: ((SPRT*)this)->execute(); break;
            case 0x74:
            case 0x7c: ((SPRT_8*)this)->execute(); break;
            default: break;
        }
    }
    else
    {
        assert( code == 0xE1 );
        if( m3_size == 1 )
        {
            ((DR_TPAGE*)this)->execute();
        }
        else
        {
            assert( m3_size == 2 );
            ((DR_MODE*)this)->execute();
        }
    }
}



void LINE_F2::execute()
{
    g_GameVram.begin();
    ofSetColor( 255,255,255, 144 );
    ofDrawLine( glm::vec3( x0y0.vx, x0y0.vy, 0 ), glm::vec3( x1y1.vx, x1y1.vy, 0 ) );
    //ofDrawRectangle( 0, 0, 400, 400 );
    g_GameVram.end();

/*
    float matrix[16];
    bx::mtxIdentity(matrix);

    bgfx::setTransform(matrix);

    bgfx::TransientVertexBuffer vertexBuffer;
    bgfx::TransientIndexBuffer indexBuffer;
    bgfx::allocTransientBuffers(&vertexBuffer, GetLayout(), 3, &indexBuffer, 3);

    sVertice* pVertices = (sVertice*)vertexBuffer.data;
    u16* pIndices = (u16*)indexBuffer.data;

    for (int i = 0; i < 3; i++)
    {
        pVertices[i].color[0] = r0;
        pVertices[i].color[1] = g0;
        pVertices[i].color[2] = b0;
        pVertices[i].gpuCode = code;
    }

    pVertices[0].v[0] = x0y0.vx;
    pVertices[0].v[1] = x0y0.vy;
    pVertices[0].v[2] = 0;

    pVertices[1].v[0] = x1y1.vx;
    pVertices[1].v[1] = x1y1.vy;
    pVertices[1].v[2] = 0;

    pVertices[2].v[0] = x2y2.vx;
    pVertices[2].v[1] = x2y2.vy;
    pVertices[2].v[2] = 0;

    pIndices[0] = 0;
    pIndices[1] = 1;
    pIndices[2] = 2;

    u64 State = BGFX_STATE_WRITE_RGB
        | BGFX_STATE_DEPTH_TEST_ALWAYS
        | BGFX_STATE_MSAA
        | BGFX_STATE_PT_LINESTRIP;

    u32 blendRGBA = getBlending(code, gCurrentDrawMode.code[0], State);

    bgfx::setState(State, blendRGBA);

    bgfx::setVertexBuffer(0, &vertexBuffer);
    bgfx::setIndexBuffer(&indexBuffer);

    static bgfx::UniformHandle s_PSXVramUniformHandle = BGFX_INVALID_HANDLE;
    if (!bgfx::isValid(s_PSXVramUniformHandle))
    {
        s_PSXVramUniformHandle = bgfx::createUniform("s_PSXVram", bgfx::UniformType::Sampler);
    }
    bgfx::setTexture(0, s_PSXVramUniformHandle, m_vramTextureHandle);
    bgfx::submit(PSXOutput_bgfxView, getSPRTShader());
*/
}



void TermPrim( sTag* p )
{
    p->m0_pNext = nullptr;
    p->m3_size = 0;
}

void SetPolyFT4( POLY_FT4* p )
{
    p->m3_size = 9;
    p->code = 0x2C;
}

void SetPolyF3( POLY_F3* p )
{
    p->m3_size = 4;
    p->code = 0x20;
}

void SetPolyFT3( POLY_FT3* p )
{
    p->m3_size = 7;
    p->code = 0x24;
}

void SetTile( TILE* p )
{
    p->m3_size = 3;
    p->code = 0x60;
}

void SetDrawMove( DR_MOVE* p, PSX_RECT* rect, int x, int y )
{
}

void SetShadeTex( POLY_FT4* p, int tge )
{
    if (tge == 0) {
        p->code &= ~1;
    }
    else {
        p->code |= 1;
    }
}

void SetPolyG3( POLY_G3* p )
{
    p->m3_size = 6;
    p->code = 0x30;
}

void SetPolyG4( POLY_G4* p )
{
    p->m3_size = 8;
    p->code = 0x38;
}

void SetPolyGT4( POLY_GT4* p )
{
    p->m3_size = 0xC;
    p->code = 0x3C;
}

void SetSprt( SPRT* p )
{
    p->m3_size = 4;
    p->code = 0x64;
}

void SetLineF2( LINE_F2* p )
{
    p->m3_size = 3;
    p->code = 0x40;
}

void SetLineF3( LINE_F3* p )
{
    p->m3_size = 5;
    p->code = 0x48;
    p->pad = 0x55555555;
}

void SetPolyF4( POLY_F4* p )
{
    p->m3_size = 5;
    p->code = 0x28;
}

void setPolyUV( POLY_FT4* poly, u16 u0, u16 v0, u16 u1, u16 v1, u16 u2, u16 v2, u16 u3, u16 v3 )
{
    if ((int)((u32)u0 << 0x10) < 0) {
        u0 = 0;
    }
    if ((int)((u32)u1 << 0x10) < 0) {
        u1 = 0;
    }
    if ((int)((u32)u2 << 0x10) < 0) {
        u2 = 0;
    }
    if ((int)((u32)u3 << 0x10) < 0) {
        u3 = 0;
    }
    if ((int)((u32)v0 << 0x10) < 0) {
        v0 = 0;
    }
    if ((int)((u32)v1 << 0x10) < 0) {
        v1 = 0;
    }
    if ((int)((u32)v2 << 0x10) < 0) {
        v2 = 0;
    }
    if ((int)((u32)v3 << 0x10) < 0) {
        v3 = 0;
    }
    if (0xff < (s16)u0) {
        u0 = 0xff;
    }
    if (0xff < (s16)u1) {
        u1 = 0xff;
    }
    if (0xff < (s16)u2) {
        u2 = 0xff;
    }
    if (0xff < (s16)u3) {
        u3 = 0xff;
    }
    if (0xff < (s16)v0) {
        v0 = 0xff;
    }
    if (0xff < (s16)v1) {
        v1 = 0xff;
    }
    if (0xff < (s16)v2) {
        v2 = 0xff;
    }
    if (0xff < (s16)v3) {
        v3 = 0xff;
    }
    poly->u0 = u0;
    poly->v0 = v0;
    poly->u1 = u1;
    poly->v1 = v1;
    poly->u2 = u2;
    poly->v2 = v2;
    poly->u3 = u3;
    poly->v3 = v3;
}

void SetDrawTPage(DR_TPAGE* p, int dfe, int dtd, int tpage) {
    u32 uVar1;
    u32 uVar2;

    p->m3_size = 1;
    uVar2 = 0xe1000000;
    if (dtd != 0) {
        uVar2 = 0xe1000200;
    }
    uVar1 = tpage & 0x9ff;
    if (dfe != 0) {
        uVar1 |= 0x400;
    }
    p->code[0] = uVar2 | uVar1;
}
