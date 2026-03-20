#version 410 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

uniform vec3 color;

#define MAX_LIGHTS 8

struct Light {
    int type; // 0: Point
    vec3 position;
    vec3 color;
    float energy;
    float range;
};

uniform int numLights;
uniform Light lights[MAX_LIGHTS];

uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

float calculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    // Check if outside shadow map range
    if (projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

vec3 calculateLight(Light light, vec3 normal, vec3 viewDir, vec3 fragPos, float shadow) {
    vec3 lightDir = normalize(light.position - fragPos);
    float dist = length(light.position - fragPos);

    // Physical-ish falloff
    float attenuation = 1.0 / (1.0 + 0.1 * dist + 0.01 * dist * dist);
    // Soft range limit
    attenuation *= clamp(1.0 - (dist / light.range), 0.0, 1.0);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color * light.energy;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = 0.5 * spec * light.color * light.energy;

    return (diffuse * (1.0 - shadow) + specular * (1.0 - shadow)) * attenuation;
}

uniform vec3 emissive;

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(vec3(0, 5, 5) - FragPos);

    // Ambient / Environment logic
    float heightAO = clamp(FragPos.y * 0.4 + 0.6, 0.3, 1.0);
    float upFactor  = clamp(norm.y * 0.5 + 0.5, 0.6, 1.0);
    float ao = heightAO * upFactor;
    vec3 ambient = 0.3 * color * ao;

    vec3 result = ambient + emissive;

    for (int i = 0; i < numLights; i++) {
        float shadow = 0.0;
        if (i == 0 && numLights > 0) {
            vec3 lightDir0 = normalize(lights[0].position - FragPos);
            shadow = calculateShadow(FragPosLightSpace, norm, lightDir0);
        }
        vec3 lightColor = calculateLight(lights[i], norm, viewDir, FragPos, shadow);
        if (!isnan(lightColor.r) && !isinf(lightColor.r)) {
            result += lightColor * color;
        }
    }

    FragColor = vec4(result, 1.0);
}