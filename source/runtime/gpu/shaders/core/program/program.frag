#version 410 core

out vec4 FragColor;

struct Light {
    int type;
    vec3 position;
    vec3 color;
    float energy;
    float range;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace[8];

uniform vec3 viewPos;
uniform int numLights;
uniform Light lights[8];
uniform sampler2DArray shadowMapArray;
uniform bool isLight;

float calculateShadow(int layer, vec4 fragPosLightSpace) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0)
        return 0.0;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    float bias = 0.005;

    // 3x3 PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMapArray, 0));
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMapArray, vec3(projCoords.xy + vec2(x, y) * texelSize, float(layer))).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    return shadow;
}

void main() {
    if (isLight) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        return;
    }

    vec3 color = vec3(0.0);
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    for (int i = 0; i < numLights; i++) {
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float distance = length(lights[i].position - FragPos);
        
        // Attenuation based on range
        float attenuation = clamp(1.0 - (distance / lights[i].range), 0.0, 1.0);
        attenuation *= attenuation; // Quadratic falloff
        
        // Diffuse
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * lights[i].color * lights[i].energy * attenuation;
        
        // Shadows
        float shadow = 0.0;
        if (i < 8) {
            shadow = calculateShadow(i, FragPosLightSpace[i]);
        }
        
        color += (1.0 - shadow) * diffuse;
    }

    // Ambient
    vec3 ambient = 0.05 * vec3(1.0);
    FragColor = vec4(ambient + color, 1.0);
}