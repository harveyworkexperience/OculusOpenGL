#version 330 core
layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = a_texcoord;
    vec4 pos = projection * view * vec4(a_vertex, 1.0);
    gl_Position = pos.xyww;
}  