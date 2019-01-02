#version 330

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexTCoord;
uniform mat4 mvp;

out vec4 TCoord;

void main()
{
    gl_Position = mvp * vec4(vertexPosition, 1.0f);
    TCoord = vertexTCoord;
}