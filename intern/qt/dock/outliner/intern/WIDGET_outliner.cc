#include <QAbstractItemView>
#include <QHeaderView>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>
#include <QVBoxLayout>

#include "../WIDGET_outliner.h"
#include "../scene/SCN_notifier.h"

#include "../../../../../source/runtime/dna/DNA_object_type.h"
#include "../../../../source/runtime/kernel/ecs/ECS_registry.h"
#include "../../../../../intern/clog/CLG_log.h"
#include "../../../../source/runtime/rna/RNA_ecs_registry.h"

namespace qt::dock {
CLG_LOGREF_DECLARE_GLOBAL(LOG_OUTLINER, "outliner");

using namespace vektor::dna;

OutlinerWidget::OutlinerWidget(QWidget *parent) : QWidget(parent)
{
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  search_bar_ = new QLineEdit();
  search_bar_->setPlaceholderText("Search...");

  // layout->addWidget(search_bar_);

  tree_view_ = new QTreeView(this);
  model_ = new QStandardItemModel(this);
  model_->setHorizontalHeaderLabels({"Scene Hierarchy", ""});

  proxy_model_ = new QSortFilterProxyModel(this);
  proxy_model_->setSourceModel(model_);
  proxy_model_->setFilterCaseSensitivity(Qt::CaseInsensitive);
  proxy_model_->setFilterKeyColumn(0);  // Filter by Label

  tree_view_->setModel(proxy_model_);
  tree_view_->setAlternatingRowColors(true);
  tree_view_->setColumnWidth(0, 200);
  tree_view_->setColumnWidth(1, 24);
  tree_view_->setUniformRowHeights(true);
  tree_view_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tree_view_->setSelectionMode(QAbstractItemView::ExtendedSelection);
  tree_view_->setIndentation(12);
  tree_view_->setExpandsOnDoubleClick(true);
  tree_view_->setAnimated(true);
  tree_view_->setHeaderHidden(true);  // Exact match - Blender hides this header

  // Enable hierarchy lines via style
  tree_view_->setStyleSheet(
      "QTreeView::branch:has-siblings:!adjoins-item { border-left: 1px solid #333; }"
      "QTreeView::branch:has-siblings:adjoins-item { border-left: 1px solid #333; }"
      "QTreeView::branch:!has-children:!has-siblings:adjoins-item { border-left: 1px solid #333; }"
      "QTreeView::branch:has-children:!has-siblings:closed, "
      "QTreeView::branch:closed:has-children:has-siblings { image: none; }"
      "QTreeView::branch:open:has-children:!has-siblings, "
      "QTreeView::branch:open:has-children:has-siblings { image: none; }");

  layout->addWidget(tree_view_);

  connect(qt::scene::SCN_notifier::instance(),
          &qt::scene::SCN_notifier::sceneChanged,
          this,
          &OutlinerWidget::refresh_entities);

  filter_timer_ = new QTimer(this);
  filter_timer_->setSingleShot(true);

  connect(filter_timer_, &QTimer::timeout, this, &OutlinerWidget::apply_filter);
  connect(search_bar_, &QLineEdit::textChanged, this, &OutlinerWidget::on_search_text_changed);

  connect(tree_view_->selectionModel(),
          &QItemSelectionModel::selectionChanged,
          [this](const QItemSelection &selected, const QItemSelection &deselected) {
            if (is_refreshing_) {
              return;
            }
            auto &registry = vektor::kernel::ECSRegistry::instance();

            for (const auto &index : deselected.indexes()) {
              if (index.column() != 0)
                continue;
              QModelIndex source_index = proxy_model_->mapToSource(index);
              QStandardItem *item = model_->itemFromIndex(source_index);
              if (item && item->data(Qt::UserRole).isValid()) {
                auto entity = (entt::entity)item->data(Qt::UserRole).toUInt();
                vektor::rna::RNA_ecs_set_selected(&registry, entity, false);
              }
            }

            for (const auto &index : selected.indexes()) {
              if (index.column() != 0)
                continue;
              QModelIndex source_index = proxy_model_->mapToSource(index);
              QStandardItem *item = model_->itemFromIndex(source_index);
              if (item && item->data(Qt::UserRole).isValid()) {
                auto entity = (entt::entity)item->data(Qt::UserRole).toUInt();
                vektor::rna::RNA_ecs_set_selected(&registry, entity, true);
                vektor::rna::RNA_ecs_set_active(&registry, entity, true);
              }
            }
          });

  tree_view_->viewport()->installEventFilter(this);
  refresh_entities();
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

void OutlinerWidget::refresh_entities()
{
  CLOG_INFO(LOG_OUTLINER, "Refreshing entities...");
  is_refreshing_ = true;
  model_->removeRows(0, model_->rowCount());

  // Level 1: Scene Collection
  auto *root_item = new QStandardItem("Scene Collection");
  // root_item->setData(QIcon(":/icons/outliner_scene.png"),
  //  Qt::DecorationRole);  // Placeholder if icons exist
  model_->appendRow({root_item, new QStandardItem("")});

  // Level 2: Collection
  auto *collection_item = new QStandardItem("Collection");
  collection_item->setData(QString("📦"), Qt::DecorationRole);  // Box icon for collection
  root_item->appendRow({collection_item, new QStandardItem("")});

  auto &registry = vektor::kernel::ECSRegistry::instance().registry();
  auto objects_view = registry.view<vektor::dna::Object>();

  // Level 3: Objects
  for (auto entity : objects_view) {
    auto &obj = objects_view.get<vektor::dna::Object>(entity);
    QString name = QString::fromUtf8(obj.id_name);

    auto *name_item = new QStandardItem(name);

    // Set icons based on object type
    QString icon_str = "❓";
    switch (obj.object_type) {
      case DNA_MESH:
        icon_str = "📐";
        break;
      case DNA_CAMERA:
        icon_str = "🎥";
        break;
      case DNA_LIGHT:
        icon_str = "💡";
        break;
      default:
        break;
    }
    name_item->setData(icon_str, Qt::DecorationRole);

    auto *visibility_item = new QStandardItem("👁️");
    visibility_item->setTextAlignment(Qt::AlignCenter);

    name_item->setData((unsigned int)entity, Qt::UserRole);
    visibility_item->setData((unsigned int)entity, Qt::UserRole);

    collection_item->appendRow({name_item, visibility_item});

    // Restore selection state from ECS
    if (vektor::rna::RNA_ecs_is_selected(&vektor::kernel::ECSRegistry::instance(),
                                         (entt::entity)entity))
    {
      QModelIndex source_index = model_->indexFromItem(name_item);
      QModelIndex proxy_index = proxy_model_->mapFromSource(source_index);
      tree_view_->selectionModel()->select(
          proxy_index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
  }

  CLOG_INFO(LOG_OUTLINER, "Refresh complete, row count: %d", model_->rowCount());

  tree_view_->expandAll();
  is_refreshing_ = false;
}

}  // namespace qt::dock
