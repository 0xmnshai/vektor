#include <metal_stdlib>
using namespace metal;

struct VertexIn {
    float3 position [[attribute(0)]];
    float3 normal   [[attribute(1)]];
};

struct Uniforms {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
    float2 u_viewportSize;
    float u_time;
    float offset;
};

struct VertexOut {
    float4 position [[position]];
};

vertex VertexOut vertex_main(VertexIn in [[stage_in]],
                               constant Uniforms &uniforms [[buffer(1)]]) {
    VertexOut out;

    // 1. Transform to clip space
    float4 pos_clip = uniforms.projection * uniforms.view * uniforms.model * float4(in.position, 1.0);
    
    // 2. Hybrid Expansion Direction
    float3 local_dir = normalize(in.normal + normalize(in.position) * 0.2);
    
    // Transform to view space direction
    float3x3 normal_matrix = float3x3(uniforms.view[0].xyz, uniforms.view[1].xyz, uniforms.view[2].xyz) * 
                             float3x3(uniforms.model[0].xyz, uniforms.model[1].xyz, uniforms.model[2].xyz);
    float3 normal_view = normal_matrix * local_dir;
    
    float2 normal_clip = (uniforms.projection * float4(normal_view, 0.0)).xy;
    
    // 3. Constant thickness (4 pixels)
    float thickness = 4.0;
    if (length(normal_clip) > 0.0001) {
        float2 screen_offset = normalize(normal_clip) * (thickness / uniforms.u_viewportSize) * 2.0 * pos_clip.w;
        pos_clip.xy += screen_offset;
    }

    // 4. Z-bias
    pos_clip.z -= 0.0005 * pos_clip.w;

    out.position = pos_clip;
    return out;
}

fragment float4 fragment_main(constant Uniforms &uniforms [[buffer(1)]]) {
    // High-quality Pulse effect
    float pulse = 0.5 + 0.5 * sin(uniforms.u_time * 5.0);
    float3 base_color = float3(1.0, 0.5, 0.0); // Orange
    float3 glow_color = float3(1.0, 0.8, 0.3); // Yellowish-white
    
    float3 final_color = mix(base_color, glow_color, pulse * 0.4);
    return float4(final_color, 1.0);
}
