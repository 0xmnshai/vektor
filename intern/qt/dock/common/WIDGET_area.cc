#include <QDockWidget>
#include <QObject>
#include <QWidget>
#include <QVBoxLayout>

#include "WIDGET_area.h"
#include "WIDGET_header.h"

#include "../assets_browser/WIDGET_assets_browser.h"
#include "../console/WIDGET_console.h"
#include "../outliner/WIDGET_outliner.h"
#include "../properties/WIDGET_properties.h"
#include "../scene/WIDGET_viewport.h"

namespace qt::dock {

AreaWidget::AreaWidget(EditorType initial_type, QWidget *parent) 
    : QWidget(parent), header_(nullptr), content_(nullptr)
{
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  header_ = new WidgetHeader(initial_type, this);
  layout->addWidget(header_);

  content_ = create_editor(initial_type);
  if (content_) {
    layout->addWidget(content_);
  }

  connect(header_, &WidgetHeader::editorTypeChanged, this, &AreaWidget::handle_editor_switch);
}

void AreaWidget::handle_editor_switch(EditorType new_type)
{
  if (content_) {
    layout()->removeWidget(content_);
    content_->hide();
    content_->setParent(nullptr);
    content_->deleteLater();
    content_ = nullptr;
  }

  content_ = create_editor(new_type);
  if (content_) {
    layout()->addWidget(content_);
    content_->show();

    // Update parent dock widget title if it exists
    QWidget *pw = parentWidget();
    while (pw) {
      if (auto *dock = qobject_cast<QDockWidget *>(pw)) {
        for (const auto &info : g_editors) {
          if (info.type == new_type) {
            dock->setWindowTitle(info.name);
            break;
          }
        }
        break;
      }
      pw = pw->parentWidget();
      if (!pw) break;
    }
  }
}

QWidget *AreaWidget::create_editor(EditorType type)
{
  switch (type) {
    case EditorType::VIEWPORT_3D:
    {
      auto *viewport = new ViewportWidget(this);
      viewport->init();
      return static_cast<QWidget*>(viewport);
    }
    case EditorType::OUTLINER:
      return static_cast<QWidget*>(new OutlinerWidget(this));
    case EditorType::PROPERTIES:
      return static_cast<QWidget*>(new PropertiesWidget(this));
    case EditorType::ASSETS_BROWSER:
      return static_cast<QWidget*>(new AssetsBrowserWidget(this));
    case EditorType::CONSOLE:
      return static_cast<QWidget*>(new ConsoleWidget(this));
    case EditorType::GAME_VIEW:
      return new QWidget(this);
    default:
      return nullptr;
  }
}

}  // namespace qt::dock
