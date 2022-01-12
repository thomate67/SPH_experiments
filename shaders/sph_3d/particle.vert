#version 460

layout (location = 0) in vec4 position;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main ()
{
    
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * position;
    gl_PointSize = 5;
}