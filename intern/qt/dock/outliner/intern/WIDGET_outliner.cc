#include <QAbstractItemView>
#include <QAction>
#include <QHeaderView>
#include <QLineEdit>
#include <QMenu>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>
#include <QVBoxLayout>

#include "../WIDGET_outliner.h"
#include "../scene/SCN_notifier.h"

#include "../../../../../intern/clog/CLG_log.h"
#include "../../../../../source/runtime/dna/DNA_object_type.h"
#include "../../../../source/runtime/kernel/ecs/ECS_registry.h"
#include "../../../../source/runtime/rna/RNA_ecs_registry.h"

namespace qt::dock {

CLG_LOGREF_DECLARE_GLOBAL(LOG_OUTLINER, "outliner");

using namespace vektor::dna;

OutlinerWidget::OutlinerWidget(QWidget *parent) : QWidget(parent)
{
  build_ui();
  build_model();

  connect(tree_view_->selectionModel(),
          &QItemSelectionModel::selectionChanged,
          this,
          &OutlinerWidget::on_selection_changed);

  connect(search_bar_, &QLineEdit::textChanged, this, &OutlinerWidget::on_search_text_changed);

  connect(filter_timer_, &QTimer::timeout, this, &OutlinerWidget::apply_filter);

  connect(tree_view_,
          &QTreeView::customContextMenuRequested,
          this,
          &OutlinerWidget::show_context_menu);

  connect(qt::scene::SCN_notifier::instance(),
          &qt::scene::SCN_notifier::sceneChanged,
          this,
          &OutlinerWidget::refresh_entities);

  refresh_entities();
}

void OutlinerWidget::build_ui()
{
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  search_bar_ = new QLineEdit(this);
  search_bar_->setPlaceholderText("Search...");

  tree_view_ = new QTreeView(this);
  tree_view_->setContextMenuPolicy(Qt::CustomContextMenu);
  tree_view_->setHeaderHidden(true);
  tree_view_->setUniformRowHeights(true);
  tree_view_->setIndentation(12);
  tree_view_->setSelectionMode(QAbstractItemView::ExtendedSelection);
  tree_view_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tree_view_->setAlternatingRowColors(true);
  tree_view_->setAnimated(true);

  // Enable hierarchy lines via style
  tree_view_->setStyleSheet(
      "QTreeView::branch:has-siblings:!adjoins-item { border-left: 1px solid #333; }"
      "QTreeView::branch:has-siblings:adjoins-item { border-left: 1px solid #333; }"
      "QTreeView::branch:!has-children:!has-siblings:adjoins-item { border-left: 1px solid #333; }"
      "QTreeView::branch:has-children:!has-siblings:closed, "
      "QTreeView::branch:closed:has-children:has-siblings { image: none; }"
      "QTreeView::branch:open:has-children:!has-siblings, "
      "QTreeView::branch:open:has-children:has-siblings { image: none; }");

  filter_timer_ = new QTimer(this);
  filter_timer_->setSingleShot(true);

  layout->addWidget(search_bar_);
  layout->addWidget(tree_view_);

  tree_view_->viewport()->installEventFilter(this);
}

void OutlinerWidget::build_model()
{
  model_ = new QStandardItemModel(this);
  model_->setHorizontalHeaderLabels({"Scene Hierarchy", ""});

  proxy_model_ = new QSortFilterProxyModel(this);
  proxy_model_->setSourceModel(model_);
  proxy_model_->setFilterCaseSensitivity(Qt::CaseInsensitive);
  proxy_model_->setFilterKeyColumn(0);

  tree_view_->setModel(proxy_model_);
  tree_view_->setColumnWidth(0, 200);
  tree_view_->setColumnWidth(1, 24);
}

void OutlinerWidget::refresh_entities()
{
  CLOG_INFO(LOG_OUTLINER, "Refreshing entities...");
  is_refreshing_ = true;

  model_->removeRows(0, model_->rowCount());

  // Level 1: Scene Collection
  auto *root_item = new QStandardItem("Scene Collection");
  model_->appendRow({root_item, new QStandardItem("")});

  // Level 2: Collection
  auto *collection_item = new QStandardItem("Collection");
  collection_item->setData(QString("📦"), Qt::DecorationRole);
  root_item->appendRow({collection_item, new QStandardItem("")});

  // Level 3: Objects
  auto &registry = vektor::kernel::ECSRegistry::instance().registry();
  auto objects_view = registry.view<vektor::dna::Object>();

  for (auto entity : objects_view) {
    const auto &obj = objects_view.get<vektor::dna::Object>(entity);
    collection_item->appendRow(create_object_item(entity, obj));
  }

  tree_view_->expandAll();
  sync_selection_from_scene();

  is_refreshing_ = false;
  CLOG_INFO(LOG_OUTLINER, "Refresh complete.");
}

QList<QStandardItem *> OutlinerWidget::create_object_item(entt::entity entity,
                                                        const vektor::dna::Object &obj)
{
  QString name = QString::fromUtf8(obj.id.name);
  auto *name_item = new QStandardItem(name);

  QString icon = "❓";
  switch (obj.type) {
    case ObjectType::Mesh:
      icon = "📐";
      break;
    case ObjectType::Camera:
      icon = "🎥";
      break;
    case ObjectType::Light:
      icon = "💡";
      break;
    case ObjectType::Empty:
      icon = "🔘";
      break;
    default:
      break;
  }

  name_item->setData(icon, Qt::DecorationRole);
  name_item->setData((uint32_t)entity, Qt::UserRole);

  auto *visibility_item = new QStandardItem("👁️");
  visibility_item->setTextAlignment(Qt::AlignCenter);
  visibility_item->setData((uint32_t)entity, Qt::UserRole);

  return {name_item, visibility_item};
}

// Re-implementing refresh_entities loop to handle multi-column
void OutlinerWidget::rebuild_tree()
{
  refresh_entities();
}

void OutlinerWidget::on_selection_changed(const QItemSelection &selected,
                                          const QItemSelection &deselected)
{
  if (is_refreshing_) {
    return;
  }
  sync_selection_to_scene(selected, deselected);
}

void OutlinerWidget::sync_selection_to_scene(const QItemSelection &selected,
                                             const QItemSelection &deselected)
{
  auto &registry = vektor::kernel::ECSRegistry::instance();

  for (const auto &idx : deselected.indexes()) {
    if (idx.column() != 0)
      continue;
    QModelIndex src = proxy_model_->mapToSource(idx);
    auto *item = model_->itemFromIndex(src);
    if (item && item->data(Qt::UserRole).isValid()) {
      auto entity = (entt::entity)item->data(Qt::UserRole).toUInt();
      vektor::rna::RNA_ecs_set_selected(&registry, entity, false);
    }
  }

  for (const auto &idx : selected.indexes()) {
    if (idx.column() != 0)
      continue;
    QModelIndex src = proxy_model_->mapToSource(idx);
    auto *item = model_->itemFromIndex(src);
    if (item && item->data(Qt::UserRole).isValid()) {
      auto entity = (entt::entity)item->data(Qt::UserRole).toUInt();
      vektor::rna::RNA_ecs_set_selected(&registry, entity, true);
      vektor::rna::RNA_ecs_set_active(&registry, entity, true);
    }
  }
}

void OutlinerWidget::sync_selection_from_scene()
{
  auto &registry = vektor::kernel::ECSRegistry::instance();
  auto &reg = registry.registry();
  auto view = reg.view<vektor::dna::Object>();

  // Brute force sync for now
  for (int i = 0; i < model_->rowCount(); ++i) {
    QStandardItem *root = model_->item(i);
    for (int j = 0; j < root->rowCount(); ++j) {
      QStandardItem *coll = root->child(j);
      for (int k = 0; k < coll->rowCount(); ++k) {
        QStandardItem *item = coll->child(k);
        if (item->data(Qt::UserRole).isValid()) {
          auto entity = (entt::entity)item->data(Qt::UserRole).toUInt();
          if (vektor::rna::RNA_ecs_is_selected(&registry, entity)) {
            QModelIndex src = model_->indexFromItem(item);
            QModelIndex proxy = proxy_model_->mapFromSource(src);
            tree_view_->selectionModel()->select(
                proxy, QItemSelectionModel::Select | QItemSelectionModel::Rows);
          }
        }
      }
    }
  }
}

void OutlinerWidget::show_context_menu(const QPoint &pos)
{
  QModelIndex proxy_index = tree_view_->indexAt(pos);
  QMenu menu(this);

  if (proxy_index.isValid()) {
    QModelIndex src = proxy_model_->mapToSource(proxy_index);
    auto *item = model_->itemFromIndex(src);

    if (item && item->data(Qt::UserRole).isValid()) {
      auto entity = (entt::entity)item->data(Qt::UserRole).toUInt();
      build_object_context_menu(menu, entity);
    }
  }

  build_add_menu(menu);
  menu.exec(tree_view_->viewport()->mapToGlobal(pos));
}

void OutlinerWidget::build_object_context_menu(QMenu &menu, entt::entity entity)
{
  QAction *select = menu.addAction("Select");
  connect(select, &QAction::triggered, [this, entity]() {
    auto &registry = vektor::kernel::ECSRegistry::instance();
    vektor::rna::RNA_ecs_set_selected(&registry, entity, true);
    refresh_entities();  // Refresh to show selection if needed, or rely on notifier
  });

  QAction *delete_obj = menu.addAction("Delete");
  connect(delete_obj, &QAction::triggered, [entity]() {
    auto &registry = vektor::kernel::ECSRegistry::instance();
    vektor::rna::RNA_ecs_destroy_entity(&registry, entity);
    outliner_notify_scene_changed();
  });

  menu.addSeparator();
}

void OutlinerWidget::build_add_menu(QMenu &menu)
{
  QMenu *add = menu.addMenu("Add");
  QMenu *mesh = add->addMenu("Mesh");

  auto create = [](const char *name, const char *desc) {
    vektor::kernel::create_entity(nullptr,
                                  nullptr,
                                  name,
                                  desc,
                                  (int)vektor::dna::ObjectType::Mesh,
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  0.26f,
                                  0.27f,
                                  0.29f);
    outliner_notify_scene_changed();
  };

  QAction *cube = mesh->addAction("Cube");
  connect(cube, &QAction::triggered, [create]() { create("Cube", "Default Cube"); });

  QAction *plane = mesh->addAction("Plane");
  connect(plane, &QAction::triggered, [create]() { create("Plane", "Default Plane"); });

  QAction *cylinder = mesh->addAction("Cylinder");
  connect(cylinder, &QAction::triggered, [create]() { create("Cylinder", "Default Cylinder"); });
}

bool OutlinerWidget::eventFilter(QObject *watched, QEvent *event)
{
  if (watched == tree_view_->viewport() && event->type() == QEvent::MouseButtonPress) {
    auto *mouse_event = dynamic_cast<QMouseEvent *>(event);
    if (!tree_view_->indexAt(mouse_event->pos()).isValid()) {
      tree_view_->selectionModel()->clearSelection();
    }
  }
  return QWidget::eventFilter(watched, event);
}

void OutlinerWidget::on_search_text_changed(const QString &)
{
  filter_timer_->start(200);  // 200ms debounce
}

void OutlinerWidget::apply_filter()
{
  proxy_model_->setFilterFixedString(search_bar_->text());
}

}  // namespace qt::dock
