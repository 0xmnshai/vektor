#pragma once

#include <QApplication>
#include <QCursor>
#include <QDoubleSpinBox>
#include <QGuiApplication>
#include <QLineEdit>
#include <QMouseEvent>
#include <QScreen>

namespace qt::dock {

class DragSpinBox : public QDoubleSpinBox {
  Q_OBJECT
 public:
  explicit DragSpinBox(QWidget *parent = nullptr);

  ~DragSpinBox();

 protected:
  void wheelEvent(QWheelEvent *event) override;

  void hideEvent(QHideEvent *event) override;
  bool eventFilter(QObject *watched, QEvent *event) override;

 private:
  bool dragging_ = false;
  bool has_override_cursor_ = false;
  QPoint start_pos_;
  QPoint last_pos_;

  void end_drag();
  void restore_cursor_if_needed();
};

}  // namespace qt::dock
