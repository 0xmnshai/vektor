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

#line 15 1
vec3 UnprojectPoint_0(float x_0, float y_0, float z_0, mat4x4 viewInv_0, mat4x4 projInv_0)
{

#line 16
    vec4 unprojectedPoint_0 = ((((((vec4(x_0, y_0, z_0, 1.0)) * (projInv_0)))) * (viewInv_0)));
    return unprojectedPoint_0.xyz / unprojectedPoint_0.w;
}


#line 17
layout(location = 0)
out vec3 entryPointParam_main_nearPoint_0;


#line 17
layout(location = 1)
out vec3 entryPointParam_main_farPoint_0;


#line 17
layout(location = 0)
in vec3 aPos_0;


#line 3
struct VertexOutput_0
{
    vec4 position_0;
    vec3 nearPoint_0;
    vec3 farPoint_0;
};


#line 21
void main()
{

#line 22
    VertexOutput_0 output_0;
    output_0.nearPoint_0 = UnprojectPoint_0(aPos_0.x, aPos_0.y, 0.0, viewInverse, projInverse);
    output_0.farPoint_0 = UnprojectPoint_0(aPos_0.x, aPos_0.y, 1.0, viewInverse, projInverse);
    output_0.position_0 = vec4(aPos_0, 1.0);
    VertexOutput_0 _S1 = output_0;

#line 26
    gl_Position = output_0.position_0;

#line 26
    entryPointParam_main_nearPoint_0 = _S1.nearPoint_0;

#line 26
    entryPointParam_main_farPoint_0 = _S1.farPoint_0;

#line 26
    return;
}

