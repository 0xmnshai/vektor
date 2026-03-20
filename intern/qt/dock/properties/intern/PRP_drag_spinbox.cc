#include "PRP_drag_spinbox.hh"

namespace qt::dock {

DragSpinBox::DragSpinBox(QWidget *parent) : QDoubleSpinBox(parent)
{
  setButtonSymbols(QAbstractSpinBox::NoButtons);
  setStyleSheet("background-color: #1a1a1a; color: #eee; border: 1px solid #333; padding: 2px;");
  lineEdit()->setReadOnly(true);
  lineEdit()->setCursor(Qt::SizeHorCursor);
  lineEdit()->installEventFilter(this);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setMinimumWidth(45);
}

DragSpinBox::~DragSpinBox()
{
  restore_cursor_if_needed();
}

void DragSpinBox::hideEvent(QHideEvent *event)
{
  if (dragging_) {
    end_drag();
  }
  QDoubleSpinBox::hideEvent(event);
}

void DragSpinBox::wheelEvent(QWheelEvent *event)
{
  if (lineEdit()->isReadOnly()) {
    double step = singleStep();
    if (event->modifiers() & Qt::ShiftModifier)
      step *= 0.1;
    if (event->modifiers() & Qt::ControlModifier)
      step *= 10.0;

    // Standard wheel delta is 120 per notch.
    // We'll use a multiplier to make it feel "good" and faster as requested.
    double delta = event->angleDelta().y() / 120.0;
    setValue(value() + delta * step * 5.0);  // 5x faster than single step as base
    event->accept();
  }
  else {
    QDoubleSpinBox::wheelEvent(event);
  }
}

bool DragSpinBox::eventFilter(QObject *watched, QEvent *event)
{
  if (watched == lineEdit()) {
    if (event->type() == QEvent::MouseButtonPress) {
      auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);
      if (mouseEvent->button() == Qt::LeftButton && lineEdit()->isReadOnly()) {
        dragging_ = true;
        start_pos_ = mouseEvent->globalPosition().toPoint();
        last_pos_ = start_pos_;

        if (!has_override_cursor_) {
          QApplication::setOverrideCursor(Qt::BlankCursor);
          has_override_cursor_ = true;
        }

        lineEdit()->grabMouse();
        return true;  // We consume this so line edit doesn't start select
      }
    }
    else if (event->type() == QEvent::MouseMove) {
      if (dragging_) {
        auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);
        QPoint current_pos = mouseEvent->globalPosition().toPoint();
        int delta = current_pos.x() - last_pos_.x();

        if (delta != 0) {
          // Ignore synthetic warp jumps
          if (qAbs(delta) < 500) {
            double step = singleStep();
            if (mouseEvent->modifiers() & Qt::ShiftModifier)
              step *= 0.1;
            if (mouseEvent->modifiers() & Qt::ControlModifier)
              step *= 10.0;

            setValue(value() + (double)delta * step);
          }

          last_pos_ = current_pos;

// Screen wrapping hack for infinite drag without double counting.
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
          QScreen *screen = QApplication::screenAt(current_pos);
#else
          QScreen *screen = QGuiApplication::screenAt(current_pos);
#endif
          if (!screen)
            screen = QApplication::primaryScreen();

          if (screen) {
            QRect geom = screen->geometry();
            bool warped = false;
            int new_x = current_pos.x();

            if (current_pos.x() <= geom.left() + 5) {
              new_x = geom.right() - 10;
              warped = true;
            }
            else if (current_pos.x() >= geom.right() - 5) {
              new_x = geom.left() + 10;
              warped = true;
            }

            if (warped) {
              QPoint warp_pos(new_x, current_pos.y());
              QCursor::setPos(warp_pos);
              last_pos_ = warp_pos;  // Next delta will be huge and ignored
            }
          }
        }
        return true;
      }
    }
    else if (event->type() == QEvent::MouseButtonRelease) {
      auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);
      if (dragging_ && mouseEvent->button() == Qt::LeftButton) {
        end_drag();
        return true;
      }
    }
    else if (event->type() == QEvent::MouseButtonDblClick) {
      if (dragging_) {
        end_drag();
      }
      lineEdit()->setReadOnly(false);
      lineEdit()->selectAll();
      lineEdit()->setFocus();
      return true;
    }
    else if (event->type() == QEvent::FocusOut) {
      lineEdit()->setReadOnly(true);
    }
  }
  return QDoubleSpinBox::eventFilter(watched, event);
}

void DragSpinBox::end_drag()
{
  if (dragging_) {
    dragging_ = false;
    lineEdit()->releaseMouse();
    QCursor::setPos(start_pos_);  // Place cursor where drag started
  }
  restore_cursor_if_needed();
}

void DragSpinBox::restore_cursor_if_needed()
{
  if (has_override_cursor_) {
    QApplication::restoreOverrideCursor();
    has_override_cursor_ = false;
  }
}
}  // namespace qt::dock
