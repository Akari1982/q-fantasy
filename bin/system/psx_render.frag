#version 130

uniform usampler2D texture0;
uniform ivec2 tpage;
uniform ivec2 clut;
uniform int depth;
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

    if( dtd == 1 ) full_color += dither_value;

    vec3 final_color = floor( full_color );

    float alpha = 1.0;
    if( abr == 0 && stp_bit == 1 )
    {
        if( (r == 0) && (g == 0) && (b == 0) )
        {
            alpha = 0.0;
        }
        else
        {
            alpha = 0.5;
        }
    }

    return vec4(final_color / 31.0, alpha);
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
    vec4 finalColor;

    ivec2 texel_coord = ivec2(vTexCoord);

    if( depth == 0 ) // 4-bit
    {
        ivec2 vram_coord = ivec2( tpage.x * 2, tpage.y ) + ivec2(texel_coord.x / 2, texel_coord.y);
        uint texel = texelFetch( texture0, vram_coord, 0 ).r;

        if( (texel_coord.x & 1) == 0 )
        {
            finalColor = get_clut_color( int( texel ) & 0xf );
        }
        else
        {
            finalColor = get_clut_color( int( texel ) >> 4 );
        }
    }
    else if( depth == 1 ) // 8-bit
    {
        ivec2 vram_coord = ivec2( tpage.x * 2, tpage.y ) + texel_coord;
        uint texel = texelFetch( texture0, vram_coord, 0 ).r;
        finalColor = get_clut_color( int(texel) );
    }
    else // 16-bit direct
    {
        ivec2 vramCoord = ivec2( tpage.x * 2, tpage.y ) + texel_coord;
        int color = get_color_16bit( vramCoord );
        finalColor = psx_color_to_rgba( color );
    }

    fragColor = finalColor * vColor;
}
