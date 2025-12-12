#version 310 es

precision mediump float;
precision mediump sampler2D;
precision mediump sampler2DArray;
precision lowp samplerCube;

uniform sampler2DArray u_TextureArray;
//uniform int u_maxLightLevel;

out vec4 FragColor;

in vec2 TexCoord;
in flat int TexID;
in flat int LightLevel;

void main()
{
	int u_maxLightLevel = 20;
	int u_minLightLevel = 4;
	int lightLevel = LightLevel;
	//float light = float(clamp(lightLevel, u_minLightLevel, u_maxLightLevel)) / float(u_maxLightLevel);
	float light = float(lightLevel + u_minLightLevel) / float(u_maxLightLevel);
	vec4 albedo = texture(u_TextureArray, vec3(TexCoord, TexID));
	float alpha = albedo.a;

	if(albedo.a < 0.5)
	{
		discard;
	}
	
	vec4 finalColor = albedo * light;
	finalColor.a = alpha;

	FragColor = finalColor;
}