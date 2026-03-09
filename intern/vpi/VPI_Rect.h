#pragma once

#include <cstdint>

#include "VPI_Types.h"

class VPI_Rect {
 public:
  explicit VPI_Rect(int32_t l = 0, int32_t t = 0, int32_t r = 0, int32_t b = 0)
      : l_(l), t_(t), r_(r), b_(b)
  {
  }

  virtual ~VPI_Rect() = default;

  [[nodiscard]] virtual inline int32_t get_width() const;

  
  [[nodiscard]] virtual inline int32_t get_height() const;
  
  virtual inline void set(int32_t l, int32_t t, int32_t r, int32_t b);
  
  [[nodiscard]] virtual inline bool is_empty() const;
  
  [[nodiscard]] virtual inline bool is_valid() const;

  virtual void inset(int32_t i);

  [[nodiscard]] virtual inline bool is_inside(int32_t x, int32_t y) const;

  [[nodiscard]] virtual inline VPI_TVisibility get_visibility(VPI_Rect &r) const;

  virtual void set_center(int32_t cx, int32_t cy);

  virtual void set_center(int32_t cx, int32_t cy, int32_t w, int32_t h);

  virtual bool clip(VPI_Rect &r) const;

  int32_t l_;
  int32_t t_;
  int32_t r_;
  int32_t b_;
};

inline int32_t VPI_Rect::get_width() const
{
  return r_ - l_;
}

inline int32_t VPI_Rect::get_height() const
{
  return b_ - t_;
}

inline void VPI_Rect::set(int32_t l, int32_t t, int32_t r, int32_t b)
{
  l_ = l;
  t_ = t;
  r_ = r;
  b_ = b;
}

inline bool VPI_Rect::is_empty() const
{
  return (get_width() == 0) || (get_height() == 0);
}

inline bool VPI_Rect::is_valid() const
{
  return (l_ <= r_) && (t_ <= b_);
}

inline bool VPI_Rect::is_inside(int32_t x, int32_t y) const
{
  return (x >= l_) && (x <= r_) && (y >= t_) && (y <= b_);
}

inline void VPI_Rect::inset(int32_t i)
{
  if (i < 0) {
    // negative inset (grow)
    l_ += i;
    t_ += i;
    r_ -= i;
    b_ -= i;
  }
  else {
    // positive inset (shrink)
    if (l_ + i <= r_ - i) {
      l_ += i;
      r_ -= i;
    }
    if (t_ + i <= b_ - i) {
      t_ += i;
      b_ -= i;
    }
  }
}

inline VPI_TVisibility VPI_Rect::get_visibility(VPI_Rect &r) const
{
  bool p1_inside = is_inside(r.l_, r.t_);
  bool p2_inside = is_inside(r.r_, r.t_);
  bool p3_inside = is_inside(r.r_, r.b_);
  bool p4_inside = is_inside(r.l_, r.b_);

  if (p1_inside && p2_inside && p3_inside && p4_inside) {
    return VPI_kFullyVisible;
  }

  if (r.r_ < l_ || r.l_ > r_ || r.b_ < t_ || r.t_ > b_) {
    return VPI_kNotVisible;
  }

  return VPI_kPartiallyVisible;
}

inline void VPI_Rect::set_center(int32_t cx, int32_t cy)
{
  int32_t w = get_width();
  int32_t h = get_height();
  int32_t half_w = w / 2;
  int32_t half_h = h / 2;

  l_ = cx - half_w;
  r_ = l_ + w;
  t_ = cy - half_h;
  b_ = t_ + h;
}

inline void VPI_Rect::set_center(int32_t cx, int32_t cy, int32_t w, int32_t h)
{
  int32_t half_w = w / 2;
  int32_t half_h = h / 2;

  l_ = cx - half_w;
  r_ = l_ + w;
  t_ = cy - half_h;
  b_ = t_ + h;
}

inline bool VPI_Rect::clip(VPI_Rect &r) const
{
  bool clipped = false;

  if (r.l_ < l_) {
    r.l_ = l_;
    clipped = true;
  }
  if (r.t_ < t_) {
    r.t_ = t_;
    clipped = true;
  }
  if (r.r_ > r_) {
    r.r_ = r_;
    clipped = true;
  }
  if (r.b_ > b_) {
    r.b_ = b_;
    clipped = true;
  }

  // if the rectangle is invalid, make it empty at the left/top
  if (!r.is_valid()) {
    r.l_ = r.r_ = r.t_ = r.b_ = 0;
    clipped = true;
  }

  return clipped;
}
