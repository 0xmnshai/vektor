#pragma once

#include "../dna/DNA_light.h"

namespace vektor::rna {

class Light {
 public:
  Light() = default;
  ~Light() = default;

  inline void set_light_dna(const dna::DNA_Light &light_dna)
  {
    light_dna_ = light_dna;
  }

  [[nodiscard]] inline dna::DNA_Light light_dna() const
  {
    return light_dna_;
  }

  [[nodiscard]] inline dna::DNA_LightType type() const
  {
    return light_dna_.type;
  }
  inline void set_type(dna::DNA_LightType type)
  {
    light_dna_.type = type;
  }

  [[nodiscard]] inline glm::vec3 color() const
  {
    return light_dna_.color;
  }
  inline void set_color(const glm::vec3 &color)
  {
    light_dna_.color = color;
  }

  [[nodiscard]] inline float energy() const
  {
    return light_dna_.energy;
  }
  inline void set_energy(float energy)
  {
    light_dna_.energy = energy;
  }

  [[nodiscard]] inline float temperature() const
  {
    return light_dna_.temperature;
  }
  inline void set_temperature(float temperature)
  {
    light_dna_.temperature = temperature;
  }

  [[nodiscard]] inline float specular_factor() const
  {
    return light_dna_.specular_factor;
  }
  inline void set_specular_factor(float factor)
  {
    light_dna_.specular_factor = factor;
  }

  [[nodiscard]] inline float diffuse_factor() const
  {
    return light_dna_.diffuse_factor;
  }
  inline void set_diffuse_factor(float factor)
  {
    light_dna_.diffuse_factor = factor;
  }

  [[nodiscard]] inline float distance() const
  {
    return light_dna_.distance;
  }
  inline void set_distance(float distance)
  {
    light_dna_.distance = distance;
  }

 protected:
  dna::DNA_Light light_dna_;
};

}  // namespace vektor::rna
