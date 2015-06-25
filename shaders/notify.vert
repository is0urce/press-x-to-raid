#version 330

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec4 color;

smooth out vec4 theColor;
smooth out vec2 theTexCoord;

uniform float scale;
uniform float aspect;
uniform vec2 movement;
uniform float phase;
uniform float timer;

void main()
{
	float wt = max(0, 1 - phase);
	gl_Position = vec4((position + wt * movement + vec2(0, timer)) * vec2(1, aspect) * scale, 0, 1);
    theColor = color;
    theTexCoord = tex;
}