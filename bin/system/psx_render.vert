#version 120

// Атрибуты вершин POLY_FT4
attribute vec3 position;    // x, y, z
attribute vec2 texcoord;    // u, v
attribute vec4 color;       // r, g, b, a

// Varying
varying vec2 vTexCoord;
varying vec4 vColor;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * vec4(position, 1.0);

    // send to fragment shader
    vTexCoord = texcoord;
    vColor = color;
}