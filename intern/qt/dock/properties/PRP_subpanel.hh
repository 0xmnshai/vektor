#pragma once

#include <QColorDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

#include "../../../../intern/config/CONFIG_manager.h"
#include "../../../../source/runtime/dna/DNA_object_type.h"
#include "intern/PRP_drag_spinbox.hh"

namespace qt::dock {

class PropertySubPanel : public QGroupBox {
  Q_OBJECT
 public:
  PropertySubPanel(const QString &title, vektor::dna::Object *ob, QWidget *parent = nullptr)
      : QGroupBox(title, parent), object_(ob)
  {
    auto &cfg = config::ConfigManager::instance();
    // Load the stylesheet config
    cfg.load("qt_style",
             "/Users/lazycodebaker/Documents/Dev/CPP/vektor/intern/config/ini/qt_style.ini");

    QString group_box_border_top_color = QString::fromStdString(
        cfg.get_string("qt_style", "GroupBox::Title", "BorderTopColor", "#2b2b2b"));
    QString group_box_font_weight = QString::fromStdString(
        cfg.get_string("qt_style", "GroupBox::Title", "FontWeight", "bold"));

    layout_ = new QGridLayout(this);
    layout_->setContentsMargins(4, 18, 4, 4);
    layout_->setSpacing(2);
    layout_->setHorizontalSpacing(8);

    // Ensure coordinate columns (X, Y, Z) share space equally and don't jump
    layout_->setColumnStretch(1, 1);
    layout_->setColumnStretch(2, 1);
    layout_->setColumnStretch(3, 1);

    layout_->setColumnMinimumWidth(1, 75);
    layout_->setColumnMinimumWidth(2, 75);
    layout_->setColumnMinimumWidth(3, 75);

    QString style =
        QString(
            "QGroupBox { font-weight: %1; border: 1px solid %2; margin-top: 10px; padding-top: "
            "10px; "
            "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 3px; color: "
            "#888; "
            "} ")
            .arg(group_box_font_weight)
            .arg(group_box_border_top_color);

    setStyleSheet(style);
  }
  virtual void update_ui() = 0;

 protected:
  vektor::dna::Object *object_;
  QGridLayout *layout_;

  void create_spin_box_row(int row,
                           const QString &label,
                           DragSpinBox *&x,
                           DragSpinBox *&y,
                           DragSpinBox *&z,
                           double step = 0.1)
  {
    auto *row_label = new QLabel(label);
    row_label->setMinimumWidth(80);
    row_label->setStyleSheet("color: #bbb; font-size: 11px;");
    layout_->addWidget(row_label, row, 0);

    auto create_sb =
        [&](const QString &text, const QString &color, DragSpinBox *&sb, double sb_step) {
          auto *h_layout = new QHBoxLayout();
          h_layout->setSpacing(2);

          auto *axis_label = new QLabel(text);
          axis_label->setStyleSheet(
              QString("color: %1; font-weight: bold; font-size: 10px;").arg(color));

          sb = new DragSpinBox();
          sb->setRange(-10e6, 10e6);
          sb->setSingleStep(sb_step);
          sb->setDecimals(3);

          h_layout->addWidget(axis_label);
          h_layout->addWidget(sb);
          return h_layout;
        };

    layout_->addLayout(create_sb("X", "#ff4444", x, step), row, 1);
    layout_->addLayout(create_sb("Y", "#44ff44", y, step), row, 2);
    layout_->addLayout(create_sb("Z", "#4444ff", z, step), row, 3);
  }
};
}  // namespace qt::dock
