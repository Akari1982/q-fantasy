#version 130

in vec4 position;
in vec2 texcoord;

uniform mat4 g_matrix;

out vec2 vTexCoord;

void main()
{
    gl_Position = g_matrix * position;
    vTexCoord = texcoord;
}