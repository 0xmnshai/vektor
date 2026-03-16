#include <QMenu>
#include <QPushButton>
#include <QWidget>
#include "WIDGET_header.h"
#include <QHBoxLayout>
#include <QMenu>
#include <vector>

namespace qt::dock {

static const std::vector<EditorInfo> g_editors = {
    {EditorType::VIEWPORT_3D, "3D Viewport", "🎥"},
    {EditorType::OUTLINER, "Outliner", "📐"},
    {EditorType::PROPERTIES, "Properties", "📦"},
    {EditorType::ASSETS_BROWSER, "Assets Browser", "📁"},
    {EditorType::CONSOLE, "Python Console", "📟"},
    {EditorType::GAME_VIEW, "Game View", "🎮"}};

WidgetHeader::WidgetHeader(EditorType initial_type, QWidget *parent)
    : QWidget(parent), current_type_(initial_type)
{
  auto *layout = new QHBoxLayout(this);
  layout->setContentsMargins(4, 2, 4, 2);
  layout->setSpacing(4);

  type_button_ = new QPushButton(this);
  type_button_->setFixedWidth(24);
  type_button_->setFixedHeight(20);
  type_button_->setFlat(true);
  
  // Minimal style for the button to look like Blender's icon button
  type_button_->setStyleSheet("QPushButton { border: none; background: transparent; font-size: 14px; } "
                              "QPushButton:hover { background: #444; border-radius: 2px; }");

  layout->addWidget(type_button_);
  layout->addStretch();

  set_editor_type(initial_type);

  connect(type_button_, &QPushButton::clicked, this, &WidgetHeader::show_type_menu);
  
  setFixedHeight(24);
  setStyleSheet("background-color: #2b2b2b; border-bottom: 1px solid #1a1a1a;");
}

void WidgetHeader::set_editor_type(EditorType type)
{
  current_type_ = type;
  for (const auto &info : g_editors) {
    if (info.type == type) {
      type_button_->setText(info.icon);
      type_button_->setToolTip(info.name);
      break;
    }
  }
}

void WidgetHeader::show_type_menu()
{
  QMenu menu(this);
  menu.setStyleSheet("QMenu { background-color: #333; color: #eee; border: 1px solid #111; } "
                     "QMenu::item { padding: 4px 20px; } "
                     "QMenu::item:selected { background-color: #4c6a8d; }");

  for (const auto &info : g_editors) {
    QAction *action = menu.addAction(info.icon + "  " + info.name);
    connect(action, &QAction::triggered, [this, info]() {
      if (current_type_ != info.type) {
        set_editor_type(info.type);
        emit editorTypeChanged(info.type);
      }
    });
  }

  menu.exec(type_button_->mapToGlobal(QPoint(0, type_button_->height())));
}

}  // namespace qt::dock
