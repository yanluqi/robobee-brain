#version 330
 
in vec4 color;

out vec4 outputColor;

uniform sampler2D gSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;

 
void main()
{
    outputColor = color;
}