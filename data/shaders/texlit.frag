#version 330 core

in vec3 vWorldPos;
in vec3 vWorldNormal;
in vec3 vColor;
in vec2 vUV;

uniform vec3 uLightDir;
uniform vec3 uCameraPos;

uniform float uAmbient;
uniform float uSpecular;
uniform float uShininess;

uniform sampler2D uTex;

out vec4 FragColor;

void main()
{
    vec3 baseTex = texture(uTex, vUV).rgb;
    vec3 base = baseTex * vColor;

    vec3 N = normalize(vWorldNormal);
    vec3 L = normalize(uLightDir);
    vec3 V = normalize(uCameraPos - vWorldPos);
    vec3 R = reflect(-L, N);

    float diff = max(dot(N, L), 0.0);
    float spec = 0.0;
    if (diff > 0.0)
        spec = pow(max(dot(V, R), 0.0), uShininess);

    vec3 color = base * (uAmbient + diff) + vec3(spec) * uSpecular;
    FragColor = vec4(color, 1.0);
}
