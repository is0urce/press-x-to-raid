#version 330

smooth in vec4 theColor;
smooth in vec2 theTexture;

out vec4 outputColor;
uniform sampler2D glyphTexture;
uniform vec3 rng;

//

highp float rand2(vec2 co)
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co.xy ,vec2(a,b));
    highp float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

vec3 noise(vec3 seed)
{
	return vec3
	(
		rand2(vec2(gl_FragCoord.x, rand2(vec2(seed.r, gl_FragCoord.y)))),
		rand2(vec2(gl_FragCoord.y, rand2(vec2(seed.g, gl_FragCoord.x)))),
		rand2(vec2(rand2(gl_FragCoord.xy), seed.b))
	);
}

// main

void main()
{
	vec4 pix = theColor;
	pix.a *= texture(glyphTexture, theTexture.xy).r;
	//pix.rgb += noise(rng) / 128; // dithering
	//pix.rgb += noise(rng) / 2; // dithering
	outputColor = pix;
}