#version 310 es

precision mediump float;
precision mediump sampler2D;
precision mediump sampler2DArray;
precision lowp samplerCube;

uniform sampler2DArray u_TextureArray;
//uniform int u_maxLightLevel;
uniform float u_SunIntensity;

out vec4 FragColor;

in vec2 TexCoord;
in flat int TexID;
in flat int LightLevel;
in flat float Shade;

void main()
{
	vec4 albedo = texture(u_TextureArray, vec3(TexCoord, TexID));
	float alpha = albedo.a;
	
	if(albedo.a < 0.5)
	{
		discard;
	}
	
	int u_maxLightLevel = 20;
	int u_minLightLevel = 4;
	int in_skyExposure = LightLevel & 0xF;
	int in_blockLight = (LightLevel >> 4) & 0xF;
	float light = float(in_blockLight + u_minLightLevel) / float(u_maxLightLevel);
	float skylight = float(in_skyExposure + u_minLightLevel) / float(u_maxLightLevel) * u_SunIntensity;
	
	vec4 finalColor = albedo * max(light, skylight) * Shade;
	finalColor.a = alpha;
	
	FragColor = finalColor;
}