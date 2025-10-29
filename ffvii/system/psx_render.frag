#version 130

uniform usampler2D g_texture;
uniform sampler2D g_background;
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

// Pas gamma correction
const float psx_gamma_lut[32] = float[32](
    0.000000,  // 0
    0.052063,  // 1
    0.103030,  // 2
    0.152893,  // 3
    0.201644,  // 4
    0.249277,  // 5
    0.295785,  // 6
    0.341163,  // 7
    0.385406,  // 8
    0.428509,  // 9
    0.470468,  // 10
    0.511280,  // 11
    0.550941,  // 12
    0.589449,  // 13
    0.626802,  // 14
    0.662998,  // 15
    0.698036,  // 16
    0.731915,  // 17
    0.764634,  // 18
    0.796194,  // 19
    0.826595,  // 20
    0.855838,  // 21
    0.883924,  // 22
    0.910855,  // 23
    0.936632,  // 24
    0.961258,  // 25
    0.984736,  // 26
    0.992368,  // 27
    0.996184,  // 28
    0.998092,  // 29
    0.999046,  // 30
    1.000000   // 31
);

out vec4 fragColor;



vec3 apply_gamma( vec3 color )
{
    color.x = psx_gamma_lut[int(color.x) >> 0x3];
    color.y = psx_gamma_lut[int(color.y) >> 0x3];
    color.z = psx_gamma_lut[int(color.z) >> 0x3];
    return color;
}



vec3 apply_vertex_color( vec3 color )
{
    vec3 vertex_color = vColor.xyz * 255.0;

    if( vertex_color.x <= 0x80 ) color.x *= vertex_color.x / 128.0;
    if( vertex_color.y <= 0x80 ) color.y *= vertex_color.y / 128.0;
    if( vertex_color.z <= 0x80 ) color.z *= vertex_color.z / 128.0;

    if( vertex_color.x > 0x80 ) color.x *= 128.0 / vertex_color.x;
    if( vertex_color.y > 0x80 ) color.y *= 128.0 / vertex_color.y;
    if( vertex_color.z > 0x80 ) color.z *= 128.0 / vertex_color.z;

    color = clamp( color, 0, 0xff );
    return color;
}



vec3 apply_dither( vec3 color )
{
    ivec2 frag_coord = ivec2( gl_FragCoord.xy );
    int dither_value = dither_matrix[(frag_coord.y % 4) * 4 + (frag_coord.x % 4)];
    color += dither_value;
    color = clamp( color, 0, 0xff );
    return color;
}



vec3 mix_with_bg( vec3 color )
{
    vec3 back_color = texelFetch( g_background, ivec2( gl_FragCoord.xy ), 0 ).xyz * 255.0;

    switch( g_abr )
    {
        case 0: // 0.5 x B + 0.5 x F
            color = back_color * 0.5 + color * 0.5;
            break;
        case 1: // 1.0 x B + 1.0 x F
            color = back_color + color;
            break;
        case 2: // 1.0 x B - 1.0 x F
            color = back_color - color;
            break;
        case 3: // 1.0 x B + 0.25 x F
            color = back_color + color * 0.25;
            break;
    }

    color = clamp( color, 0, 0xff );
    return color;
}



vec4 psx_color_to_rgba( int rgba )
{
    if( rgba == 0 ) discard;

    float r = ((rgba >> 0x0) & 0x1f) << 0x3;
    float g = ((rgba >> 0x5) & 0x1f) << 0x3;
    float b = ((rgba >> 0xa) & 0x1f) << 0x3;
    int stp_bit = (rgba >> 0xf) & 0x1;

    vec3 color = vec3( r, g, b );

    if( (g_transp == 1) && (stp_bit == 1) ) color = mix_with_bg( color );

    if( g_colored == 1 ) color = apply_vertex_color( color );

    if( g_dtd == 1 ) color = apply_dither( color );

    color = apply_gamma( color );

    return vec4(color, 1.0);
}



int get_color_16bit( ivec2 texel_coord )
{
    uint low = texelFetch( g_texture, texel_coord, 0 ).r;
    uint high = texelFetch( g_texture, texel_coord + ivec2(1, 0), 0 ).r;
    return int(low | (high << 8));
}



vec4 get_clut_color( int color_id )
{
    ivec2 g_clut_texel_coord = ivec2((g_clut.x + color_id) * 2, g_clut.y);
    int color = get_color_16bit( g_clut_texel_coord );
    return psx_color_to_rgba( color );
}



void main()
{
    vec4 final_color;

    if( g_textured == 1 )
    {
        ivec2 texel_coord = ivec2(vTexCoord);

        if( g_depth == 0 ) // 4-bit
        {
            ivec2 vram_coord = ivec2(g_tpage.x * 2, g_tpage.y) + ivec2(texel_coord.x / 2, texel_coord.y);
            uint texel = texelFetch( g_texture, vram_coord, 0 ).r;

            if( (texel_coord.x & 1) == 0 )
            {
                final_color = get_clut_color( int(texel) & 0xf );
            }
            else
            {
                final_color = get_clut_color( int(texel) >> 4 );
            }
        }
        else if( g_depth == 1 ) // 8-bit
        {
            ivec2 vram_coord = ivec2(g_tpage.x * 2, g_tpage.y) + ivec2(texel_coord.x, texel_coord.y);
            uint texel = texelFetch( g_texture, vram_coord, 0 ).r;
            final_color = get_clut_color( int(texel) );
        }
        else // 16-bit direct
        {
            ivec2 vramCoord = ivec2( g_tpage.x * 2, g_tpage.y ) + ivec2(texel_coord.x * 2, texel_coord.y);
            int color = get_color_16bit( vramCoord );
            final_color = psx_color_to_rgba( color );
        }
    }
    else
    {
        vec3 color = vColor.xyz * 255.0;
        if( g_transp == 1 ) color = mix_with_bg( color );
        if( g_dtd == 1 ) color = apply_dither( color );
        color = apply_gamma( color );
        final_color = vec4(color, 1.0);
    }

    //fragColor = vec4( g_dtd, 0, 0, 1 );
    //fragColor = vec4( g_colored, 0, 0, 1 );
    //fragColor = vec4( g_transp, 0, 0, 1 );
    fragColor = final_color;
}
