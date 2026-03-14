#include "../GPU_shader.h"
#include "../../intern/clog/CLG_log.h"
#include "MEM_gaurdalloc.h"

#include <cstddef>
#include <slang-com-ptr.h>
#include <slang.h>

namespace vektor::gpu {

CLG_LOGREF_DECLARE_GLOBAL(LOG_SHADER, "gpu_shader");

static Slang::ComPtr<slang::IGlobalSession> g_slang_session;

static slang::IGlobalSession *get_slang_global_session()
{
  if (!g_slang_session) {
    if (SLANG_FAILED(createGlobalSession(g_slang_session.writeRef()))) {
      CLOG_ERROR(LOG_SHADER, "[GPU_shader] Failed to create Slang global session");
      return nullptr;
    }
  }
  return g_slang_session.get();
}

GPUShader *GPU_shader_create_from_slang(const char *vert_path, const char *frag_path)
{
  slang::IGlobalSession *slang_global_session = get_slang_global_session();

  if (slang_global_session == nullptr) {
    CLOG_ERROR(LOG_SHADER, "[GPU_Shader] Slang Global Sessions is Empty");
  }

  slang::SessionDesc session_desc = {};
  slang::TargetDesc target_desc = {};
  target_desc.format = SLANG_GLSL;
  target_desc.profile = slang_global_session->findProfile("410");  // opengl 4.1 profile
  target_desc.flags = 0;

  session_desc.targets = &target_desc;
  session_desc.targetCount = 1;
  session_desc.compilerOptionEntryCount = 0;

  const char *searchPath = ".";
  session_desc.searchPaths = &searchPath;
  session_desc.searchPathCount = 1;

  Slang::ComPtr<slang::ISession> slang_session;
  if (SLANG_FAILED(slang_global_session->createSession(session_desc, slang_session.writeRef()))) {
    CLOG_ERROR(LOG_SHADER, "[GPU_shader] Failed to create Slang session");
    return nullptr;
  }

  auto compile_shader = [&](const char *path, SlangStage stage) -> QByteArray {
    Slang::ComPtr<slang::IModule> module;
    slang::IBlob *diagnostic_blob = nullptr;
    module = slang_session->loadModule(path, &diagnostic_blob);

    if (diagnostic_blob) {
      CLOG_INFO(LOG_SHADER,
                "[GPU_shader] Slang Diagnostics for %s : %s ",
                path,
                (const char *)diagnostic_blob->getBufferPointer());
    }

    if (!module)
      return {};

    Slang::ComPtr<slang::IEntryPoint> entry;
    module->findEntryPointByName("main", entry.writeRef());

    slang::IComponentType *components[] = {module, entry};
    Slang::ComPtr<slang::IComponentType> program;
    slang_session->createCompositeComponentType(components, 2, program.writeRef());

    Slang::ComPtr<slang::IBlob> glsl_blob;
    if (SLANG_FAILED(program->getEntryPointCode(0, 0, glsl_blob.writeRef(), &diagnostic_blob))) {
      if (diagnostic_blob) {
        CLOG_ERROR(LOG_SHADER,
                   "[GPU_shader] Slang Link Errors for %s : %s ",
                   path,
                   (const char *)diagnostic_blob->getBufferPointer());
      }
      return {};
    }

    QByteArray glsl_code((const char *)glsl_blob->getBufferPointer(),
                         (qsizetype)glsl_blob->getBufferSize());
    return glsl_code;
  };

  QByteArray vert_code = compile_shader(vert_path, SLANG_STAGE_VERTEX);
  QByteArray frag_code = compile_shader(frag_path, SLANG_STAGE_FRAGMENT);

  if (vert_code.isEmpty() || frag_code.isEmpty()) {
    return nullptr;
  }

  auto *shader = (GPUShader *)MEM_mallocN(sizeof(GPUShader), "GPU_shader");
  shader->program = new QOpenGLShaderProgram();

  if (!shader->program->addShaderFromSourceCode(QOpenGLShader::Vertex, vert_code)) {
    CLOG_ERROR(LOG_SHADER,
               "[GPU_shader] Vertex shader compile error in %s : %s ",
               vert_path,
               (const char *)shader->program->log().toUtf8());
    GPU_shader_free(shader);
    return nullptr;
  }

  if (!shader->program->addShaderFromSourceCode(QOpenGLShader::Fragment, frag_code)) {
    CLOG_ERROR(LOG_SHADER,
               "[GPU_shader] Fragment shader compile error in %s : %s ",
               frag_path,
               (const char *)shader->program->log().toUtf8());
    GPU_shader_free(shader);
    return nullptr;
  }

  if (!shader->program->link()) {
    CLOG_ERROR(LOG_SHADER,
               "[GPU_shader] Shader link error (%s + %s) : %s ",
               vert_path,
               frag_path,
               (const char *)shader->program->log().toUtf8());
    GPU_shader_free(shader);
    return nullptr;
  }
  return shader;
};

void print_compute_results()
{
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  auto *result = (float *)glMapBufferRange(
      GL_SHADER_STORAGE_BUFFER, 0, 1024 * sizeof(float), GL_MAP_READ_BIT);
  for (size_t i = 0; i < 1024; i++) {
    CLOG_INFO(LOG_SHADER, "[GPU_shader] Compute result: %f", result[i]);
  }
  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

QOpenGLShaderProgram *GPU_shader_get_program(GPUShader *shader)
{
  return shader ? shader->program : nullptr;
}

void GPU_shader_bind(GPUShader *shader)
{
  if (shader && shader->program) {
    shader->program->bind();
  }
}

void GPU_shader_unbind(GPUShader *shader)
{
  if (shader && shader->program) {
    shader->program->release();
  }
}

void GPU_shader_free(GPUShader *shader)
{
  if (shader) {
    delete shader->program;
    MEM_freeN(shader);
  }
}

void GPU_shader_uniform_float(GPUShader *shader, const char *name, float val)
{
  if (shader && shader->program) {
    shader->program->setUniformValue(name, val);
  }
}

void GPU_shader_uniform_vector3(GPUShader *shader, const char *name, const float val[3])
{
  if (shader && shader->program) {
    shader->program->setUniformValue(name, val[0], val[1], val[2]);
  }
}

void GPU_shader_uniform_matrix4(GPUShader *shader, const char *name, const float val[16])
{
  if (shader && shader->program) {
    QMatrix4x4 mat(val);
    shader->program->setUniformValue(name, mat);
  }
}
}  // namespace vektor::gpu
