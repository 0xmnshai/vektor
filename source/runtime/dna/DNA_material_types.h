#pragma once

#include <glm/glm.hpp>

#include "DNA_linkbase.h"

namespace vektor::dna {

enum ePreviewType {
  MA_FLAT = 0,
  MA_SPHERE = 1,
  MA_CUBE = 2,
  MA_SHADERBALL = 3,
  MA_SPHERE_A = 4, /* Used for icon renders only. */
  MA_TEXTURE = 5,
  MA_LAMP = 6,
  MA_SKY = 7,
  MA_HAIR = 10,
  MA_ATMOS = 11,
  MA_CLOTH = 12,
  MA_FLUID = 13,
};

/** #Material::pr_flag */
enum {
  MA_PREVIEW_WORLD = 1 << 0,
};

/** #Material::surface_render_method */
enum {
  MA_SURFACE_METHOD_DEFERRED = 0,
  MA_SURFACE_METHOD_FORWARD = 1,
};

/** #Material::volume_intersection_method */
enum {
  MA_VOLUME_ISECT_FAST = 0,
  MA_VOLUME_ISECT_ACCURATE = 1,
};

/** #Material::blend_method */
enum {
  MA_BM_SOLID = 0,
  MA_BM_CLIP = 3,
  MA_BM_HASHED = 4,
  MA_BM_BLEND = 5,
};

/** #Material::displacement_method */
enum {
  MA_DISPLACEMENT_BUMP = 0,
  MA_DISPLACEMENT_DISPLACE = 1,
  MA_DISPLACEMENT_BOTH = 2,
};

/** #Material::thickness_mode */
enum {
  MA_THICKNESS_SPHERE = 0,
  MA_THICKNESS_SLAB = 1,
};

/* Grease Pencil Stroke styles */
enum {
  GP_MATERIAL_STROKE_STYLE_SOLID = 0,
  GP_MATERIAL_STROKE_STYLE_TEXTURE = 1,
};

/* Grease Pencil Fill styles */
enum {
  GP_MATERIAL_FILL_STYLE_SOLID = 0,
  GP_MATERIAL_FILL_STYLE_GRADIENT = 1,
  GP_MATERIAL_FILL_STYLE_CHECKER = 2, /* DEPRECATED (only for convert old files) */
  GP_MATERIAL_FILL_STYLE_TEXTURE = 3,
};

/* Grease Pencil Gradient Types */
enum {
  GP_MATERIAL_GRADIENT_LINEAR = 0,
  GP_MATERIAL_GRADIENT_RADIAL = 1,
};

typedef struct MaterialGPencilStyle {
  /** Color for paint and strokes (alpha included). */
  float stroke_rgba[4] = {};
  /** Color that should be used for drawing "fills" for strokes (alpha included). */
  float fill_rgba[4] = {};
  /** Secondary color used for gradients and other stuff. */
  float mix_rgba[4] = {};
  /** Settings. */
  short flag = 0;
  /** Custom index for passes. */
  short index = 0;
  /** Style for drawing strokes (used to select shader type). */
  short stroke_style = 0;
  /** Style for filling areas (used to select shader type). */
  short fill_style = 0;
  /** Factor used to define shader behavior (several uses). */
  float mix_factor = 0;

  /** Angle used for texture orientation. */
  float texture_angle = 0;

  /** Drawing mode (line or dots). */
  int mode = 0;

  /** Type of gradient. */
  int gradient_type = 0;

} MaterialGPencilStyle;

typedef struct Color {
  float r = 1.0f;
  float g = 1.0f;
  float b = 1.0f;
  float a = 1.0f;

  Color() = default;
  Color(float r, float g, float b) : r(r), g(g), b(b), a(1.0f) {}
  Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

  // return this color struct as glm::vec4
  [[nodiscard]] glm::vec4 to_vec4() const
  {
    return {r, g, b, a};
  }
} Color;

typedef struct Material {
  char id_name[64];
  char description[256];
  char name[64];

  // color
  Color color = {1.0, 1.0, 1.0, 1.0};

  float roughness = 0.4f;
  float metallic = 0;

  char pr_type = MA_SPHERE;
  short pr_texture = 0;
  short pr_flag = 0;

  float alpha_threshold = 0.5f;
  float refract_depth = 0;
  char surface_render_method = MA_SURFACE_METHOD_DEFERRED;
  char volume_intersection_method = MA_VOLUME_ISECT_FAST;
  char blend_method = MA_BM_SOLID;
  char displacement_method = MA_DISPLACEMENT_BUMP;
  char thickness_mode = MA_THICKNESS_SPHERE;

  /** Runtime cache for GLSL materials. */
  std::vector<LinkData> gpu_material;

  struct MaterialGPencilStyle *gp_style = nullptr;
} Material;
}  // namespace vektor::dna
