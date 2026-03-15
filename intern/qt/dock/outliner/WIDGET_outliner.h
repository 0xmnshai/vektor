#pragma once
#include <QTimer>
#include <QWidget>

class QTreeView;
class QStandardItemModel;
class QSortFilterProxyModel;
class QLineEdit;

namespace qt::dock {
class OutlinerWidget : public QWidget {
  Q_OBJECT
 public:
  explicit OutlinerWidget(QWidget *parent = nullptr);

 public slots:
  void refresh_entities();
  void on_search_text_changed(const QString &text);
  void apply_filter();

 private:
  QTreeView *tree_view_;
  QStandardItemModel *model_;
  QSortFilterProxyModel *proxy_model_;
  QLineEdit *search_bar_;
  QTimer *timer_;
  QTimer *filter_timer_;
};
}  // namespace qt::dock
