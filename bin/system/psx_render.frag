#version 130

uniform usampler2D texture0;
uniform ivec2 tpage;
uniform ivec2 clut;
uniform int depth;
uniform int transp;
uniform int abr;
uniform int dtd;

in vec4 vColor;
in vec2 vTexCoord;

const float dither_matrix[16] = float[]
(
    -4.0,  0.0, -3.0,  1.0,
     2.0, -2.0,  3.0, -1.0,
    -3.0,  1.0, -4.0,  0.0,
     3.0, -1.0,  2.0, -2.0
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

    int r = (color >> 0) & 31;
    int g = (color >> 5) & 31;
    int b = (color >> 10) & 31;
    int stp_bit = (color >> 15) & 1;

    vec3 full_color = vec3( float( r ), float( g ), float( b ) );

    float dither_value = dither_matrix[(frag_coord.y % 4) * 4 + (frag_coord.x % 4)];

    if( dtd == 1 )
    {
        full_color += dither_value;
        full_color = clamp( full_color, 0.0, 31.0 );
    }

    float alpha = 1.0;

    if( color == 0 )
    {
        alpha = 0.0;
    }
    else
    {
        if( r == 0 && g == 0 && b == 0 )
        {
            alpha = 0.0;
        }

        if( transp == 1 && stp_bit == 1 )
        {
            alpha = 0.5;

            // need to use abr here for different color mixing
        }
    }

    return vec4(full_color, alpha);
}



int get_color_16bit( ivec2 texel_coord )
{
    uint low = texelFetch( texture0, texel_coord, 0 ).r;
    uint high = texelFetch( texture0, texel_coord + ivec2(1, 0), 0 ).r;
    return int(low | (high << 8));
}



vec4 get_clut_color( int color_id )
{
    ivec2 clut_texel_coord = ivec2((clut.x + color_id) * 2, clut.y);
    int color = get_color_16bit( clut_texel_coord );
    return psx_color_to_rgba( color );
}



void main()
{
    vec4 final_color;

    ivec2 texel_coord = ivec2(vTexCoord);

    if( depth == 0 ) // 4-bit
    {
        ivec2 vram_coord = ivec2( tpage.x * 2, tpage.y ) + ivec2(texel_coord.x / 2, texel_coord.y);
        uint texel = texelFetch( texture0, vram_coord, 0 ).r;

        if( (texel_coord.x & 1) == 0 )
        {
            final_color = get_clut_color( int( texel ) & 0xf );
        }
        else
        {
            final_color = get_clut_color( int( texel ) >> 4 );
        }
    }
    else if( depth == 1 ) // 8-bit
    {
        ivec2 vram_coord = ivec2( tpage.x * 2, tpage.y ) + texel_coord;
        uint texel = texelFetch( texture0, vram_coord, 0 ).r;
        final_color = get_clut_color( int(texel) );
    }
    else // 16-bit direct
    {
        ivec2 vramCoord = ivec2( tpage.x * 2, tpage.y ) + texel_coord;
        int color = get_color_16bit( vramCoord );
        final_color = psx_color_to_rgba( color );
    }

    final_color = vec4(
        psx_gamma_lut[int(final_color.x)],
        psx_gamma_lut[int(final_color.y)],
        psx_gamma_lut[int(final_color.z)],
        final_color.w
    );

    fragColor = final_color * vColor;
}
