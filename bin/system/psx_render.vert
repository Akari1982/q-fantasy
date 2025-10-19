#version 150

// POLY_FT4
in vec4 position;
in vec4 color;
in vec3 normals;
in vec2 texcoord;

out vec4 vColor;
out vec2 vTexCoord;

uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * position;

    // send to fragment shader
    vColor = color;
    vTexCoord = texcoord;
}
