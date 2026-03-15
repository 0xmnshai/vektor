#pragma once

#include "../dna/DNA_camera.h"

namespace vektor::rna {
class Camera {
 public:
  Camera() = default;
  ~Camera() = default;

  inline void set_camera_dna(const dna::DNA_Camera &camera_dna)
  {
    camera_dna_ = camera_dna;
  }

  [[nodiscard]] inline dna::DNA_Camera camera_dna() const
  {
    return camera_dna_;
  }

  inline void set_camera_type(const dna::DNA_CameraType &type)
  {
    camera_dna_.type = type;
  }

  [[nodiscard]] inline dna::DNA_CameraType camera_type() const
  {
    return camera_dna_.type;
  }

  [[nodiscard]] inline glm::vec3 pivot() const
  {
    return camera_dna_.pivot;
  }
  inline void set_pivot(const glm::vec3 &p)
  {
    camera_dna_.pivot = p;
  }

  [[nodiscard]] inline float distance() const
  {
    return camera_dna_.distance;
  }
  inline void set_distance(float d)
  {
    camera_dna_.distance = std::max(0.1f, d);
  }

  [[nodiscard]] inline float rotation_x() const
  {
    return camera_dna_.rotation_x;
  }
  inline void set_rotation_x(float x)
  {
    camera_dna_.rotation_x = x;
  }

  [[nodiscard]] inline float rotation_y() const
  {
    return camera_dna_.rotation_y;
  }

  inline void set_rotation_y(float y)
  {
    camera_dna_.rotation_y = y;
  }

  [[nodiscard]] glm::mat4 view_matrix() const;
  [[nodiscard]] glm::mat4 projection_matrix(float aspect) const;
  void orbit(float dx, float dy);
  void pan(float dx, float dy);
  void zoom(float dy);
  void fly(float dx, float dy);
  void screen_to_ray(
      float x, float y, int width, int height, glm::vec3 &out_origin, glm::vec3 &out_dir) const;

  [[nodiscard]] glm::vec3 get_world_point_on_pivot_plane(float x,
                                                         float y,
                                                         int width,
                                                         int height) const;

  void move(const glm::vec3 &forward,
            const glm::vec3 &right,
            const glm::vec3 &up,
            bool w,
            bool s,
            bool a,
            bool d,
            bool q,
            bool e,
            float speed);

  [[nodiscard]] glm::vec3 get_forward_vector() const;
  [[nodiscard]] glm::vec3 get_right_vector() const;
  [[nodiscard]] glm::vec3 get_up_vector() const;

 protected:
  dna::DNA_Camera camera_dna_;
};
}  // namespace vektor::rna
