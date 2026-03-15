#pragma once
#include <QTimer>
#include <QWidget>

class QTreeView;
class QStandardItemModel;

namespace qt::dock {
class OutlinerWidget : public QWidget {
  Q_OBJECT
 public:
  explicit OutlinerWidget(QWidget *parent = nullptr);

 public slots:
  void refresh_entities();

 private:
  QTreeView *tree_view_;
  QStandardItemModel *model_;
  QTimer *timer_;
};
}  // namespace qt::dock
