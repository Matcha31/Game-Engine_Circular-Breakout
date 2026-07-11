#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aUV;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 vWorldPos;
out vec3 vWorldNormal;
out vec3 vColor;
out vec2 vUV;

void main()
{
    vec4 worldPos = uModel * vec4(aPosition, 1.0);
    vWorldPos = worldPos.xyz;

    mat3 normalMat = mat3(transpose(inverse(uModel)));
    vWorldNormal = normalize(normalMat * aNormal);

    vColor = aColor;
    vUV = aUV;

    gl_Position = uProj * uView * worldPos;
}
