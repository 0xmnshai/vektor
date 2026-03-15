#include "../SCN_notifier.h"

namespace qt::scene {

SCN_notifier *SCN_notifier::instance()
{
  static SCN_notifier s;
  return &s;
}

SCN_notifier::SCN_notifier(QObject *parent) : QObject(parent) {}

void SCN_notifier::notifySceneChanged()
{
  emit sceneChanged();
}

}  // namespace qt::scene

extern "C" {
void outliner_notify_scene_changed()
{
  qt::scene::SCN_notifier::instance()->notifySceneChanged();
}
}
