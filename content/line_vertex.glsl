#version 330

layout(location = 0) in vec3 vertexPosition;
uniform mat4 mvp;

out vec4 TCoord;

void main()
{
    gl_Position = mvp * vec4(vertexPosition, 1.0f);
}