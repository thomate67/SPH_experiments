#version 450

layout (location = 0) in vec4 position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() 
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * position;
    gl_PointSize = 10.f;
}