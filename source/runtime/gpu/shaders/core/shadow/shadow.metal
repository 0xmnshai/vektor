#include <metal_stdlib>
using namespace metal;

struct VertexInput {
    packed_float3 position;
    packed_float3 normal;
};

struct Uniforms {
    float4x4 model;
    float4x4 lightSpaceMatrix;
};

struct VertexOut {
    float4 position [[position]];
};

vertex VertexOut vertex_main(uint vertexID [[vertex_id]],
                             const device VertexInput* vertices [[buffer(0)]],
                             constant Uniforms &uniforms [[buffer(1)]])
{
    VertexOut out;
    float3 pos = float3(vertices[vertexID].position);
    out.position = uniforms.lightSpaceMatrix * uniforms.model * float4(pos, 1.0);
    // Remap OpenGL Z [-1, 1] to Metal [0, 1] for depth
    out.position.z = (out.position.z + out.position.w) * 0.5;
    return out;
}

// Fragment shader not strictly needed for depth-only pass, 
// but some versions of Metal might prefer a dummy one.
fragment void fragment_main() {
    // Empty
}
