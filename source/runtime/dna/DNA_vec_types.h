#pragma once

namespace vektor
{

struct vec2s
{
    short x, y;
};

struct vec2f
{
    float x, y;
};

struct vec2i
{
    int x, y;
};

struct vec3i
{
    int x, y, z;
};

struct vec3f
{
    float x, y, z;
};

struct vec4f
{
    float x, y, z, w;
};

struct mat4x4f
{
    float value[4][4];
};

struct rcti
{
    int xmin, xmax;
    int ymin, ymax;

#ifdef __cplusplus
    inline bool operator==(const rcti& other) const
    {
        return xmin == other.xmin && xmax == other.xmax && ymin == other.ymin && ymax == other.ymax;
    }
    inline bool operator!=(const rcti& other) const { return !(*this == other); }
#endif
};

struct rctf
{
    float xmin, xmax;
    float ymin, ymax;
};

struct DualQuat
{
    float quat[4]      = {};
    float trans[4]     = {};

    float scale[4][4]  = {};
    float scale_weight = 0;
};

} // namespace vektor
