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
    out.vFragPos = (uniforms.model * float4(float3(v.position), 1.0)).xyz;
    out.vNormal = (uniforms.model * float4(float3(v.normal), 0.0)).xyz;
    out.position = uniforms.projection * uniforms.view * float4(out.vFragPos, 1.0);
    return out;
}
fragment float4 fragment_main(VertexOut in [[stage_in]],
                              constant float4 &color [[buffer(0)]])
{
    float3 lightDir = normalize(float3(0.5, 1.0, 0.5));
    float3 norm = normalize(in.vNormal);
    float diff = max(dot(norm, lightDir), 0.0);
    
    // Contact shadow / AO based on world position Y
    float heightAO = clamp(in.vFragPos.y * 0.4 + 0.6, 0.3, 1.0);
    float upFactor = clamp(norm.y * 0.5 + 0.5, 0.6, 1.0);
    float ao = heightAO * upFactor;

    float3 ambient = 0.3 * color.rgb * ao;
    float3 diffuse = diff * color.rgb;

    float3 viewDir = normalize(float3(0, 5, 5) - in.vFragPos);
    float3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    float3 specular = 0.05 * spec * float3(1.0);
    
    return float4(ambient + diffuse + specular, color.a);
}