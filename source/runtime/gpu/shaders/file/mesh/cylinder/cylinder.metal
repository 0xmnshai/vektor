#include <metal_stdlib>
using namespace metal;

struct VertexIn {
    float3 position [[attribute(0)]];
    float3 normal [[attribute(1)]];
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

vertex VertexOut cylinder_vertex(VertexIn in [[stage_in]],
                                 constant Uniforms &uniforms [[buffer(1)]])
{
    VertexOut out;
    out.vFragPos = (uniforms.model * float4(in.position, 1.0)).xyz;
    out.vNormal = (uniforms.model * float4(in.normal, 0.0)).xyz;
    out.position = uniforms.projection * uniforms.view * float4(out.vFragPos, 1.0);
    return out;
}

fragment float4 cylinder_fragment(VertexOut in [[stage_in]],
                                   constant float4 &color [[buffer(0)]],
                                   constant float3 &lightPos [[buffer(2)]],
                                   constant float3 &viewPos [[buffer(3)]])
{
    float3 ambient = 0.1 * color.rgb;
    
    float3 norm = normalize(in.vNormal);
    float3 lightDir = normalize(lightPos - in.vFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    float3 diffuse = diff * color.rgb;
    
    float3 viewDir = normalize(viewPos - in.vFragPos);
    float3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    float3 specular = 0.5 * spec * float3(1.0);
    
    return float4(ambient + diffuse + specular, color.a);
}
