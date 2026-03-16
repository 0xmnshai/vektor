#pragma once

#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QWidget>
#include <vector>

namespace qt::dock {

enum class EditorType {
  VIEWPORT_3D,
  OUTLINER,
  PROPERTIES,
  ASSETS_BROWSER,
  CONSOLE,
  GAME_VIEW
};

struct EditorInfo {
  EditorType type;
  QString name;
  QString icon;
};

extern const std::vector<EditorInfo> g_editors;

class WidgetHeader : public QWidget {
  Q_OBJECT
 public:
  explicit WidgetHeader(EditorType initial_type, QWidget *parent = nullptr);

  void set_editor_type(EditorType type);

 signals:
  void editorTypeChanged(EditorType new_type);

 private:
  QPushButton *type_button_;
  QLabel *title_label_;
  EditorType current_type_;
  
  void show_type_menu();
};

}  // namespace qt::dock
