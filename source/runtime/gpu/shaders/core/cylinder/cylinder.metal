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
};

struct Uniforms {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

vertex VertexOut vertex_main(uint vertexID [[vertex_id]],
                                 const device Vertex* vertices [[buffer(0)]],
                                 constant Uniforms &uniforms [[buffer(1)]])
{
    VertexOut out;
    device const Vertex &v = vertices[vertexID];
    float3 pos = float3(v.position);
    float3 norm = float3(v.normal);

    out.vFragPos = (uniforms.model * float4(pos, 1.0)).xyz;
    out.vNormal = (uniforms.model * float4(norm, 0.0)).xyz;
    out.position = uniforms.projection * uniforms.view * float4(out.vFragPos, 1.0);
    return out;
}

fragment float4 fragment_main(VertexOut in [[stage_in]],
                                    constant float4 &color [[buffer(0)]],
                                    constant float4 &lightPos [[buffer(2)]],
                                    constant float4 &viewPos [[buffer(3)]])
{
    float3 norm = normalize(in.vNormal);
    float3 lightDir = normalize(lightPos.xyz - in.vFragPos);
    
    // Diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    
    // Fill light from opposite side
    float3 fillLightDir = normalize(float3(-lightPos.x, lightPos.y * 0.2, -lightPos.z));
    float fillDiff = max(dot(norm, fillLightDir), 0.0) * 0.3;
    
    // Fake directional shadow / Ambient Occlusion based on normal pointing down
    float groundShadow = clamp(norm.y * 0.5 + 0.6, 0.2, 1.0);
    float3 ambient = 0.2 * color.rgb * groundShadow;
    
    float3 diffuse = (diff + fillDiff) * color.rgb;
    
    // Specular
    float specularStrength = 0.2;
    float3 viewDir = normalize(viewPos.xyz - in.vFragPos);
    float3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
    float3 specular = specularStrength * spec * float3(1.0);
    
    // Cavity / Edge detection for "natural border" effect using derivatives
    float3 dnx = dfdx(norm);
    float3 dny = dfdy(norm);
    float edge = length(dnx) + length(dny);
    float cavity = 1.0 - smoothstep(0.1, 0.4, edge); // darken edges

    float3 result = (ambient + diffuse + specular) * cavity;
    
    return float4(result, color.a);
}
