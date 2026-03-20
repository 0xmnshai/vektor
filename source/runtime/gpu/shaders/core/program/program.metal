#include <metal_stdlib>
using namespace metal;

struct Vertex {
    packed_float3 position;
    packed_float3 normal;
};

struct VertexOut {
    float4 position [[position]];
    float3 vNormal;
    float3 vFragPos;
    int isLight [[flat]];
};

struct Light {
    int type;
    float _pad0[3];
    packed_float3 position;
    float _pad1;
    packed_float3 color;
    float _pad2;
    float energy;
    float range;
    float _pad3[2];
};

struct LightingUniforms {
    int numLights;
    float _pad0[3];
    Light lights[8];
};

struct GlobalUniforms {
    float4x4 view;
    float4x4 projection;
    float4x4 lightSpaceMatrices[8];
    LightingUniforms lighting;
    packed_float3 viewPos;
    float _pad_viewPos;
    float time;
    float padding[3];
};

struct ObjectUniforms {
    float4x4 model;
    int isLight;
    float padding[3];
};

vertex VertexOut vertex_main(uint vertexID [[vertex_id]],
                             const device Vertex* vertices [[buffer(0)]],
                             constant ObjectUniforms &obj [[buffer(2)]],
                             constant GlobalUniforms &uniforms [[buffer(1)]])
{
    VertexOut out;
    device const Vertex &v = vertices[vertexID];
    out.vFragPos = (obj.model * float4(float3(v.position), 1.0)).xyz;
    out.vNormal = (obj.model * float4(float3(v.normal), 0.0)).xyz;
    
    out.position = uniforms.projection * uniforms.view * float4(out.vFragPos, 1.0);
    // Remap OpenGL Z [-1, 1] to Metal [0, 1]
    out.position.z = (out.position.z + out.position.w) * 0.5;
    out.isLight = obj.isLight;
    return out;
}

float calculateMetalShadow(texture2d_array<float> shadowMapArray, sampler shadowSampler, float3 fragPos, float4x4 lightSpaceMatrix, int layer) {
    float4 lightSpacePos = lightSpaceMatrix * float4(fragPos, 1.0);
    float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    
    // transform to [0,1] range
    float3 shadowCoords;
    shadowCoords.xy = projCoords.xy * 0.5 + 0.5;
    shadowCoords.y = 1.0 - shadowCoords.y; // Flip Y for Metal
    shadowCoords.z = projCoords.z; // Depth is already [0, 1] from shadow pass remapping
    
    if (shadowCoords.z > 1.0 || shadowCoords.z < 0.0) return 0.0;

    float currentDepth = shadowCoords.z;
    float bias = 0.005;
    float shadow = 0.0;
    
    float2 texelSize = 1.0 / float2(shadowMapArray.get_width(), shadowMapArray.get_height());
    
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = shadowMapArray.sample(shadowSampler, shadowCoords.xy + float2(x, y) * texelSize, layer).r;
            shadow += (currentDepth - bias > pcfDepth) ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

fragment float4 fragment_main(VertexOut in [[stage_in]],
                               constant float4 &color [[buffer(0)]],
                               constant float3 &emissive [[buffer(3)]],
                               constant GlobalUniforms &uniforms [[buffer(1)]],
                               texture2d_array<float> shadowMapArray [[texture(0)]])
{
    if (in.isLight) {
        return float4(1.0, 1.0, 1.0, 1.0);
    }

    constexpr sampler shadowSampler(coord::normalized,
                                    address::clamp_to_edge,
                                    filter::linear);

    float3 norm = length(in.vNormal) > 0.0001 ? normalize(in.vNormal) : float3(0.0, 1.0, 0.0);
    float3 viewDir = normalize(float3(uniforms.viewPos) - in.vFragPos);
    
    float3 lightingResult = 0.05 * color.rgb + emissive;

    for(int i = 0; i < uniforms.lighting.numLights; i++) {
        float shadowFactor = calculateMetalShadow(shadowMapArray, shadowSampler, in.vFragPos, uniforms.lightSpaceMatrices[i], i);

        float3 lightPos = float3(uniforms.lighting.lights[i].position);
        float3 lightColor = float3(uniforms.lighting.lights[i].color);
        
        float3 lightDir = normalize(lightPos - in.vFragPos);
        float distance = length(lightPos - in.vFragPos);
        float attenuation = clamp(1.0 - (distance / uniforms.lighting.lights[i].range), 0.0, 1.0);
        attenuation *= attenuation;

        // Diffuse (use abs to ignore normal orientation for now)
        float diff = max(dot(norm, lightDir), 0.0);
        float3 diffuse = diff * lightColor * uniforms.lighting.lights[i].energy * attenuation;
        
        // Specular
        float3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        float3 specular = spec * lightColor * uniforms.lighting.lights[i].energy * attenuation;

        lightingResult += (diffuse * color.rgb + specular) * (1.0 - shadowFactor);
    }
    
    return float4(lightingResult, color.a);
}