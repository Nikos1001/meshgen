
#version 330 core

in vec2 pUv;

uniform sampler2D uColor;
uniform sampler2D uNorm;
uniform sampler2D uPos;
uniform vec3 uCamPos;


out vec4 FragColor;

void main() {
    vec4 color = texture(uColor, pUv);
    vec3 norm = texture(uNorm, pUv).xyz;
    vec3 pos = texture(uPos, pUv).xyz;

    vec3 lightPos = vec3(-1.0f, 2.0f, -3.0f);
    vec3 lightColor = vec3(0.95f, 0.97f, 1.0f);

    vec3 lighting = vec3(0.25f, 0.25f, 0.25f); // ambient lighting

    vec3 lightDir = normalize(lightPos - pos);
    float diffuse = max(dot(norm, lightDir), 0.0);
    lighting += lightColor * diffuse; // diffuse lighting

    vec3 lookDir = normalize(uCamPos - pos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(lookDir, reflectDir), 0.0), 6);
    lighting += lightColor * spec; // specular lighting

    FragColor = color * vec4(lighting, 1.0f);
}
