#version 330
 
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;

out vec4 color;

uniform mat4 M;
uniform mat4 V;
uniform mat4 MVP;
uniform vec3 LightPosition_worldspace;
 
void main()
{
    gl_Position =  MVP * vec4(vPosition, 1.0);
 
    color = vec4( vColor, 1.0);
}