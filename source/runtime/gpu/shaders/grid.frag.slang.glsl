#version 410 core
layout(column_major) uniform;
layout(column_major) buffer;

#line 90 0
struct GlobalParams_0
{
    mat4x4 view;
    mat4x4 projection;
    mat4x4 viewInverse;
    mat4x4 projInverse;
};


#line 90

layout(std140) uniform block_GlobalParams
{
    mat4x4 view;
    mat4x4 projection;
    mat4x4 viewInverse;
    mat4x4 projInverse;
}globalParams_0;

#line 35 1
float computeDepth_0(vec3 pos_0)
{
    vec4 clip_space_pos_0 = ((((((vec4(pos_0.xyz, 1.0)) * (view)))) * (projection)));
    return clip_space_pos_0.z / clip_space_pos_0.w;
}

float computeLinearDepth_0(vec3 pos_1)
{
    vec4 clip_space_pos_1 = ((((((vec4(pos_1.xyz, 1.0)) * (view)))) * (projection)));


    return 20.0 / (1000.010009765625 - (clip_space_pos_1.z / clip_space_pos_1.w * 2.0 - 1.0) * 999.989990234375) / 100.0;
}


#line 14
vec4 grid_0(vec3 fragPos3D_0, float scale_0, bool drawAxis_0)
{
    vec2 coord_0 = fragPos3D_0.xz * scale_0;
    vec2 derivative_0 = (fwidth((coord_0)));
    vec2 g_0 = abs(fract(coord_0 - 0.5) - 0.5) / derivative_0;

    float _S1 = min(derivative_0.y, 1.0);
    float _S2 = min(derivative_0.x, 1.0);

    float _S3 = 1.0 - min(min(g_0.x, g_0.y), 1.0);

#line 23
    vec4 _S4 = vec4(0.40000000596046448, 0.40000000596046448, 0.40000000596046448, _S3);

#line 23
    vec4 color_0;

    if(drawAxis_0)
    {

#line 26
        if((abs(fragPos3D_0.x)) < (0.10000000149011612 * _S2))
        {

#line 26
            color_0 = vec4(0.10000000149011612, 0.10000000149011612, 0.89999997615814209, _S3);

#line 26
        }
        else
        {

#line 26
            color_0 = _S4;

#line 26
        }

        if((abs(fragPos3D_0.z)) < (0.10000000149011612 * _S1))
        {

#line 28
            color_0 = vec4(0.89999997615814209, 0.10000000149011612, 0.10000000149011612, _S3);

#line 28
        }

#line 25
    }
    else
    {

#line 25
        color_0 = _S4;

#line 25
    }

#line 32
    return color_0;
}


#line 12349 2
layout(location = 0)
out vec4 entryPointParam_main_0;


#line 12349
layout(location = 0)
in vec3 input_nearPoint_0;


#line 12349
layout(location = 1)
in vec3 input_farPoint_0;


#line 50 1
void main()
{
    float t_0 = - input_nearPoint_0.y / (input_farPoint_0.y - input_nearPoint_0.y);
    if(t_0 <= 0.0)
    {

#line 54
        discard;

#line 53
    }


    vec3 fragPos3D_1 = input_nearPoint_0 + t_0 * (input_farPoint_0 - input_nearPoint_0);
    float _S5 = (computeDepth_0(fragPos3D_1) + 1.0) / 2.0;


    float fading_0 = exp(- computeLinearDepth_0(fragPos3D_1) * 10.0);

    vec4 color1_0 = grid_0(fragPos3D_1, 1.0, true);
    vec4 color10_0 = grid_0(fragPos3D_1, 0.10000000149011612, true);

    vec4 outColor_0 = color1_0;
    float _S6 = color10_0.w;

#line 66
    if(_S6 > 0.10000000149011612)
    {

#line 67
        outColor_0 = color10_0;

#line 66
    }


    outColor_0[3] = max(color1_0.w, _S6) * fading_0;

#line 69
    entryPointParam_main_0 = outColor_0;

#line 69
    gl_FragDepth = _S5;

#line 69
    return;
}

