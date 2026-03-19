#include <metal_stdlib>
using namespace metal;

struct Uniforms {
    float4x4 projection;
    float4x4 view;
    float4x4 viewInverse;
    float4x4 projInverse;
};

struct VertexOutput {
    float4 position [[position]];
    float3 nearPoint;
    float3 farPoint;
};

struct FragmentOutput {
    float4 color [[color(0)]];
    float depth [[depth(any)]];
};

float3 unprojectPoint(float x, float y, float z, float4x4 viewInv, float4x4 projInv) {
    float4 unprojectedPoint = viewInv * projInv * float4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

vertex VertexOutput vertex_main(uint vertexID [[vertex_id]],
                               const device packed_float3* vertices [[buffer(0)]],
                               constant Uniforms &uniforms [[buffer(1)]]) {
    VertexOutput out;
    float3 pos = float3(vertices[vertexID]);
    // Use GL NDC Z range [-1, 1] for unprojection since we are using GL matrices
    out.nearPoint = unprojectPoint(pos.x, pos.y, -1.0, uniforms.viewInverse, uniforms.projInverse);
    out.farPoint = unprojectPoint(pos.x, pos.y, 1.0, uniforms.viewInverse, uniforms.projInverse);
    out.position = float4(pos, 1.0);
    return out;
}

float computeDepth(float3 pos, float4x4 projection, float4x4 view) {
    float4 clip_space_pos = projection * view * float4(pos, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}

float4 grid(float3 fragPos3D, float scale, bool drawAxis) {
    float2 coord = fragPos3D.xz * scale;
    float2 derivative = fwidth(coord);
    // Add epsilon to avoid division by zero
    float2 g = abs(fract(coord - 0.5) - 0.5) / (derivative + 0.0001);
    float line = min(g.x, g.y);
    float minimumz = min(derivative.y, 1.0);
    float minimumx = min(derivative.x, 1.0);

    float4 color = float4(0.4, 0.4, 0.4, 1.0 - min(line, 1.0));

    if (drawAxis) {
        float fwx = fwidth(fragPos3D.x);
        float fwz = fwidth(fragPos3D.z);
        if (abs(fragPos3D.x) < fwx)
            color = float4(0.1, 0.9, 0.1, 1.0 - min(line, 1.0)); // Z axis (Green)
        if (abs(fragPos3D.z) < fwz)
            color = float4(0.9, 0.1, 0.1, 1.0 - min(line, 1.0)); // X axis (Red)
    }

    return color;
}

float computeLinearDepth(float3 pos, float4x4 projection, float4x4 view) {
    float4 clip_space_pos = projection * view * float4(pos, 1.0);
    float clip_depth = (clip_space_pos.z / clip_space_pos.w);
    float near = 0.01;
    float far = 1000.0;
    float linearDepth = (2.0 * near * far) / (far + near - clip_depth * (far - near));
    return linearDepth / 100.0;
}

fragment FragmentOutput fragment_main(VertexOutput in [[stage_in]],
                             constant Uniforms &uniforms [[buffer(1)]]) {
    float t = -in.nearPoint.y / (in.farPoint.y - in.nearPoint.y);
    if (t <= 0.0001) discard_fragment();

    float3 fragPos3D = in.nearPoint + t * (in.farPoint - in.nearPoint);
    
    float linearDepth = computeLinearDepth(fragPos3D, uniforms.projection, uniforms.view);
    float fading = exp(-linearDepth * 10.0);

    float4 color1 = grid(fragPos3D, 1.0, true);
    float4 color10 = grid(fragPos3D, 0.1, true);

    float4 outColor = color1;
    if (color10.a > 0.1) outColor = color10;
    outColor.a = max(color1.a, color10.a);
    outColor.a *= fading;
    
    FragmentOutput out;
    out.color = outColor;
    float depth = computeDepth(fragPos3D, uniforms.projection, uniforms.view);
    // Map GL NDC depth [-1, 1] to Metal [0, 1]
    out.depth = (depth + 1.0) * 0.5;
    return out;
}
