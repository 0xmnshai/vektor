#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../RNA_camera.h"

namespace vektor::rna {

glm::mat4 Camera::view_matrix() const
{
  glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -camera_dna_.distance));
  view = glm::rotate(view, glm::radians(camera_dna_.rotation_x), glm::vec3(1.0f, 0.0f, 0.0f));
  view = glm::rotate(view, glm::radians(camera_dna_.rotation_y), glm::vec3(0.0f, 1.0f, 0.0f));
  view = glm::translate(view, -camera_dna_.pivot);
  return view;
}

glm::vec3 Camera::eye_position() const
{
  glm::mat4 view = view_matrix();
  glm::mat4 inv_view = glm::inverse(view);
  return inv_view[3];
}

glm::mat4 Camera::projection_matrix(float aspect) const
{
  return glm::perspective(
      glm::radians(camera_dna_.fov), aspect, camera_dna_.near_plane, camera_dna_.far_plane);
}

void Camera::orbit(float dx, float dy)
{
  float sensitivity = 1.0f;  // camera_dna_.distance / 2.0f;
  if (sensitivity < 0.1f)
    sensitivity = 0.1f;
  if (sensitivity > 2.0f)
    sensitivity = 2.0f;
  camera_dna_.rotation_y += (/*0.5 * */ sensitivity) * dx;
  camera_dna_.rotation_x += (/*0.5 * */ sensitivity) * dy;
}

void Camera::pan(float dx, float dy)
{
  float pan_speed = camera_dna_.distance * 0.0015f;
  glm::mat4 rot = glm::rotate(
      glm::mat4(1.0f), glm::radians(camera_dna_.rotation_y), glm::vec3(0.0f, 1.0f, 0.0f));
  rot = glm::rotate(rot, glm::radians(camera_dna_.rotation_x), glm::vec3(1.0f, 0.0f, 0.0f));

  auto right = glm::vec3(rot[0]);
  auto up = glm::vec3(rot[1]);

  camera_dna_.pivot -= right * dx * pan_speed;
  camera_dna_.pivot += up * dy * pan_speed;
}

void Camera::zoom(float dy)
{
  camera_dna_.distance += dy * 0.05f;
  if (camera_dna_.distance < 0.1f)
    camera_dna_.distance = 0.1f;
}

void Camera::fly(float dx, float dy)
{
  camera_dna_.rotation_y += dx * 0.2f;
  camera_dna_.rotation_x += dy * 0.2f;
}

void Camera::screen_to_ray(
    float x, float y, int width, int height, glm::vec3 &out_origin, glm::vec3 &out_dir) const
{
  float nx = (2.0f * x) / (float)width - 1.0f;
  float ny = 1.0f - (2.0f * y) / (float)height;

  glm::mat4 projection = projection_matrix((float)width / (float)height);
  glm::mat4 view = view_matrix();

  glm::mat4 inv_vp = glm::inverse(projection * view);
  glm::vec4 near_point = inv_vp * glm::vec4(nx, ny, -1.0f, 1.0f);
  glm::vec4 far_point = inv_vp * glm::vec4(nx, ny, 1.0f, 1.0f);

  out_origin = glm::vec3(near_point) / near_point.w;
  out_dir = glm::normalize((glm::vec3(far_point) / far_point.w) - out_origin);
}

glm::vec3 Camera::get_world_point_on_pivot_plane(float x, float y, int width, int height) const
{

  glm::vec3 ray_origin, ray_dir;
  screen_to_ray(x, y, width, height, ray_origin, ray_dir);

  // Plane normal is view direction
  glm::mat4 rot = glm::rotate(
      glm::mat4(1.0f), -glm::radians(camera_dna_.rotation_y), glm::vec3(0.0f, 1.0f, 0.0f));
  rot = glm::rotate(rot, -glm::radians(camera_dna_.rotation_x), glm::vec3(1.0f, 0.0f, 0.0f));
  glm::vec3 view_dir = -glm::vec3(rot[2]);

  float denom = ray_dir.x * view_dir.x + ray_dir.y * view_dir.y + ray_dir.z * view_dir.z;
  if (std::abs(denom) > 1e-6) {
    float t = ((camera_dna_.pivot.x - ray_origin.x) * view_dir.x +
               (camera_dna_.pivot.y - ray_origin.y) * view_dir.y +
               (camera_dna_.pivot.z - ray_origin.z) * view_dir.z) /
              denom;
    return ray_origin + t * ray_dir;
  }
  return camera_dna_.pivot;
}

void Camera::move(const glm::vec3 &forward,
                  const glm::vec3 &right,
                  const glm::vec3 &up,
                  bool w,
                  bool s,
                  bool a,
                  bool d,
                  bool q,
                  bool e,
                  float speed)
{
  if (w)
    camera_dna_.pivot += forward * speed;
  if (s)
    camera_dna_.pivot -= forward * speed;
  if (a)
    camera_dna_.pivot -= right * speed;
  if (d)
    camera_dna_.pivot += right * speed;
  if (q)
    camera_dna_.pivot -= up * speed;
  if (e)
    camera_dna_.pivot += up * speed;
}

glm::vec3 Camera::get_forward_vector() const
{
  glm::mat4 rot = glm::rotate(
      glm::mat4(1.0f), -glm::radians(camera_dna_.rotation_y), glm::vec3(0.0f, 1.0f, 0.0f));
  rot = glm::rotate(rot, -glm::radians(camera_dna_.rotation_x), glm::vec3(1.0f, 0.0f, 0.0f));
  return -glm::vec3(rot[2]);
}

glm::vec3 Camera::get_right_vector() const
{
  glm::mat4 rot = glm::rotate(
      glm::mat4(1.0f), -glm::radians(camera_dna_.rotation_y), glm::vec3(0.0f, 1.0f, 0.0f));
  rot = glm::rotate(rot, -glm::radians(camera_dna_.rotation_x), glm::vec3(1.0f, 0.0f, 0.0f));
  return glm::vec3(rot[0]);
}

glm::vec3 Camera::get_up_vector() const
{
  glm::mat4 rot = glm::rotate(
      glm::mat4(1.0f), -glm::radians(camera_dna_.rotation_y), glm::vec3(0.0f, 1.0f, 0.0f));
  rot = glm::rotate(rot, -glm::radians(camera_dna_.rotation_x), glm::vec3(1.0f, 0.0f, 0.0f));
  return glm::vec3(rot[1]);
}
}  // namespace vektor::rna
