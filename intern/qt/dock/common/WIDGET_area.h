#pragma once

#include <QWidget>
#include "WIDGET_header.h"

namespace qt::dock {

class AreaWidget : public QWidget {
  Q_OBJECT
 public:
  explicit AreaWidget(EditorType initial_type, QWidget *parent = nullptr);

 private slots:
  void handle_editor_switch(EditorType new_type);

 private:
  WidgetHeader *header_;
  QWidget *content_ = nullptr;
  
  QWidget* create_editor(EditorType type);
};

}  // namespace qt::dock
