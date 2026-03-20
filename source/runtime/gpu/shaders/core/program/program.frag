#version 410 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 color;

void main() {
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.5));
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    float heightAO = clamp(FragPos.y * 0.4 + 0.6, 0.3, 1.0);
    float upFactor = clamp(norm.y * 0.5 + 0.5, 0.6, 1.0);
    float ao = heightAO * upFactor;
    vec3 ambient = 0.3 * color * ao;
    vec3 diffuse = diff * color;
    vec3 viewDir = normalize(vec3(0, 5, 5) - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = 0.05 * spec * vec3(1.0);
    
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}