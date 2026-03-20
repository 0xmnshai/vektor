#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

#include "../../dock/scene/SCN_notifier.h"
#include "PRP_transform.hh"
#include "WIDGET_properties.hh"

#include "../../../../../source/runtime/kernel/ecs/ECS_registry.h"

namespace qt::dock {
PropertiesPanel::PropertiesPanel(QWidget *parent) : QWidget(parent)
{
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  auto *scroll_area = new QScrollArea();
  scroll_area->setWidgetResizable(true);
  scroll_area->setFrameShape(QFrame::NoFrame);
  scroll_area->setStyleSheet("background-color: #212121;");

  container_widget_ = new QWidget();
  container_widget_->setStyleSheet("background-color: #212121;");

  sub_panel_layout_ = new QVBoxLayout(container_widget_);
  sub_panel_layout_->setAlignment(Qt::AlignTop);
  sub_panel_layout_->setContentsMargins(8, 8, 8, 8);
  sub_panel_layout_->setSpacing(8);

  scroll_area->setWidget(container_widget_);
  layout->addWidget(scroll_area);

  connect(qt::scene::SCN_notifier::instance(),
          &qt::scene::SCN_notifier::sceneChanged,
          this,
          &PropertiesPanel::on_selection_changed);
}

void PropertiesPanel::on_selection_changed()
{
  auto &registry = vektor::kernel::ECSRegistry::instance().registry();
  auto view = registry.view<vektor::dna::Object, vektor::dna::Active>();

  selected_object_ = nullptr;
  for (auto entity : view) {
    const auto &active = view.get<vektor::dna::Active>(entity);
    if (active.active) {
      selected_object_ = &view.get<vektor::dna::Object>(entity);
      break;
    }
  }

  QLayoutItem *item;
  while ((item = sub_panel_layout_->takeAt(0)) != nullptr) {
    if (item->widget()) {
      item->widget()->deleteLater();
    }
    delete item;
  }

  if (!selected_object_) {
    auto *no_select = new QLabel("Select an object to view properties");
    no_select->setAlignment(Qt::AlignCenter);
    no_select->setStyleSheet("color: #666; margin-top: 40px; font-style: italic;");
    sub_panel_layout_->addWidget(no_select);
    return;
  }

  sub_panel_layout_->addWidget(new TransformPanel(selected_object_, container_widget_));

  /**
  // 2. Data/Type specific panels
    if (m_selectedObject->type == OB_TYPE_MESH) {
      m_subPanelLayout->addWidget(new MaterialSubPanel(m_selectedObject, m_container));
    } else if (m_selectedObject->type == OB_TYPE_CAMERA) {
      m_subPanelLayout->addWidget(new CameraSubPanel(m_selectedObject, m_container));
    }
  */


}
}  // namespace qt::dock
