
#version 330 core

in vec2 pUv;
in vec3 pNorm;
in vec3 pTang;
in vec3 pPos;
in float pTex;

layout (location = 0) out vec4 oColor;
layout (location = 1) out vec3 oNorm;
layout (location = 2) out vec3 oPos;

uniform sampler2D uColor[3];
uniform sampler2D uNormMap[3];
uniform sampler2D uRough[3];

void main() {
    vec4 t = texture(uColor[int(pTex)], pUv);
    vec4 T = texture(uNormMap[int(pTex)], pUv);
    vec3 bitang = cross(pTang, pNorm);
    vec3 badNorm = texture(uNormMap[int(pTex)], pUv).xyz * 2 - 1;
    vec3 norm = badNorm.x * pTang +
                badNorm.y * bitang +
                badNorm.z * pNorm;

    oColor = texture(uColor[int(pTex)], pUv);
    oNorm = norm;
    oPos = pPos;
}
