#version 410 core

in vec3 vNormal;
in vec3 vFragPos;

uniform vec4 color;
uniform vec3 lightPos;
uniform vec3 viewPos;

layout(location = 0) out vec4 FragColor;

void main()
{
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(lightPos - vFragPos);
    
    // Diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    
    // Fill light from opposite side
    vec3 fillLightDir = normalize(vec3(-lightPos.x, lightPos.y * 0.2, -lightPos.z));
    float fillDiff = max(dot(norm, fillLightDir), 0.0) * 0.3;
    
    // Fake directional shadow / Ambient Occlusion based on normal pointing down
    float groundShadow = clamp(norm.y * 0.5 + 0.6, 0.2, 1.0);
    vec3 ambient = 0.2 * color.rgb * groundShadow;
    
    vec3 diffuse = (diff + fillDiff) * color.rgb;
    
    // Specular
    float specularStrength = 0.2;
    vec3 viewDir = normalize(viewPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
    vec3 specular = specularStrength * spec * vec3(1.0);
    
    // Cavity / Edge detection for "natural border" effect
    vec3 dnx = dFdx(norm);
    vec3 dny = dFdy(norm);
    float edge = length(dnx) + length(dny);
    float cavity = 1.0 - smoothstep(0.1, 0.4, edge); // darken edges

    vec3 result = (ambient + diffuse + specular) * cavity;
    
    FragColor = vec4(result, color.a);
}
