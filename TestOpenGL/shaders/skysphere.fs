#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform sampler2D skysphere;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}