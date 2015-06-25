#version 330

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;

smooth out vec4 theColor;

uniform float scale;
uniform float aspect;

void main()
{
	gl_Position = vec4(position * scale, 0, 1);
	gl_Position.y *= aspect;
	theColor = color;
}