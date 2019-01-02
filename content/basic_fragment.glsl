#version 330

in vec4 TCoord;

uniform sampler2D textureSampler;
layout(location = 0) out vec4 color;

void main() 
{
    color = texture(textureSampler, TCoord.rg);
}