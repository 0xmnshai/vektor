#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

namespace vektor::gpu {

constexpr static int GPU_VERT_ATTR_MAX_LEN = 16;

enum class VertexAttrType : uint8_t {
  Invalid = 0,
  Float,
  Float2,
  Float3,
  Float4,
  Byte4,
  Byte4N,
  UByte4,
};

typedef enum GPUUsageType {
  GPU_USAGE_STREAM = 0,
  GPU_USAGE_STATIC = 1,
  GPU_USAGE_DYNAMIC = 2,
  GPU_USAGE_DEVICE_ONLY = 3,
  GPU_USAGE_FLAG_BUFFER_TEXTURE_ONLY = 1 << 3,
} GPUUsageType;

typedef struct GPUVertFormat {
  uint32_t attr_len : 5;
  uint32_t name_len : 6;
  uint32_t stride : 11;
  uint32_t packed : 1;
  uint32_t name_offset : 8;
  uint32_t deinterleaved : 1;

  char names[GPU_VERT_ATTR_MAX_LEN];

  void pack();
  uint32_t attribute_add(const char *name, VertexAttrType type, size_t offset = -1);
} GPUVertFormat;

class VertexBufferDelete {};

class VertexBuffer {
 public:
  static size_t memory_usage;
  uint32_t vertex_length;
  uint32_t vertex_alloc;
  GPUVertFormat format;

  explicit VertexBuffer(uint32_t vertex_length, uint32_t vertex_data);
  ~VertexBuffer() = default;

  void allocate(uint32_t vert_len);
  void resize(uint32_t vert_len);
  void upload();

  [[nodiscard]] size_t size_alloc_get() const
  {
    return size_t(this->vertex_alloc) * this->format.stride;
  }

  template<typename FormatT>
  static std::unique_ptr<VertexBuffer, VertexBufferDelete> create(uint32_t vertex_length,
                                                                  uint32_t vertex_data);

  template<typename FormatT>
  static std::unique_ptr<VertexBuffer, VertexBufferDelete> from_size_with_format(
      int size, GPUUsageType usage_type = GPUUsageType::GPU_USAGE_STATIC);
};
}  // namespace vektor::gpu
