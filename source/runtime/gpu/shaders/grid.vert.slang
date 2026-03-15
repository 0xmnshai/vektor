#version 410

struct VertexOutput {
    float4 position : SV_Position;
    float3 nearPoint;
    float3 farPoint;
};

// Use top-level uniforms for direct OpenGL mapping
uniform float4x4 view;
uniform float4x4 projection;
uniform float4x4 viewInverse;
uniform float4x4 projInverse;

float3 UnprojectPoint(float x, float y, float z, float4x4 viewInv, float4x4 projInv) {
    float4 unprojectedPoint = mul(viewInv, mul(projInv, float4(x, y, z, 1.0)));
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

[shader("vertex")]
VertexOutput main(float3 aPos : aPos) {
    VertexOutput output;
    output.nearPoint = UnprojectPoint(aPos.x, aPos.y, 0.0, viewInverse, projInverse);
    output.farPoint  = UnprojectPoint(aPos.x, aPos.y, 1.0, viewInverse, projInverse);
    output.position = float4(aPos, 1.0);
    return output;
}
