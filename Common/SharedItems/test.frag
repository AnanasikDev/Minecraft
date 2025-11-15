#version 310 es

precision mediump float;
precision mediump sampler2D;
precision mediump sampler2DArray;
precision lowp samplerCube;

uniform sampler2DArray u_TextureArray;

out vec4 FragColor;

in vec2 TexCoord;
in flat int TexID;

void main()
{
	FragColor = texture(u_TextureArray, vec3(TexCoord, TexID));
	//vec4(float(TexID) / 3.0, 0.0, 0.0, 1.0);// 
}