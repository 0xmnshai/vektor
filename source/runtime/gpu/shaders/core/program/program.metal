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
    float4 vFragPosLightSpace;
};

struct Uniforms {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
    float4x4 lightSpaceMatrix;
};

// Must match C++ GPULight struct layout (64 bytes total, 16-byte aligned)
struct Light {
    int type;          // 4 bytes
    float3 _pad0;      // 12 bytes (padding)
    packed_float3 position; // 12 bytes
    float _pad1;       // 4 bytes
    packed_float3 color;    // 12 bytes
    float energy;      // 4 bytes
    float range;       // 4 bytes
    float3 _pad2;      // 12 bytes (padding to 64 bytes)
};

// Must match C++ LightingUniforms struct (num_lights + 12-byte pad + lights)
struct LightingUniforms {
    int numLights;     // 4 bytes
    float3 _pad0;      // 12 bytes (padding to 16-byte boundary)
    Light lights[8];   // 8 * 64 = 512 bytes
};

vertex VertexOut vertex_main(uint vertexID [[vertex_id]],
                             const device Vertex* vertices [[buffer(0)]],
                             constant Uniforms &uniforms [[buffer(1)]])
{
    VertexOut out;
    device const Vertex &v = vertices[vertexID];
    out.vFragPos = (uniforms.model * float4(float3(v.position), 1.0)).xyz;
    out.vNormal = (uniforms.model * float4(float3(v.normal), 0.0)).xyz;
    out.vFragPosLightSpace = uniforms.lightSpaceMatrix * float4(out.vFragPos, 1.0);
    out.position = uniforms.projection * uniforms.view * float4(out.vFragPos, 1.0);
    return out;
}

float3 calculateLight(Light light, float3 fragPos, float3 normal, float3 viewDir) {
    float3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    
    // Physical-ish falloff
    float attenuation = 1.0 / (1.0 + 0.1 * distance + 0.01 * distance * distance);
    // Soft range limit
    attenuation *= clamp(1.0 - (distance / light.range), 0.0, 1.0);

    float diff = max(dot(normal, lightDir), 0.0);
    float3 diffuse = diff * light.color * light.energy;

    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    float3 specular = 0.5 * spec * light.color * light.energy;

    return (diffuse + specular) * attenuation;
}

fragment float4 fragment_main(VertexOut in [[stage_in]],
                               constant float4 &color [[buffer(0)]],
                               constant float3 &emissive [[buffer(3)]],
                               constant LightingUniforms &lighting [[buffer(2)]],
                               texture2d<float> shadowMap [[texture(0)]])
{
    constexpr sampler shadowSampler(coord::normalized,
                                    address::clamp_to_edge,
                                    filter::linear,
                                    compare_func::less);

    float3 norm = normalize(in.vNormal);
    float3 viewDir = normalize(float3(0, 5, 5) - in.vFragPos);
    
    // Shadow calculation
    float3 projCoords = in.vFragPosLightSpace.xyz / in.vFragPosLightSpace.w;
    float2 shadowCoords = projCoords.xy * 0.5 + 0.5;
    shadowCoords.y = 1.0 - shadowCoords.y; // Flip Y for Metal
    float currentDepth = projCoords.z;
    
    float shadow = 0.0;
    if (currentDepth <= 1.0) {
        shadow = shadowMap.sample_compare(shadowSampler, shadowCoords, currentDepth - 0.005);
    }
    // sample_compare returns 1.0 if NOT in shadow (test passes), 0.0 if in shadow.
    // Our calculateLight expects 1.0 for shadow, 0.0 for no shadow.
    float shadowFactor = 1.0 - shadow;

    float heightAO = clamp(in.vFragPos.y * 0.4 + 0.6, 0.3, 1.0);
    float upFactor = clamp(norm.y * 0.5 + 0.5, 0.6, 1.0);
    float ao = heightAO * upFactor;

    float3 ambient = 0.3 * color.rgb * ao;
    float3 result = ambient + emissive;

    for(int i = 0; i < lighting.numLights; i++) {
        float3 lightColor = calculateLight(lighting.lights[i], in.vFragPos, norm, viewDir);
        if (!any(isnan(lightColor))) {
            float s = (i == 0) ? shadowFactor : 0.0;
            result += lightColor * color.rgb * (1.0 - s);
        }
    }
    
    return float4(result, color.a);
}