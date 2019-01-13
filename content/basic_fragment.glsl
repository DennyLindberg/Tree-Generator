#version 330

uniform sampler2D textureSampler;
layout(location = 0) out vec4 color;

in vec3 vPosition;
in vec4 vColor;
in vec4 vTCoord;

void main() 
{
    color = texture(textureSampler, vTCoord.rg);
    //color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}