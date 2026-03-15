#version 410

struct VertexOutput {
  float4 position : SV_Position;
  float3 nearPoint;
  float3 farPoint;
};

uniform float4x4 view;
uniform float4x4 projection;
uniform float4x4 viewInverse;
uniform float4x4 projInverse;

float4 grid(float3 fragPos3D, float scale, bool drawAxis)
{
  float2 coord = fragPos3D.xz * scale;
  float2 derivative = fwidth(coord);
  float2 g = abs(fract(coord - 0.5) - 0.5) / derivative;
  float line = min(g.x, g.y);
  float minimumz = min(derivative.y, 1.0);
  float minimumx = min(derivative.x, 1.0);

  float4 color = float4(0.4, 0.4, 0.4, 1.0 - min(line, 1.0));

  if (drawAxis) {
    if (abs(fragPos3D.x) < 0.1 * minimumx)
      color = float4(0.1, 0.1, 0.9, 1.0 - min(line, 1.0));  // Z axis (Blue)
    if (abs(fragPos3D.z) < 0.1 * minimumz)
      color = float4(0.9, 0.1, 0.1, 1.0 - min(line, 1.0));  // X axis (Red)
  }

  return color;
}

float computeDepth(float3 pos)
{
  float4 clip_space_pos = mul(projection, mul(view, float4(pos.xyz, 1.0)));
  return (clip_space_pos.z / clip_space_pos.w);
}

float computeLinearDepth(float3 pos)
{
  float4 clip_space_pos = mul(projection, mul(view, float4(pos.xyz, 1.0)));
  float clip_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0;
  float linearDepth = (2.0 * 0.01 * 1000.0) / (1000.0 + 0.01 - clip_depth * (1000.0 - 0.01));
  return linearDepth / 100.0;
}

[shader("fragment")]
float4 main(VertexOutput input, out float depth: SV_Depth) : SV_Target
{
  float t = -input.nearPoint.y / (input.farPoint.y - input.nearPoint.y);
  if (t <= 0)
    discard;

  float3 fragPos3D = input.nearPoint + t * (input.farPoint - input.nearPoint);
  depth = (computeDepth(fragPos3D) + 1.0) / 2.0;

  float linearDepth = computeLinearDepth(fragPos3D);
  float fading = exp(-linearDepth * 10.0);

  float4 color1 = grid(fragPos3D, 1.0, true);
  float4 color10 = grid(fragPos3D, 0.1, true);

  float4 outColor = color1;
  if (color10.a > 0.1)
    outColor = color10;
  outColor.a = max(color1.a, color10.a);
  outColor.a *= fading;
  return outColor;
}


