#version 410 core

layout(location = 0) out vec4 FragColor;

in vec3 nearPoint;
in vec3 farPoint;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 viewInverse;
uniform mat4 projInverse;

vec4 grid(vec3 fragPos3D, float scale, bool drawAxis)
{
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 g = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(g.x, g.y);
    float minimumz = min(derivative.y, 1.0);
    float minimumx = min(derivative.x, 1.0);

    vec4 color = vec4(0.4, 0.4, 0.4, 1.0 - min(line, 1.0));

    if (drawAxis) {
        float fwx = fwidth(fragPos3D.x);
        float fwz = fwidth(fragPos3D.z);
        if (abs(fragPos3D.x) < fwx)
            color = vec4(0.1, 0.9, 0.1, 1.0 - min(line, 1.0));  // Z axis (Green)
        if (abs(fragPos3D.z) < fwz)
            color = vec4(0.9, 0.1, 0.1, 1.0 - min(line, 1.0));  // X axis (Red)
    }

    return color;
}

float computeDepth(vec3 pos)
{
    vec4 clip_space_pos = projection * view * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}

float computeLinearDepth(vec3 pos)
{
    vec4 clip_space_pos = projection * view * vec4(pos.xyz, 1.0);
    float clip_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0;
    float linearDepth = (2.0 * 0.01 * 1000.0) / (1000.0 + 0.01 - clip_depth * (1000.0 - 0.01));
    return linearDepth / 100.0;
}

void main()
{
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    if (t <= 0)
        discard;

    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);
    gl_FragDepth = ((computeDepth(fragPos3D) + 1.0) / 2.0);

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = exp(-linearDepth * 10.0);

    vec4 color1 = grid(fragPos3D, 1.0, true);
    vec4 color10 = grid(fragPos3D, 0.1, true);

    vec4 outColor = color1;
    if (color10.a > 0.1)
        outColor = color10;
    outColor.a = max(color1.a, color10.a);
    outColor.a *= fading;
    FragColor = outColor;
}
