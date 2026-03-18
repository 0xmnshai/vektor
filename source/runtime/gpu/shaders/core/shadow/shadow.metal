#include <metal_stdlib>
using namespace metal;

struct Vertex {
    packed_float3 position;
    packed_float3 normal;
};

struct Uniforms {
    float4x4 model;
    float4x4 lightSpaceMatrix;
};

vertex float4 vertex_main(uint vertexID [[vertex_id]],
                          const device Vertex* vertices [[buffer(0)]],
                          constant Uniforms &uniforms [[buffer(1)]])
{
    device const Vertex &v = vertices[vertexID];
    return uniforms.lightSpaceMatrix * uniforms.model * float4(float3(v.position), 1.0);
}

fragment void fragment_main()
{
    // Shadow map only needs depth
}
