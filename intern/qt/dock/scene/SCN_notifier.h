#pragma once

#include <QObject>

namespace qt::scene {

class SCN_notifier : public QObject {
  Q_OBJECT

 public:
  static SCN_notifier *instance();

 signals:
  void sceneChanged();

 public slots:
  void notifySceneChanged();

 private:
  explicit SCN_notifier(QObject *parent = nullptr);
};

}  // namespace qt::scene

extern "C" {
void outliner_notify_scene_changed();
}
