#define GL_SILENCE_DEPRECATION

#include "../GPU_shader.h"
#include "../../intern/clog/CLG_log.h"
#include "MEM_gaurdalloc.h"

#include <QFile>
#include <QIODevice>
#include <cstddef>
#include <slang-com-ptr.h>
#include <slang.h>

#include "../../creator_global.h"

namespace vektor::gpu {

CLG_LOGREF_DECLARE_GLOBAL(LOG_SHADER, "gpu_shader");

extern void *GPU_metal_pipeline_create(const QByteArray &vert_code, const QByteArray &frag_code);
extern void *GPU_metal_pipeline_create_from_source(const char *source);

static Slang::ComPtr<slang::IGlobalSession> g_slang_session;

static slang::IGlobalSession *get_slang_global_session()
{
  if (!g_slang_session) {
    SlangGlobalSessionDesc desc = {};
    desc.structureSize = sizeof(SlangGlobalSessionDesc);
    desc.apiVersion = SLANG_API_VERSION;
    desc.enableGLSL = true;

    if (SLANG_FAILED(slang_createGlobalSession2(&desc, g_slang_session.writeRef()))) {
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
    return nullptr;
  }

  slang::SessionDesc session_desc = {};
  slang::TargetDesc target_desc = {};

  const bool is_metal = (creator::G.gpu_backend == creator::GPU_BACKEND_METAL);

  if (is_metal) {
    target_desc.format = SLANG_METAL_LIB;
    target_desc.profile = slang_global_session->findProfile("metal_2_0");
  }
  else {
    target_desc.format = SLANG_GLSL;
    target_desc.profile = slang_global_session->findProfile("glsl_410");
    if (target_desc.profile == 0) {
      CLOG_INFO(LOG_SHADER, "[GPU_shader] glsl_410 profile not found, trying glsl_330");
      target_desc.profile = slang_global_session->findProfile("glsl_330");
    }
  }
  target_desc.flags = 0;

  session_desc.targets = &target_desc;
  session_desc.targetCount = 1;
  session_desc.compilerOptionEntryCount = 0;
  session_desc.compilerOptionEntries = nullptr;

  const char *searchPath = ".";
  session_desc.searchPaths = &searchPath;
  session_desc.searchPathCount = 1;

  Slang::ComPtr<slang::ISession> slang_session;
  if (SLANG_FAILED(slang_global_session->createSession(session_desc, slang_session.writeRef()))) {
    CLOG_ERROR(LOG_SHADER, "[GPU_shader] Failed to create Slang session");
    return nullptr;
  }

  auto compile_shader = [&](const char *path, SlangStage stage) -> QByteArray {
    slang::IModule *module = nullptr;
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

    Slang::ComPtr<slang::IBlob> code_blob;
    if (SLANG_FAILED(program->getEntryPointCode(0, 0, code_blob.writeRef(), &diagnostic_blob))) {
      if (diagnostic_blob) {
        CLOG_ERROR(LOG_SHADER,
                   "[GPU_shader] Slang Link Errors for %s : %s ",
                   path,
                   (const char *)diagnostic_blob->getBufferPointer());
      }
      return {};
    }

    QByteArray code((const char *)code_blob->getBufferPointer(),
                    (qsizetype)code_blob->getBufferSize());

    // Hack: Force #version 410 for macOS compatibility
    if (!is_metal && !code.isEmpty()) {
      code.replace("#version 450", "#version 410 core");
      code.replace("#version 460", "#version 410 core");
      // Remove layout(binding = X) which is not supported in 410 for blocks unless used with
      // extensions Also remove _0 suffixes from common uniform names if Slang added them This is a
      // bit risky but simpler than reflection for now
      code.replace("view_0", "view");
      code.replace("projection_0", "projection");
      code.replace("viewInverse_0", "viewInverse");
      code.replace("projInverse_0", "projInverse");
      code.replace("globalParams_0.", "");  // If wrapped in block but we want direct access
      code.replace("block_GlobalParams_0", "block_GlobalParams");
      // Strip layout(binding = 0)
      code.replace("layout(binding = 0)", "");
    }

    // Log the generated GLSL for debugging
    if (!is_metal && !code.isEmpty()) {
      QString glsl_path = QString(path) + ".glsl";
      QFile file(glsl_path);
      if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(code);
        file.close();
        CLOG_INFO(
            LOG_SHADER, "[GPU_shader] Wrote generated GLSL to %s", glsl_path.toUtf8().constData());
      }
      CLOG_INFO(LOG_SHADER, "[GPU_shader] Generated GLSL for %s:\n%s", path, code.constData());
    }

    return code;
  };

  QByteArray vert_code = compile_shader(vert_path, SLANG_STAGE_VERTEX);
  QByteArray frag_code = compile_shader(frag_path, SLANG_STAGE_FRAGMENT);

  if (vert_code.isEmpty() || frag_code.isEmpty()) {
    return nullptr;
  }

  auto *shader = (GPUShader *)MEM_mallocN(sizeof(GPUShader), "GPU_shader");
  shader->backend = is_metal ? GPUShader::GPU_BACKEND_METAL : GPUShader::GPU_BACKEND_OPENGL;
  shader->program = nullptr;
  shader->metal_pipeline = nullptr;

  if (!is_metal) {
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
  }
  else {
    shader->metal_pipeline = GPU_metal_pipeline_create(vert_code, frag_code);
    if (!shader->metal_pipeline) {
      CLOG_ERROR(LOG_SHADER, "[GPU_shader] Failed to create Metal pipeline");
      GPU_shader_free(shader);
      return nullptr;
    }
  }

  return shader;
};

GPUShader *GPU_shader_create_from_source(const char *vert_path, const char *frag_path)
{
  const bool is_metal = (creator::G.gpu_backend == creator::GPU_BACKEND_METAL);

  if (is_metal) {
    // For Metal, we expect a single .metal file for now, or we'll look for .metal
    QString path = QString(vert_path);
    if (!path.endsWith(".metal")) {
      path = path.section('.', 0, -2) + ".metal";
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      CLOG_ERROR(LOG_SHADER, "[GPU_shader] Failed to open Metal source: %s", path.toUtf8().constData());
      return nullptr;
    }

    QByteArray source = file.readAll();
    file.close();

    auto *shader = (GPUShader *)MEM_mallocN(sizeof(GPUShader), "GPU_shader");
    shader->backend = GPUShader::GPU_BACKEND_METAL;
    shader->program = nullptr;
    shader->metal_pipeline = GPU_metal_pipeline_create_from_source(source.constData());

    if (!shader->metal_pipeline) {
      MEM_freeN(shader);
      return nullptr;
    }
    return shader;
  }
  else {
    // OpenGL: read .vert and .frag
    QFile v_file(vert_path);
    if (!v_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      CLOG_ERROR(LOG_SHADER, "[GPU_shader] Failed to open vertex shader: %s", vert_path);
      return nullptr;
    }
    QByteArray v_code = v_file.readAll();
    v_file.close();

    QFile f_file(frag_path);
    if (!f_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      CLOG_ERROR(LOG_SHADER, "[GPU_shader] Failed to open fragment shader: %s", frag_path);
      return nullptr;
    }
    QByteArray f_code = f_file.readAll();
    f_file.close();

    auto *shader = (GPUShader *)MEM_mallocN(sizeof(GPUShader), "GPU_shader");
    shader->backend = GPUShader::GPU_BACKEND_OPENGL;
    shader->metal_pipeline = nullptr;
    shader->program = new QOpenGLShaderProgram();

    if (!shader->program->addShaderFromSourceCode(QOpenGLShader::Vertex, v_code)) {
      CLOG_ERROR(LOG_SHADER, "[GPU_shader] Vertex compile error: %s", shader->program->log().toUtf8().constData());
      GPU_shader_free(shader);
      return nullptr;
    }

    if (!shader->program->addShaderFromSourceCode(QOpenGLShader::Fragment, f_code)) {
      CLOG_ERROR(LOG_SHADER, "[GPU_shader] Fragment compile error: %s", shader->program->log().toUtf8().constData());
      GPU_shader_free(shader);
      return nullptr;
    }

    if (!shader->program->link()) {
      CLOG_ERROR(LOG_SHADER, "[GPU_shader] Link error: %s", shader->program->log().toUtf8().constData());
      GPU_shader_free(shader);
      return nullptr;
    }

    return shader;
  }
}

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
    if (shader->backend == GPUShader::GPU_BACKEND_OPENGL) {
      delete shader->program;
    }
    else {
      // Metal pipeline freeing
      if (shader->metal_pipeline) {
        extern void GPU_metal_pipeline_free(void *pipeline);
        GPU_metal_pipeline_free(shader->metal_pipeline);
      }
      // For now just placeholder or use internal ObjC++ function
    }
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
