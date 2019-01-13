#version 330

uniform sampler2D textureSampler;
layout(location = 0) out vec4 color;

in vec3 vPosition;
in vec4 vColor;
in vec4 vTCoord;

void main() 
{
    vec4 sample = texture(textureSampler, vTCoord.rg);
    color = mix(vec4(0.2f, 0.7f, 0.3f, 1.0f), vec4(0.1f, 0.3f, 0.1f, 1.0f), sample.a);
}