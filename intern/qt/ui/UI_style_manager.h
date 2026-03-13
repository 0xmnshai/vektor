#pragma once

#include <QtCore/QString>

namespace qt::ui {

class UI_style_manager {
 public:
  explicit UI_style_manager() = default;
  ~UI_style_manager() = default;

  static QString get_qt_style();
};
}  // namespace qt::ui
