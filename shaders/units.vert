#version 330

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texture;
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 positionprev;

smooth out vec4 theColor;
smooth out vec2 theTexture;

uniform float scale;
uniform float aspect;
uniform float phase;
uniform vec2 movement;

void main()
{
	gl_Position = vec4(mix(positionprev + movement, position, min(phase, 1)) * vec2(1, aspect) * scale, 0, 1);
    theColor = color;
    theTexture = texture;
}