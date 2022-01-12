#version 450

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvAttribute;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() 
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * position;
    gl_PointSize = 10.f;
}