#pragma once

#include <QOpenGLShaderProgram>
#include <QtCore/QString>

namespace vektor::gpu {
class SlangLoader {
 public:
  SlangLoader() = default;
  ~SlangLoader() = default;

  static QOpenGLShaderProgram *load(const QString &vert_path,
                                    const QString &frag_path,
                                    QObject *parent = nullptr);

  static QOpenGLShaderProgram *load_by_name(const QString &shader_dir,
                                            const QString &base_name,
                                            QObject *parent = nullptr)
  {
    return load(shader_dir + "/" + base_name + ".vert.slang",
                shader_dir + "/" + base_name + ".frag.slang",
                parent);
  }
};
}  // namespace vektor::gpu
