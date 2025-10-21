#version 150

in vec4 position;
in vec4 color;
in vec3 normals;
in vec2 texcoord;

out vec4 vColor;
out vec2 vTexCoord;

uniform mat4 matrix;

void main()
{
    gl_Position = matrix * position;

    vColor = color;
    vTexCoord = texcoord;
}
