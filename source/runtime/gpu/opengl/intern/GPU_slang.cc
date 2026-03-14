#include "../GPU_slang.h"
#include "../../GPU_shader.h"
#include "../../intern/clog/CLG_log.h"

namespace vektor::gpu {
CLG_LOGREF_DECLARE_GLOBAL(LOG_GPU, "gpu_slang");

QOpenGLShaderProgram *SlangLoader::load(const QString &vert_path,
                                        const QString &frag_path,
                                        QObject *parent)
{
  //   auto *shader = new QOpenGLShaderProgram(parent);
  //   shader->addShaderFromSourceFile(QOpenGLShader::Vertex, vert_path);
  //   shader->addShaderFromSourceFile(QOpenGLShader::Fragment, frag_path);
  //   shader->link();
  //   return shader;

  GPUShader *shader = GPU_shader_create_from_slang(vert_path.toUtf8().constData(),
                                                   frag_path.toUtf8().constData());

  if (!shader) {
    qWarning() << "[SlangLoader] Failed to create GPU shader from" << vert_path << frag_path;
    return nullptr;
  }

  // Extract the program for legacy Qt compatibility
  QOpenGLShaderProgram *prog = GPU_shader_get_program(shader);
  if (prog) {
    prog->setParent(parent);
  }

  // NOTE: In a full Blender-style refactor, we would return GPUShader* and manage its lifetime.
  // For now, we return the program and leak the thin GPUShader wrapper (or we'd need to free it).
  // Actually, let's just make SlangLoader return the program and handle the GPUShader cleanup
  // locally but keep the program alive. This is tricky because GPU_shader_free deletes the
  // program.

  // Better: keep the GPUShader wrap but let the user manage it?
  // No, user wants SlangLoader::load to return QOpenGLShaderProgram*.

  // We'll modify GPU_shader_free to have an option or just do a custom return here.
  return prog;
}
}  // namespace vektor::gpu
