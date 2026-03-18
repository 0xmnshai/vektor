#include "entt/entt.hpp"
#include <glm/glm.hpp>

#include "../../dna/DNA_object_type.h"
#include "../../rna/RNA_ecs_registry.h"
#include "../../rna/RNA_internal.h"
#include "../../rna/RNA_types.h"
#include "../ECS_registry.h"

#include "../../creator_global.h"
#include "../../gpu/GPU_shader.h"
#include "../../lib/intern/appdir.h"
#include <iostream>
#include <string>

#include "ECS_mesh_primitives.h"

extern "C" {
void outliner_notify_scene_changed();
}

namespace vektor::kernel {
void create_entity(rna::VektorRNA *v_rna,
                   rna::RNAStruct *rna_,
                   const char *name,
                   const char *description,
                   int type,
                   float x,
                   float y,
                   float z,
                   float r,
                   float g,
                   float b)
{
  auto &registry = ECSRegistry::instance();
  auto entity = (entt::entity)rna::RNA_ecs_create_entity();

  auto object = (dna::Object *)rna::RNA_ecs_get_object(&registry, entity);

  object->type = (dna::ObjectType)type;
  strncpy(object->id.name, name, sizeof(object->id.name) - 1);
  object->id.name[sizeof(object->id.name) - 1] = '\0';

  strncpy(object->description, description, sizeof(object->description) - 1);
  object->description[sizeof(object->description) - 1] = '\0';

  object->type = (dna::ObjectType)type;

  object->transform.location = glm::vec3(x, y, z);
  object->transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
  object->transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);

  if (object->type == dna::ObjectType::Mesh) {
    if (std::string(name).find("Cube") != std::string::npos) {
      add_primitive_cube_exec(object, 1.0f);
    }
    else if (std::string(name).find("Plane") != std::string::npos) {
      add_primitive_plane_exec(object, 10.0f);
    }
    else {
      add_primitive_cylinder_exec(object, 1.0f, 2.0f, 32);
    }

    std::cout << "MATERIAL LENGTH IS " << object->mesh->materials.size() << std::endl;

    if (object->mesh && !object->mesh->materials.empty()) {
      object->mesh->materials[0]->color.r = r;
      object->mesh->materials[0]->color.g = g;
      object->mesh->materials[0]->color.b = b;
    }

    vektor::gpu::GPUShaderSourceParameters params = {nullptr};
    if (creator::G.gpu_backend == creator::GPU_BACKEND_METAL) {
      params.vert_entry = "vertex_main";
      params.frag_entry = "fragment_main";
      const char *metal_src = R"(
#include <metal_stdlib>
using namespace metal;
struct Vertex {
    packed_float3 position;
    packed_float3 normal;
};
struct VertexOut {
    float4 position [[position]];
    float3 vNormal;
    float3 vFragPos;
};
struct Uniforms {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};
vertex VertexOut vertex_main(uint vertexID [[vertex_id]],
                             const device Vertex* vertices [[buffer(0)]],
                             constant Uniforms &uniforms [[buffer(1)]])
{
    VertexOut out;
    device const Vertex &v = vertices[vertexID];
    out.vFragPos = (uniforms.model * float4(float3(v.position), 1.0)).xyz;
    out.vNormal = (uniforms.model * float4(float3(v.normal), 0.0)).xyz;
    out.position = uniforms.projection * uniforms.view * float4(out.vFragPos, 1.0);
    return out;
}
fragment float4 fragment_main(VertexOut in [[stage_in]],
                              constant float4 &color [[buffer(0)]])
{
    float3 lightDir = normalize(float3(0.5, 1.0, 0.5));
    float3 norm = normalize(in.vNormal);
    float diff = max(dot(norm, lightDir), 0.0);
    
    // Contact shadow / AO based on world position Y
    float heightAO = clamp(in.vFragPos.y * 0.4 + 0.6, 0.3, 1.0);
    float upFactor = clamp(norm.y * 0.5 + 0.5, 0.6, 1.0);
    float ao = heightAO * upFactor;

    float3 ambient = 0.3 * color.rgb * ao;
    float3 diffuse = diff * color.rgb;

    float3 viewDir = normalize(float3(0, 5, 5) - in.vFragPos);
    float3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    float3 specular = 0.05 * spec * float3(1.0);
    
    return float4(ambient + diffuse + specular, color.a);
}
      )";
      object->shader_program = gpu::GPU_shader_create_from_strings(metal_src, metal_src, &params);
    }
    else {
      const char *vert_src = R"(
#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
      )";
      const char *frag_src = R"(
#version 410 core
out vec4 FragColor;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
uniform vec3 color;
void main() {
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.5));
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    float heightAO = clamp(FragPos.y * 0.4 + 0.6, 0.3, 1.0);
    float upFactor = clamp(norm.y * 0.5 + 0.5, 0.6, 1.0);
    float ao = heightAO * upFactor;
    vec3 ambient = 0.3 * color * ao;
    vec3 diffuse = diff * color;
    vec3 viewDir = normalize(vec3(0, 5, 5) - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = 0.05 * spec * vec3(1.0);
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
      )";
      object->shader_program = gpu::GPU_shader_create_from_strings(vert_src, frag_src, &params);
    }
  }
  else {
    object->shader_program = nullptr;
  }

  // rna::PointerRNA object_ptr = rna::RNA_pointer_from_entity<dna::Object>(
  //     registry.registry(), entity, object_type);

  outliner_notify_scene_changed();
}
}  // namespace vektor::kernel
