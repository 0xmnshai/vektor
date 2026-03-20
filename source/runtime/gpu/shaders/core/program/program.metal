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
    float4 vFragPosLightSpace[8];
    int isLight [[flat]];
};

struct Light {
    int type;            // 4 bytes
    float _pad0[3];      // 12 bytes
    float3 position;     // 16 bytes (aligned)
    float3 color;        // 16 bytes (aligned)
    float energy;        // 4 bytes
    float range;         // 4 bytes
    float _pad2[2];      // 8 bytes (padding to 64 bytes)
};

struct LightingUniforms {
    int numLights;       // 4 bytes
    float _pad0[3];      // 12 bytes
    Light lights[8];     // 8 * 64 = 512 bytes
};

struct GlobalUniforms {
    float4x4 view;
    float4x4 projection;
    float4x4 lightSpaceMatrices[8];
    LightingUniforms lighting;
    float time;
    float padding[3];
};

vertex VertexOut vertex_main(uint vertexID [[vertex_id]],
                             const device Vertex* vertices [[buffer(0)]],
                             constant struct { float4x4 model; int isLight; } &obj [[buffer(2)]],
                             constant GlobalUniforms &uniforms [[buffer(1)]])
{
    VertexOut out;
    device const Vertex &v = vertices[vertexID];
    out.vFragPos = (obj.model * float4(float3(v.position), 1.0)).xyz;
    out.vNormal = (obj.model * float4(float3(v.normal), 0.0)).xyz;
    for (int i = 0; i < 8; i++) {
        out.vFragPosLightSpace[i] = uniforms.lightSpaceMatrices[i] * float4(out.vFragPos, 1.0);
    }
    out.position = uniforms.projection * uniforms.view * float4(out.vFragPos, 1.0);
    out.isLight = obj.isLight;
    return out;
}

float calculateMetalShadow(texture2d_array<float> shadowMapArray, sampler shadowSampler, float4 lightSpacePos, int layer) {
    float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    float2 shadowCoords = projCoords.xy * 0.5 + 0.5;
    shadowCoords.y = 1.0 - shadowCoords.y; // Flip Y for Metal
    
    if (projCoords.z > 1.0 || projCoords.z < 0.0) return 0.0;

    float currentDepth = projCoords.z;
    float bias = 0.005;
    float shadow = 0.0;
    
    float2 texelSize = 1.0 / float2(shadowMapArray.get_width(), shadowMapArray.get_height());
    
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = shadowMapArray.sample(shadowSampler, shadowCoords + float2(x, y) * texelSize, layer);
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

    float3 norm = normalize(in.vNormal);
    float3 viewDir = normalize(float3(0, 5, 5) - in.vFragPos);
    
    float3 ambient = 0.05 * float3(1.0);
    float3 result = ambient;

    for(int i = 0; i < uniforms.lighting.numLights; i++) {
        float shadowFactor = 0.0;
        if (i < 8) {
            shadowFactor = calculateMetalShadow(shadowMapArray, shadowSampler, in.vFragPosLightSpace[i], i);
        }

        float3 lightDir = normalize(uniforms.lighting.lights[i].position - in.vFragPos);
        float distance = length(uniforms.lighting.lights[i].position - in.vFragPos);
        float attenuation = clamp(1.0 - (distance / uniforms.lighting.lights[i].range), 0.0, 1.0);
        attenuation *= attenuation;

        float diff = max(dot(norm, lightDir), 0.0);
        float3 diffuse = diff * uniforms.lighting.lights[i].color * uniforms.lighting.lights[i].energy * attenuation;
        
        result += diffuse * (1.0 - shadowFactor);
    }
    
    return float4(result, color.a);
}