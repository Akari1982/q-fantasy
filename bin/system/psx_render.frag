#version 130 // Обновляем до версии 1.30, чтобы использовать texelFetch

// Устаревшие расширения не нужны
// #extension GL_EXT_gpu_shader4 : enable
// #extension GL_EXT_shader_integer_mix : enable

uniform usampler2D texture0;
uniform ivec4 tpage;
uniform ivec2 clut;
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
    ivec2 fragCoord = ivec2( gl_FragCoord.xy );

    int r = (color >> 0) & 31;
    int g = (color >> 5) & 31;
    int b = (color >> 10) & 31;
    int stp_bit = (color >> 15) & 1;

    vec3 full_color = vec3( float(r), float(g), float(b) );

    float dither_value = dither_matrix[(fragCoord.y % 4) * 4 + (fragCoord.x % 4)];

    if( dtd == 1 ) full_color += dither_value;

    vec3 final_color = floor(full_color);

    float alpha = 1.0;
    if( tpage.w == 0 && stp_bit == 1 )
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

    return vec4( final_color / 31.0, alpha );
}



int get_color_16bit(ivec2 texelCoord)
{
    uint low = texelFetch(texture0, texelCoord, 0).r;
    uint high = texelFetch(texture0, texelCoord + ivec2(1, 0), 0).r;
    return int(low | (high << 8));
}



int get_color_index( ivec2 texelCoord, int colorDepth )
{
    ivec2 vramCoord = ivec2(tpage.x * 2, tpage.y) + texelCoord;
    uint texelValue = texelFetch(texture0, vramCoord, 0).r;

    if (colorDepth == 0) // 4-bit
    {
        int pixelIndex = texelCoord.x & 1;
        if (pixelIndex == 0)
        {
            return int(texelValue) & 15;
        }
        else
        {
            return int(texelValue) >> 4;
        }
    }
    else // 8-bit
    {
        return int(texelValue);
    }
}



vec4 get_clut_color(int color_id, int clutX, int clutY)
{
    ivec2 clut_texel_coord = ivec2((clutX + color_id) * 2, clutY);
    int color = get_color_16bit(clut_texel_coord);
    return psx_color_to_rgba(color);
}



void main()
{
    int color_depth = tpage.z;
    vec4 finalColor;

    ivec2 texel_coord = ivec2(vTexCoord);

    if( color_depth == 2 ) // 16-bit direct
    {
        ivec2 vramCoord = ivec2( tpage.x * 2, tpage.y ) + texel_coord;
        int color = get_color_16bit( vramCoord );
        finalColor = psx_color_to_rgba( color );
    }
    else
    {
        int color_id = get_color_index( texel_coord, color_depth);
        finalColor = get_clut_color(color_id, clut.x, clut.y);
    }

    fragColor = finalColor * vColor;
}
