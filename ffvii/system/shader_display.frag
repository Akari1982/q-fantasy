#version 130

uniform usampler2D g_vram;

in vec2 vTexCoord;

out vec4 fragColor;

// gamma correction
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



vec3 apply_gamma( vec3 color )
{
    color.x = psx_gamma_lut[int(color.x) >> 0x3];
    color.y = psx_gamma_lut[int(color.y) >> 0x3];
    color.z = psx_gamma_lut[int(color.z) >> 0x3];
    return color;
}

vec4 color5_to_rgba( int color5 )
{
    float r = ((color5 >> 0x0) & 0x1f) << 0x3;
    float g = ((color5 >> 0x5) & 0x1f) << 0x3;
    float b = ((color5 >> 0xa) & 0x1f) << 0x3;

    vec3 color = vec3( r, g, b );

    color = apply_gamma( color );

    return vec4(color, 1.0);
}

void main()
{
    int color = int(texelFetch( g_vram, ivec2(vTexCoord.x + 0.5 / 1024.0, vTexCoord.y + 0.5 / 512.0), 0 ).r);
    fragColor = color5_to_rgba( color );
}
