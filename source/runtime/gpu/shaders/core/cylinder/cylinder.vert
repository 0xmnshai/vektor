#version 410 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vNormal;
out vec3 vFragPos;

void main()
{
    vNormal = mat3(transpose(inverse(model))) * normal;
    vFragPos = vec3(model * vec4(pos, 1.0));
    gl_Position = projection * view * vec4(vFragPos, 1.0);
}
