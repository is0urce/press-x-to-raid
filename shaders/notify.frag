#version 330

smooth in vec4 theColor;
smooth in vec2 theTexCoord;

out vec4 outputColor;
uniform sampler2D glyphTexture;

uniform float timer;

//uniform float outline;

// main

//float samp(vec2 tc, vec2 l1, float c1, vec2 l2, float c2)
//{
//	return texture(glyphTexture, tc + l1 * c1 + l2 * c2).r;
//}
//
//float oline(vec2 tc, vec2 l1, vec2 l2)
//{
//	float acc = 0;
//	acc += samp(tc, l1, 1, l2, 0);
//	acc += samp(tc, l1, -1, l2, 0);
//	acc += samp(tc, l1, 0, l2, 1);
//	acc += samp(tc, l1, 0, l2, -1);
//	acc += samp(tc, l1, 1, l2, 1) * 0.7;
//	acc += samp(tc, l1, -1, l2, -1) * 0.7;
//	acc += samp(tc, l1, -1, l2, 1) * 0.7;
//	acc += samp(tc, l1, 1, l2, -1) * 0.7;
//
//	acc *= 0.147;
//	//acc *= 0.3;
//	//acc *= 0.25;
//
//	return mix(0, 1, clamp(acc, 0, 1));
//}

void main()
{
	//vec4 pix = theColor;
	//pix *= texture(glyphTexture, theTexCoord.xy).r;
	//pix.rgb *= pix.rgb * texture(glyphTexture, theTexCoord.xy).r;
	//float ol = oline(theTexCoord.xy, vec2(outline, 0), vec2(0, outline));
	//pix.a = clamp(pix.a, ol, 1);
	//pix.rgb = mix(pix.rgb, vec3(0, 0, 0), clamp(ol - pix.a, 0, 1));
	
	vec4 pix = theColor * texture(glyphTexture, theTexCoord.xy).rrrr;
	pix.a = mix(pix.a, 0, clamp(timer, 0, 1));
	outputColor = pix;
}