#version 310 es

precision mediump float;
precision mediump sampler2D;
precision mediump sampler2DArray;

uniform sampler2DArray u_TextureArray;
uniform float u_SunIntensity;

out vec4 FragColor;

in vec2 TexCoord;
in flat int TexID;
in flat int LightLevel;

void main()
{
	vec4 albedo = texture(u_TextureArray, vec3(TexCoord, TexID));
	float alpha = albedo.a;
	
	const int u_maxLightLevel = 20;
	const int u_minLightLevel = 4;
	int in_skyExposure = LightLevel & 0xF;
	int in_blockLight = (LightLevel >> 4) & 0xF;
	float light = float(in_blockLight + u_minLightLevel) / float(u_maxLightLevel);
	float skylight = float(in_skyExposure + u_minLightLevel) / float(u_maxLightLevel) * u_SunIntensity;
	
	vec4 finalColor = albedo * max(light, skylight);
	finalColor.a = alpha;
	
	FragColor = finalColor;
}