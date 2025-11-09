#include "libgpu.h"
#include "psxgpu.h"



std::vector<u8>::const_iterator l_current_tim;

void (*l_vsync_callback)() = nullptr;



int PsyqOpenTim(std::vector<u8>::const_iterator ptr)
{
    l_current_tim = ptr;
    return 0;
}



int GetTimData(std::vector<u8>::const_iterator input, TIM_IMAGE* timimg)
{
    if (READ_LE_U32(input) == 0x10)
    {
        std::vector<u8>::const_iterator timaddr = input + 0x8;
        timimg->mode = READ_LE_U32(input + 0x4);

        int clut_len = 0;

        if (timimg->mode & 0x8)
        {
            timimg->crect.x = READ_LE_S16(input + 0xc + 0x0);
            timimg->crect.y = READ_LE_S16(input + 0xc + 0x2);
            timimg->crect.w = READ_LE_S16(input + 0xc + 0x4);
            timimg->crect.h = READ_LE_S16(input + 0xc + 0x6);
            timimg->caddr = &(*(input + 0x14));
            clut_len = READ_LE_U32(timaddr) / 4;
            timaddr += READ_LE_U32(timaddr) & ~0x3;
        }
        else
        {
            timimg->caddr = nullptr;
        }

        timimg->prect.x = READ_LE_S16(timaddr + 0x4 + 0x0);
        timimg->prect.y = READ_LE_S16(timaddr + 0x4 + 0x2);
        timimg->prect.w = READ_LE_S16(timaddr + 0x4 + 0x4);
        timimg->prect.h = READ_LE_S16(timaddr + 0x4 + 0x6);
        timimg->paddr = &(*(timaddr + 0xc));
        return clut_len + (READ_LE_U32(timaddr) / 4) + 0x2;
    }
    else
    {
        return -1;
    }
}



TIM_IMAGE* PsyqReadTim(TIM_IMAGE* timimg)
{
    int tim_offset = GetTimData(l_current_tim, timimg);

    if (tim_offset != -1)
    {
        l_current_tim += tim_offset * 4;
        return timimg;
    }

    return nullptr;
}



void PsyqLoadImage(SRECT* rect, const u8* data)
{
    int num_pixels = rect->w * rect->h;
    std::vector<u16> temp_data_u16(num_pixels);

    const u8* src = data;
    for(int i = 0; i < num_pixels; ++i)
    {
        u8 low_byte = *(src++);
        u8 high_byte = *(src++);
        temp_data_u16[i] = (high_byte << 8) | low_byte;
    }

    g_vram_texture.bind();
    glTexSubImage2D(GL_TEXTURE_2D, 0, rect->x, rect->y, rect->w, rect->h, GL_RED_INTEGER, GL_UNSIGNED_SHORT, temp_data_u16.data());
    g_vram_texture.unbind();
}



void PsyqLoadImage(SRECT* rect, std::span<u8>::iterator data)
{
    int num_pixels = rect->w * rect->h;
    std::vector<u16> temp_data_u16(num_pixels);

    for(int i = 0; i < num_pixels; ++i)
    {
        u8 low_byte = READ_LE_U8(data + 0);
        u8 high_byte = READ_LE_U8(data + 1);
        temp_data_u16[i] = (static_cast<u16>(high_byte) << 8) | low_byte;
        data += 2;
    }

    g_vram_texture.bind();
    glTexSubImage2D(GL_TEXTURE_2D, 0, rect->x, rect->y, rect->w, rect->h, GL_RED_INTEGER, GL_UNSIGNED_SHORT, temp_data_u16.data());
    g_vram_texture.unbind();
}



u16 PsyqLoadTPage(const u8* data, int tp, int abr, int x, int y, int w, int h)
{
    SRECT rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    if (tp == 0) rect.w = w / 0x4;
    else if (tp == 0x1) rect.w = w / 0x2;

    PsyqLoadImage(&rect, data);

    return PsyqGetTPage(tp, abr, x, y);
}



void PsyqClearImage(SRECT* rect, u8 r, u8 g, u8 b)
{
    u16 clear_value = 0;
    clear_value |= ((r >> 0x3) & 0x1f) << 0x0;
    clear_value |= ((g >> 0x3) & 0x1f) << 0x5;
    clear_value |= ((b >> 0x3) & 0x1f) << 0xa;
    GLuint textureID = g_vram_texture.getTextureData().textureID;
    glClearTexSubImage(textureID, 0, rect->x, rect->y, 0, rect->w, rect->h, 1, GL_RED_INTEGER, GL_UNSIGNED_SHORT, &clear_value);
}



s32 PsyqVSync(s32 mode)
{
    GameRender();

    l_vsync_callback();

    return 1;
}



void PsyqVsyncCallback(void (*func)())
{
    l_vsync_callback = func;
}



DISPENV* PsyqSetDefDispEnv(DISPENV* env, s32 x, s32 y, s32 w, s32 h)
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



DRAWENV* PsyqSetDefDrawEnv(DRAWENV* env, s32 x, s32 y, s32 w, s32 h)
{
    env->clip.x = x;
    env->clip.y = y;
    env->clip.w = w;
    env->clip.h = h;
    env->ofs[0] = x;
    env->ofs[1] = y;
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



void PsyqSetDrawEnv(DR_ENV* dr_env, DRAWENV* env)
{
    dr_env->type = GPU_DR_ENV;
    dr_env->env = *env;
}



DISPENV* PsyqPutDispEnv(DISPENV* env)
{
    g_rendering_disp_x = env->disp.x;
    g_rendering_disp_y = env->disp.y;
    g_rendering_disp_w = env->disp.w;
    g_rendering_disp_h = env->disp.h;
    return env;
}



DRAWENV* PsyqPutDrawEnv(DRAWENV* env)
{
    DR_ENV dr_env;
    PsyqSetDrawEnv(&dr_env, env);
    dr_env.execute();
    return env;
}



void PsyqSetDrawMode(DR_MODE* p, int dfe, int dtd, int tpage, SRECT* tw)
{
    p->type = GPU_DR_MODE;
    p->dfe = dfe;
    p->dtd = dtd;
    p->tpage = tpage;
    if (tw != nullptr) p->tw = *tw;
}



void PsyqSetDispMask(int mask)
{
    g_rendering_disp_enable = (mask != 0) ? 0x1 : 0;
}



OTag* PsyqClearOTagR(OTag* ot, s32 n)
{
    OTag* current = ot;
    for(int i = 0; i < n - 1; ++i)
    {
        ++current;
        current->next = current - 1;
        current->type = GPU_OTAG;
    }

    PsyqTermPrim(ot);

    return ot;
}



OTag* PsyqClearOTag(OTag* ot, s32 n)
{
    OTag* current = ot;
    for(int i = n - 1; i != 0; --i)
    {
        ++current;
        (current - 1)->next = current;
        (current - 1)->type = GPU_OTAG;
    }

    PsyqTermPrim(ot);

    return ot;
}



void PsyqDrawOTag(OTag* ot)
{
    glBindFramebuffer(GL_FRAMEBUFFER, g_fbo_id);
    glViewport(0, 0, VRAM_W, VRAM_H);
    glDisable(GL_BLEND);
    glEnable(GL_SCISSOR_TEST);

    g_render_shader.begin();

    while(ot)
    {
        ot->execute();
        ot = ot->next;
    }

    g_render_shader.end();

    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void PsyqSetLineF2(LINE_F2* p)
{
    p->type = GPU_LINE_F2;
    p->code = 0x40;
}



void PsyqSetPolyFT4(POLY_FT4* p)
{
    p->type = GPU_POLY_FT4;
    p->code = 0x2c;
}



void PsyqSetTile(TILE* p)
{
    p->type = GPU_TILE;
    p->code = 0x7c;
}



void PsyqSetSprt(SPRT* p)
{
    p->type = GPU_SPRT;
    p->code = 0x64;
}



void PsyqSetSprt16(SPRT_16* p)
{
    p->type = GPU_SPRT_16;
    p->code = 0x7c;
}



void PsyqAddPrim(OTag* ot, OTag* p)
{
    p->next = ot->next;
    ot->next = p;
}



void PsyqTermPrim(OTag* p)
{
    p->next = nullptr;
    p->type = GPU_TERMINATE;
}



u16 PsyqGetClut(s32 x, s32 y)
{
    return (y << 0x6) | ((x >> 0x4) & 0x3f);
}



u16 PsyqGetTPage(int tp, int abr, int x, int y)
{
    return ((y & 0x200) << 0x2) | ((tp & 0x3) << 0x7) | ((abr & 0x3) << 0x5) | ((y & 0x100) >> 0x4) | ((x & 0x3ff) >> 0x6);
}
