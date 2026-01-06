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
}