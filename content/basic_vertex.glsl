#version 330

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexNormal;
layout(location = 2) in vec4 vertexColor;
layout(location = 3) in vec4 vertexTCoord;

uniform mat4 mvp;

out vec3 vPosition;
out vec4 vColor;
out vec4 vTCoord;

void main()
{
    gl_Position = mvp * vec4(vertexPosition, 1.0f);
    vPosition = vertexPosition;
    vColor = vertexColor;
    vTCoord = vertexTCoord;
}