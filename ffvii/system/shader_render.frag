#version 130

uniform usampler2D g_texture;

uniform ivec2 g_tpage;
uniform ivec2 g_clut;
uniform int g_depth;
uniform int g_transp;
uniform int g_colored;
uniform int g_textured;
uniform int g_abr;
uniform int g_dtd;

in vec4 vColor;
in vec2 vTexCoord;

const int dither_matrix[16] = int[]
(
    -4,  0, -3,  1,
     2, -2,  3, -1,
    -3,  1, -4,  0,
     3, -1,  2, -2
);

out uint fragColor;



vec3 apply_vertex_color(vec3 color)
{
    vec3 vertex_color = vColor.xyz * 255.0;

    if (vertex_color.x <= 0x80) color.x *= vertex_color.x / 128.0;
    if (vertex_color.y <= 0x80) color.y *= vertex_color.y / 128.0;
    if (vertex_color.z <= 0x80) color.z *= vertex_color.z / 128.0;

    if (vertex_color.x > 0x80) color.x *= 128.0 / vertex_color.x;
    if (vertex_color.y > 0x80) color.y *= 128.0 / vertex_color.y;
    if (vertex_color.z > 0x80) color.z *= 128.0 / vertex_color.z;

    return clamp(color, 0, 0xff);
}



vec3 apply_dither(vec3 color)
{
    ivec2 frag_coord = ivec2(gl_FragCoord.xy);

    color += dither_matrix[(frag_coord.y % 4) * 4 + (frag_coord.x % 4)];

    return clamp(color, 0, 0xff);
}



vec3 mix_with_bg(vec3 front_color)
{
    int rgba = int(texelFetch(g_texture, ivec2(gl_FragCoord.xy), 0).r);

    // unpack color
    int r = ((rgba >> 0x0) & 0x1f) << 0x3;
    int g = ((rgba >> 0x5) & 0x1f) << 0x3;
    int b = ((rgba >> 0xa) & 0x1f) << 0x3;
    vec3 back_color = vec3(float(r), float(g), float(b));

    switch (g_abr)
    {
        case 0: // 0.5 x B + 0.5 x F
            front_color = back_color * 0.5 + front_color * 0.5;
            break;
        case 1: // 1.0 x B + 1.0 x F
            front_color = back_color + front_color;
            break;
        case 2: // 1.0 x B - 1.0 x F
            front_color = back_color - front_color;
            break;
        case 3: // 1.0 x B + 0.25 x F
            front_color = back_color + front_color * 0.25;
            break;
    }

    return clamp(front_color, 0, 0xff);
}



int color_apply(int rgba)
{
    if (rgba == 0) discard;

    // unpack color
    float r = ((rgba >> 0x0) & 0x1f) << 0x3;
    float g = ((rgba >> 0x5) & 0x1f) << 0x3;
    float b = ((rgba >> 0xa) & 0x1f) << 0x3;
    int stp_bit = (rgba >> 0xf) & 0x1;

    vec3 color = ivec3(r, g, b);

    if (g_colored == 1) color = apply_vertex_color(color);

    if ((g_transp == 1) && (stp_bit == 1)) color = mix_with_bg(color);

    if (g_dtd == 1) color = apply_dither(color);

    // pack color
    int final_color = 0;
    final_color |= ((int(color.r) >> 0x3) & 0x1f) << 0x0;
    final_color |= ((int(color.g) >> 0x3) & 0x1f) << 0x5;
    final_color |= ((int(color.b) >> 0x3) & 0x1f) << 0xa;
    final_color |= (stp_bit == 1) ? 0x8000 : 0;

    return final_color;
}



int get_clut_color(int color_id)
{
    ivec2 g_clut_texel_coord = ivec2(g_clut.x + color_id, g_clut.y);
    return int(texelFetch(g_texture, g_clut_texel_coord, 0).r);
}



void main()
{
    int final_color = 0;

    if (g_textured == 1)
    {
        ivec2 texel_coord = ivec2(vTexCoord);

        if (g_depth == 0) // 4-bit
        {
            ivec2 vram_coord = ivec2(g_tpage.x, g_tpage.y) + ivec2(texel_coord.x / 4, texel_coord.y);
            uint texel;
            if ((texel_coord.x & 2) == 0)
            {
                texel = texelFetch(g_texture, vram_coord, 0).r & uint(0xff);
            }
            else
            {
                texel = texelFetch(g_texture, vram_coord, 0).r >> uint(8);
            }
            if ((texel_coord.x & 1) == 0)
            {
                final_color = get_clut_color(int(texel) & 0xf);
            }
            else
            {
                final_color = get_clut_color(int(texel) >> 4);
            }
            final_color = color_apply(final_color);
        }
        else if (g_depth == 1) // 8-bit
        {
            ivec2 vram_coord = ivec2(g_tpage.x, g_tpage.y) + ivec2(texel_coord.x / 2, texel_coord.y);
            uint texel;
            if ((texel_coord.x & 1) == 0)
            {
                texel = texelFetch(g_texture, vram_coord, 0).r & uint(0xff);
            }
            else
            {
                texel = texelFetch(g_texture, vram_coord, 0).r >> uint(8);
            }
            final_color = get_clut_color(int(texel));
            final_color = color_apply(final_color);
        }
        else // 16-bit direct
        {
            ivec2 vramCoord = ivec2(g_tpage.x, g_tpage.y) + ivec2(texel_coord.x, texel_coord.y);
            int color = int(texelFetch(g_texture, vramCoord, 0).r);
            final_color = color_apply(color);
        }
    }
    else
    {
        vec3 color = vColor.xyz * 255.0;
        if (g_transp == 1) color = mix_with_bg(color);
        if (g_dtd == 1) color = apply_dither(color);

        // pack color
        final_color |= ((int(color.r) >> 0x3) & 0x1f) << 0x0;
        final_color |= ((int(color.g) >> 0x3) & 0x1f) << 0x5;
        final_color |= ((int(color.b) >> 0x3) & 0x1f) << 0xa;
    }

    fragColor = uint(final_color);
}
