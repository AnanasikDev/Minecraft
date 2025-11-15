#version 310 es
#extension GL_ARB_explicit_attrib_location : enable

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in int aTexID;

uniform mat4 u_ModelMat;
uniform mat4 u_ViewMat;
uniform mat4 u_ProjMat;

out vec2 TexCoord;
out flat int TexID;

void main()
{
    gl_Position = u_ProjMat * u_ViewMat * u_ModelMat * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    TexID = aTexID;
}