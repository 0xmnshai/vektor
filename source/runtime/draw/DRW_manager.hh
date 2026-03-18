// Forward declare Scene to avoid circular dependencies if any
#include "../dna/DNA_scene_types.h"
#include "../gpu/GPU_mesh.h"

#include <glm/glm.hpp>

namespace vektor::draw {

struct ObjectDrawData {
  vektor::gpu::GPUMesh *gpu_mesh = nullptr;
  // Further details like transform buffers could go here
};

void DRW_draw_view(vektor::dna::Scene *scene, void *encoder_or_context, const glm::mat4 &view, const glm::mat4 &projection);

} // namespace vektor::draw
