
#pragma once

#include <cstdint>

#include "../gaurdalloc/MEM_gaurdalloc.hh"

namespace vektor
{
class GLFW_Rect
{
public:
    GLFW_Rect(int32_t l = 0,
              int32_t t = 0,
              int32_t r = 0,
              int32_t b = 0)
        : l_(l)
        , t_(t)
        , r_(r)
        , b_(b)
    {
    }

    virtual ~GLFW_Rect() = default;

    virtual inline int32_t get_width() const;

    virtual inline int32_t get_height() const;

    virtual inline void    set(int32_t l,
                               int32_t t,
                               int32_t r,
                               int32_t b);

    virtual inline bool    is_empty() const;

    virtual inline bool    is_valid() const;

    virtual void           inset(int32_t i);

    virtual inline void    union_rect(const GLFW_Rect& r);

    virtual inline void    union_point(int32_t x,
                                       int32_t y);

    virtual inline void    clamp_point(int32_t& x,
                                       int32_t& y);

    virtual inline bool    is_inside(int32_t x,
                                     int32_t y) const;

    virtual void           set_center(int32_t cx,
                                      int32_t cy);

    virtual void           set_center(int32_t cx,
                                      int32_t cy,
                                      int32_t w,
                                      int32_t h);

    virtual bool           clip(GLFW_Rect& r) const;

    /** Left coordinate of the rectangle */
    int32_t                l_;
    /** Top coordinate of the rectangle */
    int32_t                t_;
    /** Right coordinate of the rectangle */
    int32_t                r_;
    /** Bottom coordinate of the rectangle */
    int32_t                b_;

    MEM_CXX_CLASS_ALLOC_FUNCS("GLFW:GLFW_Rect")
};

inline int32_t GLFW_Rect::get_width() const
{
    return r_ - l_;
}

inline int32_t GLFW_Rect::get_height() const
{
    return b_ - t_;
}

inline void GLFW_Rect::set(int32_t l,
                           int32_t t,
                           int32_t r,
                           int32_t b)
{
    l_ = l;
    t_ = t;
    r_ = r;
    b_ = b;
}

inline bool GLFW_Rect::is_empty() const
{
    return (get_width() == 0) || (get_height() == 0);
}

inline bool GLFW_Rect::is_valid() const
{
    return (l_ <= r_) && (t_ <= b_);
}

inline void GLFW_Rect::union_rect(const GLFW_Rect& r)
{
    if (r.l_ < l_)
    {
        l_ = r.l_;
    }
    if (r.r_ > r_)
    {
        r_ = r.r_;
    }
    if (r.t_ < t_)
    {
        t_ = r.t_;
    }
    if (r.b_ > b_)
    {
        b_ = r.b_;
    }
}

inline void GLFW_Rect::union_point(int32_t x,
                                  int32_t y)
{
    if (x < l_)
    {
        l_ = x;
    }
    if (x > r_)
    {
        r_ = x;
    }
    if (y < t_)
    {
        t_ = y;
    }
    if (y > b_)
    {
        b_ = y;
    }
}

inline void GLFW_Rect::clamp_point(int32_t& x,
                                  int32_t& y)
{
    if (x < l_)
    {
        x = l_;
    }
    else if (x > r_)
    {
        x = r_;
    }

    if (y < t_)
    {
        y = t_;
    }
    else if (y > b_)
    {
        y = b_;
    }
}

inline bool GLFW_Rect::is_inside(int32_t x,
                                int32_t y) const
{
    return (x >= l_) && (x <= r_) && (y >= t_) && (y <= b_);
}
} // namespace vektor