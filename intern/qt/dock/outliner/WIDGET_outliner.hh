#pragma once

#include <QItemSelection>
#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>
#include <QTimer>
#include <QWidget>
#include <entt/entt.hpp>

#include <unordered_map>

#include "../../../../source/runtime/dna/DNA_object_type.h"

class QTreeView;
class QStandardItemModel;
class QSortFilterProxyModel;
class QLineEdit;

namespace qt::dock {
class OutlinerPanel : public QWidget {
  Q_OBJECT
 public:
  explicit OutlinerPanel(QWidget *parent = nullptr);

 private slots:
  void refresh_entities();
  void on_search_text_changed(const QString &text);
  void apply_filter();

  void show_context_menu(const QPoint &pos);
  void on_selection_changed(const QItemSelection &selected, const QItemSelection &deselected);

 private:
  void build_ui();
  void build_model();
  void rebuild_tree();
  void sync_selection_from_scene();
  void sync_selection_to_scene(const QItemSelection &selected, const QItemSelection &deselected);

  QList<QStandardItem *> create_object_item(entt::entity entity,
                                           const vektor::dna::Object &obj);

  void build_object_context_menu(QMenu &menu, entt::entity entity);
  static void build_add_menu(QMenu &menu);

 protected:
  bool eventFilter(QObject *watched, QEvent *event) override;

 private:
  QTreeView *tree_view_ = nullptr;
  QStandardItemModel *model_ = nullptr;
  QSortFilterProxyModel *proxy_model_ = nullptr;
  QLineEdit *search_bar_ = nullptr;
  QTimer *filter_timer_ = nullptr;
  QTimer *refresh_timer_ = nullptr;
  bool is_refreshing_ = false;

  std::unordered_map<entt::entity, QStandardItem *> entity_to_item_;
};
}  // namespace qt::dock
