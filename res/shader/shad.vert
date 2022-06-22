
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUv;
layout (location = 2) in vec3 aNorm;
layout (location = 3) in vec3 aTang;
layout (location = 4) in float aTex;

uniform mat4 uProj;
uniform mat4 uView;
uniform mat4 uModel;

out vec2 pUv;
out vec3 pNorm;
out vec3 pTang;
out vec3 pPos;
out float pTex;

void main() {
   pUv = aUv;
   pNorm = aNorm;
   pTang = aTang;
   pPos = (uModel * vec4(aPos.xyz, 1.0)).xyz;
   pTex = aTex;

   gl_Position = uProj * uView * uModel * vec4(aPos.xyz, 1.0);
}