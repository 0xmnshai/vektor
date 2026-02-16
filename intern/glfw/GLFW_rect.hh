
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

    virtual inline int32_t getWidth() const;

    virtual inline int32_t getHeight() const;

    virtual inline void    set(int32_t l,
                               int32_t t,
                               int32_t r,
                               int32_t b);

    virtual inline bool    isEmpty() const;

    virtual inline bool    isValid() const;

    virtual void           inset(int32_t i);

    virtual inline void    unionRect(const GLFW_Rect& r);

    virtual inline void    unionPoint(int32_t x,
                                      int32_t y);

    virtual inline void    clampPoint(int32_t& x,
                                      int32_t& y);

    virtual inline bool    isInside(int32_t x,
                                    int32_t y) const;

    virtual void           setCenter(int32_t cx,
                                     int32_t cy);

    virtual void           setCenter(int32_t cx,
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

inline int32_t GLFW_Rect::getWidth() const
{
    return r_ - l_;
}

inline int32_t GLFW_Rect::getHeight() const
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

inline bool GLFW_Rect::isEmpty() const
{
    return (getWidth() == 0) || (getHeight() == 0);
}

inline bool GLFW_Rect::isValid() const
{
    return (l_ <= r_) && (t_ <= b_);
}

inline void GLFW_Rect::unionRect(const GLFW_Rect& r)
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

inline void GLFW_Rect::unionPoint(int32_t x,
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

inline void GLFW_Rect::clampPoint(int32_t& x,
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

inline bool GLFW_Rect::isInside(int32_t x,
                                int32_t y) const
{
    return (x >= l_) && (x <= r_) && (y >= t_) && (y <= b_);
}
} // namespace vektor