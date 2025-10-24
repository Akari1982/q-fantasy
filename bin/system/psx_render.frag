#version 130

uniform usampler2D g_texture;
uniform sampler2D g_background;
uniform ivec2 g_tpage;
uniform ivec2 g_clut;
uniform int g_depth;
uniform int g_transp;
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



vec4 psx_color_to_rgba( int color )
{
    ivec2 frag_coord = ivec2( gl_FragCoord.xy );

    int r = ((color >> 0x0) & 0x1f) << 0x3;
    int g = ((color >> 0x5) & 0x1f) << 0x3;
    int b = ((color >> 0xa) & 0x1f) << 0x3;
    int stp_bit = (color >> 0xf) & 0x1;

    ivec3 full_color = ivec3( r, g, b );

    int dither_value = dither_matrix[(frag_coord.y % 4) * 4 + (frag_coord.x % 4)];

    if( g_dtd == 1 )
    {
        full_color += dither_value;
        full_color = clamp( full_color, 0, 0xff );
    }

    if( color == 0 )
    {
        discard;
    }

    vec3 final_color = vec3(
        psx_gamma_lut[full_color.x >> 0x3],
        psx_gamma_lut[full_color.y >> 0x3],
        psx_gamma_lut[full_color.z >> 0x3]
    );
    final_color *= vColor.xyz;

    if( (g_transp == 1) && (stp_bit == 1) )
    {
        vec3 back_color = texelFetch( g_background, frag_coord, 0 ).xyz;

        switch( g_abr )
        {
            case 0: // 0.5 x B + 0.5 x F
                final_color = mix(back_color, final_color, 0.5);
                break;
            case 1: // 1.0 x B + 1.0 x F
                final_color = back_color + final_color;
                break;
            case 2: // 1.0 x B - 1.0 x F
                final_color = back_color - final_color;
                final_color = clamp( final_color, 0, 1 );
                break;
            case 3: // 1.0 x B + 0.25 x F
                final_color = back_color + final_color * 0.25;
                break;
            default:
                final_color = final_color;
                break;
        }
    }

    return vec4(final_color, 1.0);
}



int get_color_16bit( ivec2 texel_coord )
{
    uint low = texelFetch( g_texture, texel_coord, 0 ).r;
    uint high = texelFetch( g_texture, texel_coord + ivec2(1, 0), 0 ).r;
    return int(low | (high << 8));
}



vec4 get_g_clut_color( int color_id )
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
                final_color = get_g_clut_color( int(texel) & 0xf );
            }
            else
            {
                final_color = get_g_clut_color( int(texel) >> 4 );
            }
        }
        else if( g_depth == 1 ) // 8-bit
        {
            ivec2 vram_coord = ivec2(g_tpage.x * 2, g_tpage.y) + texel_coord;
            uint texel = texelFetch( g_texture, vram_coord, 0 ).r;
            final_color = get_g_clut_color( int(texel) );
        }
        else // 16-bit direct
        {
            ivec2 vramCoord = ivec2( g_tpage.x * 2, g_tpage.y ) + texel_coord;
            int color = get_color_16bit( vramCoord );
            final_color = psx_color_to_rgba( color );
        }
    }
    else
    {
        final_color = vColor;
    }

    fragColor = final_color;
}
