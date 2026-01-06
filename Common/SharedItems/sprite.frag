#version 310 es

precision mediump float;
precision mediump sampler2D;
precision mediump sampler2DArray;

uniform sampler2DArray u_TextureArray;

out vec4 FragColor;

in vec2 TexCoord;
in flat int TexID;

void main()
{
	FragColor = texture(u_TextureArray, vec3(TexCoord, TexID));
	//vec4 albedo = texture(u_TextureArray, vec3(TexCoord, TexID));
	//float alpha = albedo.a;
	//
	//if (alpha < 0.1)
	//{
	//	discard;
	//}
	//
	//vec4 finalColor = albedo;
	//
	//finalColor.a = alpha;
	//
	//FragColor = finalColor;
}